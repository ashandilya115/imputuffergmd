# Input Buffer (Geode mod for Geometry Dash)

Turns a 1-tick frame-perfect jump timing into a 2, 3 or 4 tick window (configurable up to 8) by buffering early clicks.

## What it does

- A click that does nothing yet (too early for an orb or a landing) stays armed for the rest of the window.
- Each tick the armed click is re-sent as a fresh press. The first time it actually does something, the buffer clears.
- If the window runs out, the click is dropped.
- Ship and wave are skipped by default, since they are hold-based.

## Settings

| Setting | Default | Notes |
|---|---|---|
| Enable Input Buffer | on | master switch |
| Buffer Window (ticks) | 3 | 1 = vanilla, 2-4 = wider windows, max 8. One tick is about 4.17ms at 240 TPS |
| Buffer Player 2 | on | dual-mode second player |
| Skip Ship and Wave | on | no buffering in hold-based modes |

## Before you build

1. In `mod.json`, change `id` (`yourname.input-buffer`) and `developer` to your own. The id must look like `name.modname` in lowercase.
2. Check `geode` and `gd` versions in `mod.json` against what you have installed (this project targets Geode 5.x and GD 2.2081).

## Option A: build with GitHub (no local setup)

1. Create a new GitHub repo and upload everything in this folder.
2. Open the **Actions** tab and run **Build Binaries** (it also runs on every push).
3. When it finishes, download the **Build Output** artifact. It contains the `.geode` file.
4. Put the `.geode` file in your Geode `mods` folder (or use Geode's "Install from file") and restart the game.

The workflow builds for Windows, macOS, Android 32/64 and iOS.

## Option B: build locally

1. Install the Geode CLI and SDK: https://docs.geode-sdk.org/getting-started/
2. Run `geode sdk install` and `geode sdk install-binary`.
3. In this folder run `geode build`.
4. The `.geode` file appears in `build/`. Install it as above.

## Tuning

Start with a window of 3. If orbs or jumps fire when you did not want them to, lower it; if you still miss tight timings, raise it. Window 1 behaves like vanilla.

## Notes

- Hooks: `GJBaseGameLayer::handleButton`, `GJBaseGameLayer::processCommands`, `PlayLayer::resetLevel`.
- The "did the click do anything" check compares player state (y velocity, gravity direction, dashing, on-ground) before and after a click. If your Geode bindings name any of these members differently, the compiler will tell you which line to fix in `src/BufferState.hpp`.
- This makes levels easier. Don't use it for verifications, leaderboards or records.
