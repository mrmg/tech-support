#include "printer.h"

#include "bn_array.h"
#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_part_icon.h"
#include "bn_sprite_items_printer.h"
#include "bn_sprite_items_ticket_badge.h"

namespace printer
{

namespace
{

constexpr bn::array<bn::fixed_rect, count> solid_table = {
    solid_box_at(0),
    solid_box_at(1),
};

constexpr int idle_tiles_index = 0;
constexpr int broken_tiles_index = 1;
constexpr int broken_flash_tiles_index = 2;
constexpr int flash_period_base = 20;
constexpr int flash_period_min = 6;
constexpr int flash_period_urgency_step = 1;
constexpr int no_badge_tiles = -1;

}

bn::span<const definition> definitions()
{
    return bn::span<const definition>(definition_table.data(), definition_table.size());
}

bn::span<const bn::fixed_rect> solid_boxes()
{
    return bn::span<const bn::fixed_rect>(solid_table.data(), solid_table.size());
}

entity::entity(int index) :
    _index(index),
    _state(visual_state::idle),
    _urgency(0),
    _flash_frame(0),
    _badge_tiles(no_badge_tiles),
    _visible(true),
    _sprite(bn::sprite_items::printer.create_sprite(definition_table[index].center))
{
    BN_ASSERT(index >= 0 && index < count, "printer index out of range");
    _sync_sprite();
}

void entity::set_camera(const bn::camera_ptr& camera)
{
    _sprite.set_camera(camera);

    if(_badge)
    {
        _badge->set_camera(camera);
    }
}

void entity::set_broken(bool broken)
{
    set_visual_state(broken ? visual_state::broken : visual_state::idle);
}

void entity::set_visual_state(visual_state state)
{
    if(_state == state)
    {
        return;
    }

    _state = state;
    _flash_frame = 0;
    _sync_sprite();
}

void entity::set_urgency(int urgency_level)
{
    if(urgency_level < 0)
    {
        urgency_level = 0;
    }

    if(_urgency == urgency_level)
    {
        return;
    }

    _urgency = urgency_level;
}

void entity::set_type_badge(int graphics_index)
{
    BN_ASSERT(graphics_index >= desk::badge_reboot && graphics_index <= desk::badge_server,
              "badge graphics_index out of range");

    if(_badge_tiles == graphics_index)
    {
        _sync_badge();
        return;
    }

    _badge_tiles = graphics_index;
    _sync_badge();
}

void entity::clear_type_badge()
{
    if(_badge_tiles == no_badge_tiles)
    {
        return;
    }

    _badge_tiles = no_badge_tiles;
    _sync_badge();
}

void entity::set_visible(bool visible)
{
    if(_visible == visible)
    {
        return;
    }

    _visible = visible;
    _sprite.set_visible(visible);
    _sync_badge();
}

bool entity::is_broken() const
{
    return _state == visual_state::broken;
}

visual_state entity::state() const
{
    return _state;
}

int entity::_flash_period() const
{
    int period = flash_period_base - _urgency * flash_period_urgency_step;

    if(period < flash_period_min)
    {
        period = flash_period_min;
    }

    return period;
}

void entity::update()
{
    if(_state != visual_state::broken)
    {
        return;
    }

    const int period = _flash_period();
    ++_flash_frame;

    if(_flash_frame >= period)
    {
        _flash_frame = 0;
    }

    if(_flash_frame == 0 || _flash_frame == period / 2)
    {
        _sync_sprite();
    }
}

int entity::index() const
{
    return _index;
}

int entity::ticket_target_id() const
{
    return target_id(_index);
}

bn::fixed_point entity::position() const
{
    return definition_table[_index].center;
}

bn::fixed_rect entity::solid_box() const
{
    return solid_box_at(_index);
}

bn::fixed_rect entity::interact_range() const
{
    return interact_range_at(_index);
}

bool entity::in_interact_range(const bn::fixed_point& point) const
{
    return interact_range().contains(point);
}

bn::sprite_ptr& entity::sprite()
{
    return _sprite;
}

void entity::_sync_sprite()
{
    int tiles_index = idle_tiles_index;

    if(_state == visual_state::broken)
    {
        const bool flash_on = _flash_frame >= _flash_period() / 2;
        tiles_index = flash_on ? broken_flash_tiles_index : broken_tiles_index;
    }

    _sprite.set_tiles(bn::sprite_items::printer.tiles_item().create_tiles(tiles_index));
}

bn::fixed_point entity::_badge_position() const
{
    const bn::fixed_point center = definition_table[_index].center;
    return bn::fixed_point(center.x(), center.y() - type_badge_y_offset);
}

void entity::_sync_badge()
{
    const bool show = _visible && _badge_tiles != no_badge_tiles;

    if(! show)
    {
        if(_badge)
        {
            _badge->set_visible(false);
        }

        return;
    }

    const bn::fixed_point pos = _badge_position();
    // J-04: toner badge uses part_icon frame 0 (same glyph as toner bin + carry).
    const bool part_badge = _badge_tiles == badge_toner;
    const int graphics_index = part_badge ? 0 : _badge_tiles;

    if(! _badge)
    {
        bn::sprite_ptr badge =
            part_badge ? bn::sprite_items::part_icon.create_sprite(pos, graphics_index)
                       : bn::sprite_items::ticket_badge.create_sprite(pos, graphics_index);
        badge.set_bg_priority(_sprite.bg_priority());
        badge.set_z_order(-5);

        if(const bn::optional<bn::camera_ptr> camera = _sprite.camera())
        {
            badge.set_camera(*camera);
        }

        _badge = badge;
    }
    else
    {
        if(part_badge)
        {
            _badge->set_tiles(bn::sprite_items::part_icon.tiles_item().create_tiles(graphics_index));
        }
        else
        {
            _badge->set_tiles(bn::sprite_items::ticket_badge.tiles_item().create_tiles(graphics_index));
        }

        _badge->set_position(pos);
        _badge->set_visible(true);
    }
}

}
