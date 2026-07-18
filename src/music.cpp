#include "music.h"

#include "bn_music.h"

#if __has_include("bn_music_items.h")
#include "bn_music_items.h"
#define TS_HAS_MUSIC_ITEMS 1
#endif

namespace music
{

void play_bed()
{
#if defined(TS_HAS_MUSIC_ITEMS)
    if(bn::music::playing())
    {
        return;
    }

    // Volume kept moderate so SFX stay audible; loop is Maxmod default.
    bn::music_items::music_shift.play(0.45);
#endif
}

void stop()
{
#if defined(TS_HAS_MUSIC_ITEMS)
    if(bn::music::playing())
    {
        bn::music::stop();
    }
#endif
}

}
