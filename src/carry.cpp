#include "carry.h"

#include "bn_keypad.h"
#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_part_icon.h"

namespace carry
{

namespace
{

constexpr int toner_tiles_index = 0;
constexpr int psu_tiles_index = 1;

// Bottom-right carry HUD (screen-fixed; not camera-attached).
constexpr bn::fixed hud_x = 104;
constexpr bn::fixed hud_y = 64;

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

}

slot::slot() :
    _held(part::none)
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
    _sync_hud();
}

void slot::set(part value)
{
    _held = value;
    _sync_hud();
}

void slot::update_at_closet(const bn::fixed_point& player_pos, const closet::entity& storage)
{
    if(! storage.in_interact_range(player_pos))
    {
        return;
    }

    if(bn::keypad::a_pressed())
    {
        // Replace rule: empty takes toner; carrying swaps toner ↔ PSU.
        if(_held == part::none)
        {
            _held = part::toner;
        }
        else
        {
            _held = other_part(_held);
        }

        _sync_hud();
        return;
    }

    if(bn::keypad::b_pressed() && _held != part::none)
    {
        // Return to infinite closet stock.
        _held = part::none;
        _sync_hud();
    }
}

void slot::update_hud()
{
    if(_hud_icon)
    {
        _hud_icon->set_position(hud_x, hud_y);
    }
}

void slot::_sync_hud()
{
    if(_held == part::none)
    {
        if(_hud_icon)
        {
            _hud_icon->set_visible(false);
        }

        return;
    }

    const int graphics_index = tiles_index_for(_held);

    if(! _hud_icon)
    {
        bn::sprite_ptr icon = bn::sprite_items::part_icon.create_sprite(hud_x, hud_y, graphics_index);
        icon.set_bg_priority(0);
        icon.set_z_order(-30);
        _hud_icon = icon;
    }
    else
    {
        _hud_icon->set_tiles(bn::sprite_items::part_icon.tiles_item().create_tiles(graphics_index));
        _hud_icon->set_position(hud_x, hud_y);
        _hud_icon->set_visible(true);
    }
}

}
