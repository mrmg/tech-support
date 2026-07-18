#include "coffee.h"

#include "bn_array.h"
#include "bn_bpp_mode.h"
#include "bn_color.h"
#include "bn_keypad.h"
#include "bn_sprite_items_coffee.h"
#include "bn_sprite_items_progress_bar.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_tiles_ptr.h"

namespace coffee
{

namespace
{

constexpr bn::array<bn::fixed_rect, 1> solid_table = {
    solid_box(),
};

constexpr int bar_frame_count = 9;
constexpr bn::fixed bar_y_offset = -20;

// Brighter machine ink while in interact range (index 0 = transparent key).
constexpr bn::color highlight_colors[] = {
    bn::color(31, 0, 31),
    bn::color(14, 8, 4),
    bn::color(20, 12, 6),
    bn::color(26, 16, 8),
    bn::color(8, 8, 10),
    bn::color(18, 18, 20),
    bn::color(26, 26, 28),
    bn::color(31, 12, 8),
    bn::color(31, 28, 12),
    bn::color(31, 31, 31),
    bn::color(12, 26, 12),
    bn::color(4, 4, 5),
    bn::color(28, 22, 16),
    bn::color(16, 10, 6),
    bn::color(8, 5, 3),
    bn::color(0, 0, 0),
};

constexpr bn::sprite_palette_item highlight_palette_item(highlight_colors, bn::bpp_mode::BPP_4);

[[nodiscard]] int bar_graphics_index(int progress_frames)
{
    if(progress_frames <= 0)
    {
        return 0;
    }

    int filled = (progress_frames * (bar_frame_count - 1) + brew_duration_frames - 1) /
                 brew_duration_frames;

    if(filled < 1)
    {
        filled = 1;
    }

    if(filled > bar_frame_count - 1)
    {
        filled = bar_frame_count - 1;
    }

    return filled;
}

}

bn::span<const bn::fixed_rect> solid_boxes()
{
    return bn::span<const bn::fixed_rect>(solid_table.data(), solid_table.size());
}

station::station() :
    _visible(true),
    _in_range(false),
    _brew_frames(0),
    _sprite(bn::sprite_items::coffee.create_sprite(center))
{
}

void station::set_camera(const bn::camera_ptr& camera)
{
    _sprite.set_camera(camera);

    if(_bar)
    {
        _bar->set_camera(camera);
    }
}

void station::set_visible(bool visible)
{
    _visible = visible;
    _sprite.set_visible(visible);

    if(! visible)
    {
        cancel_brew();
        set_in_range(false);
    }
}

void station::set_in_range(bool in_range)
{
    if(_in_range == in_range)
    {
        return;
    }

    _in_range = in_range;
    _apply_body_palette();
}

void station::cancel_brew()
{
    _brew_frames = 0;
    _hide_bar();
}

bool station::update_brew(const bn::fixed_point& player_pos, const bn::camera_ptr& camera)
{
    if(! _visible || ! in_interact_range(player_pos) || ! bn::keypad::a_held())
    {
        cancel_brew();
        return false;
    }

    ++_brew_frames;
    _show_bar(_brew_frames, camera);

    if(_brew_frames < brew_duration_frames)
    {
        return false;
    }

    cancel_brew();
    return true;
}

bn::fixed_point station::position() const
{
    return center;
}

bn::fixed_rect station::solid() const
{
    return solid_box();
}

bn::fixed_rect station::interact() const
{
    return interact_range();
}

bool station::in_interact_range(const bn::fixed_point& point) const
{
    return interact_range().contains(point);
}

bool station::brewing() const
{
    return _brew_frames > 0;
}

bn::sprite_ptr& station::sprite()
{
    return _sprite;
}

void station::_apply_body_palette()
{
    if(_in_range)
    {
        _sprite.set_palette(highlight_palette_item.create_palette());
    }
    else
    {
        _sprite.set_palette(bn::sprite_items::coffee.palette_item());
    }
}

void station::_hide_bar()
{
    if(_bar)
    {
        _bar->set_visible(false);
    }
}

void station::_show_bar(int progress_frames, const bn::camera_ptr& camera)
{
    const int graphics_index = bar_graphics_index(progress_frames);
    const bn::fixed_point pos(center.x(), center.y() + bar_y_offset);

    if(! _bar)
    {
        bn::sprite_ptr bar = bn::sprite_items::progress_bar.create_sprite(pos, graphics_index);
        bar.set_bg_priority(0);
        bar.set_z_order(-40);
        bar.set_camera(camera);
        _bar = bar;
    }
    else
    {
        _bar->set_position(pos);
        _bar->set_tiles(bn::sprite_items::progress_bar.tiles_item().create_tiles(graphics_index));
        _bar->set_camera(camera);
        _bar->set_visible(true);
    }
}

}
