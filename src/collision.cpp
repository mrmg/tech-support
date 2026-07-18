#include "collision.h"

namespace collision
{

bool overlaps_any(const bn::fixed_rect& box, bn::span<const bn::fixed_rect> solids)
{
    for(const bn::fixed_rect& solid : solids)
    {
        if(box.intersects(solid))
        {
            return true;
        }
    }

    return false;
}

bn::fixed_point move_and_slide(const bn::fixed_point& position, bn::fixed dx, bn::fixed dy,
                               const bn::fixed_size& hitbox_size, bn::span<const bn::fixed_rect> solids)
{
    bn::fixed x = position.x();
    bn::fixed y = position.y();

    if(dx != 0)
    {
        const bn::fixed_rect next_x(x + dx, y, hitbox_size.width(), hitbox_size.height());

        if(! overlaps_any(next_x, solids))
        {
            x += dx;
        }
    }

    if(dy != 0)
    {
        const bn::fixed_rect next_y(x, y + dy, hitbox_size.width(), hitbox_size.height());

        if(! overlaps_any(next_y, solids))
        {
            y += dy;
        }
    }

    return bn::fixed_point(x, y);
}

}
