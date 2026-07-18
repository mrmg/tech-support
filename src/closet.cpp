#include "closet.h"

#include "bn_sprite_items_closet.h"

namespace closet
{

entity::entity() :
    _sprite(bn::sprite_items::closet.create_sprite(center))
{
}

void entity::set_camera(const bn::camera_ptr& camera)
{
    _sprite.set_camera(camera);
}

bn::fixed_point entity::position() const
{
    return center;
}

bn::fixed_rect entity::solid() const
{
    return solid_box();
}

bn::fixed_rect entity::interact() const
{
    return interact_range();
}

bool entity::in_interact_range(const bn::fixed_point& point) const
{
    return interact_range().contains(point);
}

bn::sprite_ptr& entity::sprite()
{
    return _sprite;
}

}
