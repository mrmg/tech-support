#ifndef DESK_H
#define DESK_H

#include "bn_array.h"
#include "bn_camera_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"

// Data-driven coworker desks: solids, interact ranges, idle/broken visuals.
namespace desk
{

constexpr int count = 4;

constexpr bn::fixed solid_width = 40;
constexpr bn::fixed solid_height = 28;

// Slightly larger than the solid so the player can stand beside the desk.
constexpr bn::fixed interact_width = 56;
constexpr bn::fixed interact_height = 44;

// I-02: type badge floats just above the desk sprite.
constexpr bn::fixed type_badge_y_offset = 22;

// ticket_badge.bmp frames (match ticket::type order).
constexpr int badge_reboot = 0;
constexpr int badge_toner = 1;
constexpr int badge_psu = 2;
constexpr int badge_server = 3;

enum class visual_state
{
    idle,
    broken,
};

struct definition
{
    bn::fixed_point center;
};

// Source of truth for desk placement (office solids reuse these centers).
inline constexpr bn::array<definition, count> definition_table = {
    definition{bn::fixed_point(-140, -16)},
    definition{bn::fixed_point(140, -16)},
    definition{bn::fixed_point(-140, 56)},
    definition{bn::fixed_point(140, 56)},
};

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

// Four desk collision AABBs (aligned with office solids).
[[nodiscard]] bn::span<const bn::fixed_rect> solid_boxes();

class entity
{
public:
    explicit entity(int index);

    void set_camera(const bn::camera_ptr& camera);

    void set_broken(bool broken);
    void set_visual_state(visual_state state);
    // Soft urgency intensifies broken-PC flash rate (0 = default; no fail/removal).
    void set_urgency(int urgency_level);
    [[nodiscard]] bool is_broken() const;
    [[nodiscard]] visual_state state() const;

    // I-02: show/hide type badge above the desk (graphics_index = badge_* frame).
    void set_type_badge(int graphics_index);
    void clear_type_badge();

    // Desk PC + type badge (office floor hide when in server room).
    void set_visible(bool visible);

    // Advances broken-PC error flash while the desk is broken.
    void update();

    [[nodiscard]] int index() const;
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

// Nearest desk index to point, or -1 if none.
[[nodiscard]] int nearest_index(const bn::fixed_point& point);

}

#endif
