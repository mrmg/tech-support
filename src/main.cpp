#include "bn_core.h"

#include "campaign.h"
#include "shift_scene.h"
#include "title_scene.h"

int main()
{
    bn::core::init();

    // Fresh boot = new campaign run (Day 1). Title→shift reads campaign::current_day();
    // returning via B keeps session day for later resume (C-05).
    campaign::reset();

    while(true)
    {
        run_title_scene();
        run_shift_scene();
    }
}
