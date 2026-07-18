#include "closet.h"

#include "bn_bpp_mode.h"
#include "bn_color.h"
#include "bn_sprite_palette_item.h"

#include "bn_sprite_items_closet.h"

namespace closet
{

namespace
{

// Brighter cupboard ink so "in range" reads without a second art frame.
// Index order matches grit export of graphics/closet.bmp (0 = transparent key).
constexpr bn::color highlight_colors[] = {
    bn::color(31, 0, 31),
    bn::color(22, 16, 10),
    bn::color(16, 11, 6),
    bn::color(10, 12, 14),
    bn::color(12, 26, 30),
    bn::color(6, 16, 20),
    bn::color(31, 10, 8),
    bn::color(24, 6, 4),
    bn::color(30, 30, 30),
    bn::color(31, 31, 16),
    bn::color(18, 12, 8),
    bn::color(31, 31, 31),
    bn::color(26, 26, 28),
    bn::color(16, 16, 18),
    bn::color(31, 24, 10),
    bn::color(12, 12, 14),
};

constexpr bn::sprite_palette_item highlight_palette_item(highlight_colors, bn::bpp_mode::BPP_4);

}

entity::entity() :
    _sprite(bn::sprite_items::closet.create_sprite(center)),
    _in_range(false)
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

void entity::set_in_range(bool in_range)
{
    if(_in_range == in_range)
    {
        return;
    }

    _in_range = in_range;
    _sprite.set_palette(in_range ? highlight_palette_item : bn::sprite_items::closet.palette_item());
}

bn::sprite_ptr& entity::sprite()
{
    return _sprite;
}

}
