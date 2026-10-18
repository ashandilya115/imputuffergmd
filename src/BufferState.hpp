#pragma once

#include <Geode/Geode.hpp>

// Shared state + helpers for the input buffer mod.
namespace ib {

	// PlayerButton::Jump
	constexpr int kJump = 1;

	struct Pending {
		bool active = false;
		int ticksLeft = 0;
	};

	struct State {
		// index 0 = player 1, index 1 = player 2
		Pending pending[2];
		bool held[2] = {false, false};
		bool injecting = false;

		void clear() {
			pending[0] = {};
			pending[1] = {};
			held[0] = false;
			held[1] = false;
			injecting = false;
		}
	};

	inline State& state() {
		static State s;
		return s;
	}

	// ---- settings ----

	inline bool isEnabled() {
		return geode::Mod::get()->getSettingValue<bool>("enabled");
	}

	inline int windowTicks() {
		int v = static_cast<int>(geode::Mod::get()->getSettingValue<int64_t>("buffer-ticks"));
		if (v < 1) v = 1;
		if (v > 8) v = 8;
		return v;
	}

	inline bool buffersPlayer2() {
		return geode::Mod::get()->getSettingValue<bool>("buffer-player2");
	}

	inline bool skipHeldModes() {
		return geode::Mod::get()->getSettingValue<bool>("skip-held-modes");
	}

	// ---- player snapshot, used to detect whether a click "did something" ----

	struct Snap {
		double yVel;
		bool upsideDown;
		bool dashing;
		bool onGround;
	};

	inline Snap takeSnap(PlayerObject* p) {
		return Snap{
			p->m_yVelocity,
			p->m_isUpsideDown,
			p->m_isDashing,
			p->m_isOnGround
		};
	}

	inline bool changed(Snap const& a, Snap const& b) {
		return a.yVel != b.yVel
			|| a.upsideDown != b.upsideDown
			|| a.dashing != b.dashing
			|| a.onGround != b.onGround;
	}

	// Ship and wave are hold-based: a click is never "missed" there.
	inline bool isHeldMode(PlayerObject* p) {
		return skipHeldModes() && (p->m_isShip || p->m_isDart);
	}
}
