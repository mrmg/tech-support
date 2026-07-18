#include "bn_core.h"

#include "shift_scene.h"
#include "title_scene.h"

int main()
{
    bn::core::init();

    while(true)
    {
        run_title_scene();
        run_shift_scene();
    }
}
