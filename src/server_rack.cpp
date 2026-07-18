#include "server_rack.h"

#include "bn_keypad.h"
#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_progress_bar.h"
#include "bn_sprite_items_server_rack.h"

namespace server_rack
{

namespace
{

constexpr int needs_reset_tiles_index = 0;
constexpr int reset_done_tiles_index = 1;

constexpr int bar_frame_count = 9;
constexpr bn::fixed bar_offset_y = -28;

[[nodiscard]] int bar_graphics_index(int progress_frames)
{
    if(progress_frames <= 0)
    {
        return 0;
    }

    int filled = (progress_frames * (bar_frame_count - 1) + hold_duration_frames - 1) /
                 hold_duration_frames;

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

entity::entity() :
    _sprite(bn::sprite_items::server_rack.create_sprite(center, needs_reset_tiles_index)),
    _server_reset_done(false),
    _progress_frames(0)
{
    _sync_visual();
}

void entity::set_camera(const bn::camera_ptr& camera)
{
    _sprite.set_camera(camera);

    if(_bar)
    {
        _bar->set_camera(camera);
    }
}

void entity::set_visible(bool visible)
{
    _sprite.set_visible(visible);

    if(! visible)
    {
        _hide_bar();
        _progress_frames = 0;
    }
}

void entity::update(const bn::fixed_point& player_pos, const bn::camera_ptr& camera)
{
    if(_server_reset_done)
    {
        _hide_bar();
        return;
    }

    if(! bn::keypad::a_held() || ! in_interact_range(player_pos))
    {
        _progress_frames = 0;
        _hide_bar();
        return;
    }

    ++_progress_frames;
    _show_bar(_progress_frames, camera);

    if(_progress_frames >= hold_duration_frames)
    {
        _server_reset_done = true;
        _progress_frames = 0;
        _hide_bar();
        _sync_visual();
    }
}

void entity::cancel_hold()
{
    _progress_frames = 0;
    _hide_bar();
}

void entity::reset_shift()
{
    _server_reset_done = false;
    cancel_hold();
    _sync_visual();
}

bool entity::server_reset_done() const
{
    return _server_reset_done;
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

void entity::_sync_visual()
{
    const int tiles_index = _server_reset_done ? reset_done_tiles_index : needs_reset_tiles_index;
    _sprite.set_tiles(bn::sprite_items::server_rack.tiles_item().create_tiles(tiles_index));
}

void entity::_hide_bar()
{
    if(_bar)
    {
        _bar->set_visible(false);
    }
}

void entity::_show_bar(int progress_frames, const bn::camera_ptr& camera)
{
    const bn::fixed_point pos(center.x(), center.y() + bar_offset_y);
    const int graphics_index = bar_graphics_index(progress_frames);

    if(! _bar)
    {
        bn::sprite_ptr bar = bn::sprite_items::progress_bar.create_sprite(pos, graphics_index);
        bar.set_camera(camera);
        bar.set_bg_priority(0);
        bar.set_z_order(-20);
        _bar = bar;
    }
    else
    {
        _bar->set_position(pos);
        _bar->set_camera(camera);
        _bar->set_tiles(bn::sprite_items::progress_bar.tiles_item().create_tiles(graphics_index));
        _bar->set_visible(true);
    }
}

}
