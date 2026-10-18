#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "BufferState.hpp"

using namespace geode::prelude;

/*
 * How it works
 * ------------
 * Every real click reaches the player through GJBaseGameLayer::handleButton,
 * which the game calls from inside a physics tick.
 *
 *  1. When a jump press comes in, we let the game handle it normally and
 *     compare the player's state before/after. If the click did something
 *     (jump, orb, gravity flip, dash...) there is nothing to buffer.
 *  2. If the click did nothing (you were a few ticks too early for an orb,
 *     or too early to land a jump), we "arm" it for N-1 more ticks.
 *  3. Every tick, before the physics step, an armed click is re-sent as a
 *     fresh press. The moment one of those re-sends actually does
 *     something, the buffer is cleared. If the window runs out, it is
 *     dropped.
 *
 * So a timing that needed the click on exactly one tick now works if you
 * click anywhere in the N ticks before it.
 */

class $modify(BufferedGameLayer, GJBaseGameLayer) {
	void handleButton(bool down, int button, bool isPlayer1) {
		auto& st = ib::state();

		// Only touch real gameplay (not the editor), only the jump button,
		// and never re-enter from our own injected clicks.
		if (st.injecting || button != ib::kJump || !PlayLayer::get() || !ib::isEnabled()) {
			GJBaseGameLayer::handleButton(down, button, isPlayer1);
			return;
		}

		int idx = isPlayer1 ? 0 : 1;
		PlayerObject* player = isPlayer1 ? m_player1 : m_player2;

		if (!player || (!isPlayer1 && !ib::buffersPlayer2())) {
			GJBaseGameLayer::handleButton(down, button, isPlayer1);
			return;
		}

		if (!down) {
			// Releasing does not cancel an armed click, so quick taps still work.
			st.held[idx] = false;
			GJBaseGameLayer::handleButton(down, button, isPlayer1);
			return;
		}

		st.held[idx] = true;

		ib::Snap before = ib::takeSnap(player);
		GJBaseGameLayer::handleButton(down, button, isPlayer1);
		ib::Snap after = ib::takeSnap(player);

		if (ib::changed(before, after) || ib::isHeldMode(player)) {
			// The click was used (or does not need buffering).
			st.pending[idx] = {};
			return;
		}

		// The click did nothing yet, arm it for the rest of the window.
		st.pending[idx].active = true;
		st.pending[idx].ticksLeft = ib::windowTicks() - 1;
	}

	void bufferTick() {
		auto& st = ib::state();

		if (!PlayLayer::get()) {
			st.clear();
			return;
		}

		for (int i = 0; i < 2; ++i) {
			auto& p = st.pending[i];
			if (!p.active) continue;

			if (p.ticksLeft <= 0) {
				p.active = false;
				continue;
			}
			--p.ticksLeft;

			bool isP1 = (i == 0);
			PlayerObject* player = isP1 ? m_player1 : m_player2;
			if (!player) {
				p.active = false;
				continue;
			}

			ib::Snap before = ib::takeSnap(player);

			// Calling GJBaseGameLayer::handleButton here calls the original
			// function, so these do not go back through our hook.
			st.injecting = true;
			GJBaseGameLayer::handleButton(false, ib::kJump, isP1);
			GJBaseGameLayer::handleButton(true, ib::kJump, isP1);
			if (!st.held[i]) {
				// The real button is already up, so end the fake click too.
				GJBaseGameLayer::handleButton(false, ib::kJump, isP1);
			}
			st.injecting = false;

			if (ib::changed(before, ib::takeSnap(player))) {
				p.active = false; // click was consumed
			}
		}
	}

	void processCommands(float dt, bool isHalfTick, bool isLastTick) {
		auto& st = ib::state();

		// Count whole ticks only, and skip all work when nothing is armed.
		if (!isHalfTick && (st.pending[0].active || st.pending[1].active)) {
			this->bufferTick();
		}

		GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
	}
};

class $modify(BufferedPlayLayer, PlayLayer) {
	void resetLevel() {
		ib::state().clear();
		PlayLayer::resetLevel();
	}
};
