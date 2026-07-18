#include "sfx.h"

#include "bn_sound.h"

#if __has_include("bn_sound_items.h")
#include "bn_sound_items.h"
#define TS_HAS_SOUND_ITEMS 1
#endif

namespace sfx
{

namespace
{

#if defined(TS_HAS_SOUND_ITEMS)

void play(bn::sound_item item)
{
    // Channel-full → nullopt; never assert. Missing wavs omit items at codegen time.
    bn::sound::play_optional(item);
}

#endif

}

void play_pickup()
{
#if defined(TS_HAS_SOUND_ITEMS)
    play(bn::sound_items::sfx_pickup);
#endif
}

void play_fix_complete()
{
#if defined(TS_HAS_SOUND_ITEMS)
    play(bn::sound_items::sfx_fix_complete);
#endif
}

void play_ticket_spawn()
{
#if defined(TS_HAS_SOUND_ITEMS)
    play(bn::sound_items::sfx_ticket_spawn);
#endif
}

void play_ui_open()
{
#if defined(TS_HAS_SOUND_ITEMS)
    play(bn::sound_items::sfx_ui_open);
#endif
}

void play_shift_end()
{
#if defined(TS_HAS_SOUND_ITEMS)
    play(bn::sound_items::sfx_shift_end);
#endif
}

void play_sacked()
{
#if defined(TS_HAS_SOUND_ITEMS)
    play(bn::sound_items::sfx_sacked);
#endif
}

}
