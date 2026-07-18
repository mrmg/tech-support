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

// Must match closet::toner_tiles_index / psu_tiles_index and fault badge art contract.
constexpr int toner_tiles_index = closet::toner_tiles_index;
constexpr int psu_tiles_index = closet::psu_tiles_index;

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

[[nodiscard]] part part_for_bin(const closet::bin& supply_bin)
{
    return supply_bin.is_psu() ? part::psu : part::toner;
}

[[nodiscard]] closet::bin* nearest_bin_in_range(const bn::fixed_point& player_pos,
                                               bn::span<closet::bin> bins)
{
    closet::bin* nearest = nullptr;
    bn::fixed nearest_dist_sq = 0;

    for(closet::bin& supply_bin : bins)
    {
        if(! supply_bin.in_interact_range(player_pos))
        {
            continue;
        }

        const bn::fixed dx = player_pos.x() - supply_bin.position().x();
        const bn::fixed dy = player_pos.y() - supply_bin.position().y();
        const bn::fixed dist_sq = dx * dx + dy * dy;

        if(nearest == nullptr || dist_sq < nearest_dist_sq)
        {
            nearest = &supply_bin;
            nearest_dist_sq = dist_sq;
        }
    }

    return nearest;
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

void slot::update_at_bins(const bn::fixed_point& player_pos, bn::span<closet::bin> bins)
{
    closet::bin* supply_bin = nearest_bin_in_range(player_pos, bins);

    if(supply_bin == nullptr)
    {
        return;
    }

    const part bin_part = part_for_bin(*supply_bin);

    if(bn::keypad::a_pressed())
    {
        if(! inventory::has_stock(bin_part))
        {
            supply_bin->flash_deny();
            return;
        }

        if(_held == bin_part)
        {
            // Already carrying this bin's part — no swap/cycle.
            return;
        }

        // Empty hands: take this bin's part. Holding the other: swap to this bin.
        _held = bin_part;
        sfx::play_pickup();
        _sync_icon();
        return;
    }

    if(bn::keypad::b_pressed() && _held != part::none)
    {
        if(_held != bin_part)
        {
            // Return only to the matching bin.
            supply_bin->flash_deny();
            return;
        }

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
