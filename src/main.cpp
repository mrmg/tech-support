#include "bn_core.h"

#include "campaign.h"
#include "shift_scene.h"
#include "title_scene.h"

int main()
{
    bn::core::init();

    // Fresh boot = Day 1. Returning to title via B must NOT reset (C-05 resume).
    // Explicit new game: title SELECT. Campaign-complete restart: shift results path.
    campaign::reset();

    while(true)
    {
        run_title_scene();
        // Day intro runs inside shift_scene before each shift.
        run_shift_scene();
    }
}
