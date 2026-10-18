# Input Buffer

Turns 1-tick frame-perfect jump timings into a **2, 3 or 4 tick window** (up to 8) by buffering early clicks.

If you click a few ticks before an orb or a landing and nothing happens yet, the click stays armed and fires on the first tick it can actually do something. If the window runs out, it is dropped.

## Settings

- **Enable Input Buffer** - on/off.
- **Buffer Window (ticks)** - 1 = vanilla, 2-4 = wider timing windows. One tick is about 4.17ms at 240 TPS.
- **Buffer Player 2** - also buffer dual-mode player 2.
- **Skip Ship and Wave** - those modes are hold-based, so there is nothing to buffer.

## Notes

This makes levels easier. Don't use it for verifications, leaderboards or records.
