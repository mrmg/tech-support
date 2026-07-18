#ifndef MUSIC_H
#define MUSIC_H

// Thin wrappers around bn::music_items — looping Direct Sound bed (Maxmod).
// If audio/*.mod (etc.) are absent, bn_music_items.h is not generated and
// these become no-ops so the game still links.
namespace music
{

// Start the placeholder shift/title bed if nothing is already playing.
void play_bed();

void stop();

}

#endif
