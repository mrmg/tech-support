#ifndef SFX_H
#define SFX_H

// Thin wrappers around bn::sound_items — play_optional so full channels stay silent
// (no assert). If audio/*.wav are absent, bn_sound_items.h is not generated and
// these become no-ops so the game still links.
namespace sfx
{

void play_pickup();
void play_fix_complete();
void play_ticket_spawn();
void play_ui_open();
void play_shift_end();
void play_sacked();

}

#endif
