#ifndef SACKED_SCENE_H
#define SACKED_SCENE_H

// G-03: campaign-ending sacked game over (distinct from per-day fail / "don't come back tomorrow").
// Caller resets campaign + reputation + inventory after dismiss, then returns to title.
void run_sacked_scene();

#endif
