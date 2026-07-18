#include "carry.h"

#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_part_icon.h"

#include "inventory.h"
#include "sfx.h"

namespace carry
{

namespace
{

constexpr int toner_tiles_index = 0;
constexpr int psu_tiles_index = 1;

// Above-head offset from player center (16×16 sprite).
constexpr bn::fixed carry_offset_y = -14;
constexpr bn::fixed bob_amplitude = 1;
constexpr int bob_lut_step = 48;

[[nodiscard]] int tiles_index_for(part value)
{
    switch(value)
    {
    case part::toner:
        return toner_tiles_index;

    case part::psu:
        return psu_tiles_index;

    case part::none:
    default:
        return toner_tiles_index;
    }
}

[[nodiscard]] part other_part(part value)
{
    switch(value)
    {
    case part::toner:
        return part::psu;

    case part::psu:
        return part::toner;

    case part::none:
    default:
        return part::toner;
    }
}

// Empty hands: prefer toner, else PSU, else none (both out of stock).
[[nodiscard]] part first_available_part()
{
    if(inventory::has_stock(part::toner))
    {
        return part::toner;
    }

    if(inventory::has_stock(part::psu))
    {
        return part::psu;
    }

    return part::none;
}

[[nodiscard]] bn::fixed_point icon_position(const bn::fixed_point& player_pos, int bob_angle)
{
    const bn::fixed bob = bn::lut_sin(bob_angle) * bob_amplitude;
    return bn::fixed_point(player_pos.x(), player_pos.y() + carry_offset_y + bob);
}

}

slot::slot() :
    _held(part::none),
    _bob_frame(0)
{
}

part slot::held() const
{
    return _held;
}

bool slot::empty() const
{
    return _held == part::none;
}

void slot::clear()
{
    _held = part::none;
    _sync_icon();
}

void slot::set(part value)
{
    _held = value;
    _sync_icon();
}

void slot::set_camera(const bn::camera_ptr& camera)
{
    _camera = camera;

    if(_icon)
    {
        _icon->set_camera(camera);
    }
}

void slot::update_at_closet(const bn::fixed_point& player_pos, const closet::entity& storage)
{
    if(! storage.in_interact_range(player_pos))
    {
        return;
    }

    if(bn::keypad::a_pressed())
    {
        if(_held == part::none)
        {
            const part take = first_available_part();

            if(take == part::none)
            {
                // Both part types at zero stock — pickup blocked.
                return;
            }

            _held = take;
        }
        else
        {
            const part swap_to = other_part(_held);

            if(! inventory::has_stock(swap_to))
            {
                // Target type exhausted — keep current part.
                return;
            }

            _held = swap_to;
        }

        sfx::play_pickup();
        _sync_icon();
        return;
    }

    if(bn::keypad::b_pressed() && _held != part::none)
    {
        // Return to hands; stock unchanged until install consumes.
        _held = part::none;
        _sync_icon();
    }
}

void slot::update_follow(const bn::fixed_point& player_pos)
{
    if(! _icon || _held == part::none)
    {
        return;
    }

    _bob_frame = (_bob_frame + bob_lut_step) & 2047;
    _icon->set_position(icon_position(player_pos, _bob_frame));
}

void slot::_sync_icon()
{
    if(_held == part::none)
    {
        if(_icon)
        {
            _icon->set_visible(false);
        }

        return;
    }

    const int graphics_index = tiles_index_for(_held);

    if(! _icon)
    {
        bn::sprite_ptr icon = bn::sprite_items::part_icon.create_sprite(0, 0, graphics_index);
        icon.set_bg_priority(0);
        icon.set_z_order(-30);

        if(_camera)
        {
            icon.set_camera(*_camera);
        }

        _icon = icon;
    }
    else
    {
        _icon->set_tiles(bn::sprite_items::part_icon.tiles_item().create_tiles(graphics_index));
        _icon->set_visible(true);
    }
}

}
