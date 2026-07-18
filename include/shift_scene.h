#ifndef SHIFT_SCENE_H
#define SHIFT_SCENE_H

// Shift scene: office loop until title exit or G-03 sacked game over.
// Results A retries / continues; B returns to title (session day kept).
// At sack threshold after results dismiss → sacked scene + full campaign reset → title.
void run_shift_scene();

#endif
