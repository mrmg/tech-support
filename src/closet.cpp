#include "closet.h"

#include "bn_assert.h"
#include "bn_bpp_mode.h"
#include "bn_color.h"
#include "bn_sprite_items_closet.h"
#include "bn_sprite_items_part_icon.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_string.h"

#include "common_variable_8x16_sprite_font.h"
#include "inventory.h"

namespace closet
{

namespace
{

constexpr bn::array<bn::fixed_rect, count> solid_table = {
    solid_box_at(0),
    solid_box_at(1),
};

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

// I-05 deny red — brief body tint when pickup/return is blocked.
constexpr int deny_flash_frames = 14;
constexpr bn::fixed deny_fade_intensity = 0.85;
constexpr bn::color deny_fade_color(31, 4, 4);

constexpr bn::fixed part_icon_y_offset = -6;
constexpr bn::fixed count_y_offset = 14;

[[nodiscard]] carry::part part_for_index(int index)
{
    return index == psu_index ? carry::part::psu : carry::part::toner;
}

[[nodiscard]] int tiles_for_index(int index)
{
    return index == psu_index ? psu_tiles_index : toner_tiles_index;
}

}

bn::span<const bn::fixed_rect> solid_boxes()
{
    return bn::span<const bn::fixed_rect>(solid_table.data(), solid_table.size());
}

bin::bin(int index) :
    _index(index),
    _visible(true),
    _in_range(false),
    _deny_frames(0),
    _shown_stock(-1),
    _sprite(bn::sprite_items::closet.create_sprite(centers[index])),
    _count_text(common::variable_8x16_sprite_font)
{
    BN_ASSERT(index >= 0 && index < count, "closet bin index out of range");
    _count_text.set_bg_priority(0);
    _count_text.set_center_alignment();
    _sync_part_icon(inventory::stock_of(part_for_index(_index)));
    _sync_count_label(inventory::stock_of(part_for_index(_index)));
    _shown_stock = inventory::stock_of(part_for_index(_index));
}

void bin::set_camera(const bn::camera_ptr& camera)
{
    _camera = camera;
    _sprite.set_camera(camera);

    if(_part_icon)
    {
        _part_icon->set_camera(camera);
    }

    for(bn::sprite_ptr& glyph : _count_sprites)
    {
        glyph.set_camera(camera);
    }
}

void bin::set_visible(bool visible)
{
    if(_visible == visible)
    {
        return;
    }

    _visible = visible;
    _sprite.set_visible(visible);

    if(_part_icon)
    {
        _part_icon->set_visible(visible && _shown_stock > 0);
    }

    for(bn::sprite_ptr& glyph : _count_sprites)
    {
        glyph.set_visible(visible);
    }
}

void bin::set_in_range(bool in_range)
{
    if(_in_range == in_range)
    {
        return;
    }

    _in_range = in_range;

    if(_deny_frames <= 0)
    {
        _apply_body_palette();
    }
}

void bin::flash_deny()
{
    _deny_frames = deny_flash_frames;
    bn::sprite_palette_ptr deny_palette =
        bn::sprite_palette_ptr::create_new(bn::sprite_items::closet.palette_item());
    deny_palette.set_fade(deny_fade_color, deny_fade_intensity);
    _sprite.set_palette(deny_palette);
}

void bin::sync_stock()
{
    const int stock = inventory::stock_of(part_for_index(_index));

    if(stock == _shown_stock)
    {
        return;
    }

    _shown_stock = stock;
    _sync_part_icon(stock);
    _sync_count_label(stock);
}

void bin::update()
{
    if(_deny_frames <= 0)
    {
        return;
    }

    --_deny_frames;

    if(_deny_frames <= 0)
    {
        _apply_body_palette();
    }
}

int bin::index() const
{
    return _index;
}

bool bin::is_toner() const
{
    return _index == toner_index;
}

bool bin::is_psu() const
{
    return _index == psu_index;
}

int bin::part_tiles_index() const
{
    return tiles_for_index(_index);
}

bn::fixed_point bin::position() const
{
    return centers[_index];
}

bn::fixed_rect bin::solid() const
{
    return solid_box_at(_index);
}

bn::fixed_rect bin::interact() const
{
    return interact_range_at(_index);
}

bool bin::in_interact_range(const bn::fixed_point& point) const
{
    return interact_range_at(_index).contains(point);
}

bn::sprite_ptr& bin::sprite()
{
    return _sprite;
}

void bin::_apply_body_palette()
{
    _sprite.set_palette(_in_range ? highlight_palette_item : bn::sprite_items::closet.palette_item());
}

void bin::_sync_part_icon(int stock)
{
    const bool show = _visible && stock > 0;
    const bn::fixed_point pos(centers[_index].x(), centers[_index].y() + part_icon_y_offset);
    const int graphics_index = tiles_for_index(_index);

    if(! show)
    {
        if(_part_icon)
        {
            _part_icon->set_visible(false);
        }

        return;
    }

    if(! _part_icon)
    {
        bn::sprite_ptr icon = bn::sprite_items::part_icon.create_sprite(pos, graphics_index);
        icon.set_bg_priority(0);
        icon.set_z_order(-25);

        if(_camera)
        {
            icon.set_camera(*_camera);
        }

        _part_icon = icon;
    }
    else
    {
        _part_icon->set_tiles(bn::sprite_items::part_icon.tiles_item().create_tiles(graphics_index));
        _part_icon->set_position(pos);
        _part_icon->set_visible(true);
    }
}

void bin::_sync_count_label(int stock)
{
    _count_sprites.clear();

    if(! _visible)
    {
        return;
    }

    int shown = stock;

    if(shown < 0)
    {
        shown = 0;
    }

    if(shown > 99)
    {
        shown = 99;
    }

    bn::string<4> label = bn::to_string<4>(shown);
    const bn::fixed_point pos(centers[_index].x(), centers[_index].y() + count_y_offset);
    _count_text.generate(pos.x(), pos.y(), label, _count_sprites);

    if(_camera)
    {
        for(bn::sprite_ptr& glyph : _count_sprites)
        {
            glyph.set_camera(*_camera);
            glyph.set_z_order(-24);
        }
    }
}

void update_range_highlights(const bn::fixed_point& player_pos, bn::span<bin> bins)
{
    bin* nearest = nullptr;
    bn::fixed nearest_dist_sq = 0;

    for(bin& supply_bin : bins)
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

    for(bin& supply_bin : bins)
    {
        supply_bin.set_in_range(&supply_bin == nearest);
    }
}

}
