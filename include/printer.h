#ifndef PRINTER_H
#define PRINTER_H

#include "bn_array.h"
#include "bn_camera_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"

#include "desk.h"

// Office floor printers: solids, interact ranges, idle/broken visuals (J-03).
// Toner tickets bind here — never to computer desks.
namespace printer
{

constexpr int count = 2;

// Ticket target IDs: desks use 0..desk::count-1; printers follow.
constexpr int target_id_base = desk::count;

constexpr bn::fixed solid_width = 36;
constexpr bn::fixed solid_height = 28;

constexpr bn::fixed interact_width = 52;
constexpr bn::fixed interact_height = 44;

constexpr bn::fixed type_badge_y_offset = 22;

// Toner badge selector → part_icon frame 0 (shared with bin + carry — J-04).
constexpr int badge_toner = desk::badge_toner;

enum class visual_state
{
    idle,
    broken,
};

struct definition
{
    bn::fixed_point center;
};

// Opposite ends of the office floor, between the desk rows in the walk band.
inline constexpr bn::array<definition, count> definition_table = {
    definition{bn::fixed_point(-200, 20)},
    definition{bn::fixed_point(200, 20)},
};

[[nodiscard]] constexpr int target_id(int index)
{
    return target_id_base + index;
}

[[nodiscard]] constexpr bool is_target_id(int id)
{
    return id >= target_id_base && id < target_id_base + count;
}

[[nodiscard]] constexpr int index_from_target(int id)
{
    return id - target_id_base;
}

[[nodiscard]] constexpr bn::fixed_rect solid_box_at(int index)
{
    return bn::fixed_rect(definition_table[index].center.x(), definition_table[index].center.y(),
                          solid_width, solid_height);
}

[[nodiscard]] constexpr bn::fixed_rect interact_range_at(int index)
{
    return bn::fixed_rect(definition_table[index].center.x(), definition_table[index].center.y(),
                          interact_width, interact_height);
}

[[nodiscard]] bn::span<const definition> definitions();

[[nodiscard]] bn::span<const bn::fixed_rect> solid_boxes();

class entity
{
public:
    explicit entity(int index);

    void set_camera(const bn::camera_ptr& camera);

    void set_broken(bool broken);
    void set_visual_state(visual_state state);
    void set_urgency(int urgency_level);
    [[nodiscard]] bool is_broken() const;
    [[nodiscard]] visual_state state() const;

    void set_type_badge(int graphics_index);
    void clear_type_badge();

    void set_visible(bool visible);

    void update();

    [[nodiscard]] int index() const;
    [[nodiscard]] int ticket_target_id() const;
    [[nodiscard]] bn::fixed_point position() const;
    [[nodiscard]] bn::fixed_rect solid_box() const;
    [[nodiscard]] bn::fixed_rect interact_range() const;
    [[nodiscard]] bool in_interact_range(const bn::fixed_point& point) const;

    [[nodiscard]] bn::sprite_ptr& sprite();

private:
    int _index;
    visual_state _state;
    int _urgency;
    int _flash_frame;
    int _badge_tiles;
    bool _visible;
    bn::sprite_ptr _sprite;
    bn::optional<bn::sprite_ptr> _badge;

    [[nodiscard]] int _flash_period() const;
    void _sync_sprite();
    void _sync_badge();
    [[nodiscard]] bn::fixed_point _badge_position() const;
};

}

#endif
