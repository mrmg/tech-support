#include "bn_core.h"

#include "campaign.h"
#include "inventory.h"
#include "reputation.h"
#include "shift_scene.h"
#include "title_scene.h"

int main()
{
    bn::core::init();

    // Fresh boot = Day 1 + starting stock/budget/anger. Title B resume must NOT reset (C-05).
    // Explicit new game: title SELECT. Campaign-complete restart: shift results path.
    campaign::reset();
    inventory::reset();
    reputation::reset();

    while(true)
    {
        run_title_scene();
        // Day intro runs inside shift_scene before each shift.
        run_shift_scene();
    }
}
