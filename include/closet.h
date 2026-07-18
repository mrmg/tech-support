#ifndef CLOSET_H
#define CLOSET_H

#include "bn_array.h"
#include "bn_camera_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

// Office-floor supply bins: adjacent toner + PSU (J-04). Replaces the old single
// cycling cupboard. Bin icon uses the same part_icon frames as the carried item.
namespace closet
{

constexpr int count = 2;

constexpr int toner_index = 0;
constexpr int psu_index = 1;

// part_icon.bmp / carried-item frames (must stay aligned with carry.cpp).
constexpr int toner_tiles_index = 0;
constexpr int psu_tiles_index = 1;

constexpr bn::fixed solid_width = 24;
constexpr bn::fixed solid_height = 36;

// Slightly larger than the solid so the player can stand at the bin face.
constexpr bn::fixed interact_width = 36;
constexpr bn::fixed interact_height = 48;

// Aisle pair just below the top wall band — reachable without a room change.
inline constexpr bn::array<bn::fixed_point, count> centers = {
    bn::fixed_point(-22, -40),
    bn::fixed_point(22, -40),
};

[[nodiscard]] constexpr bn::fixed_rect solid_box_at(int index)
{
    return bn::fixed_rect(centers[index].x(), centers[index].y(), solid_width, solid_height);
}

[[nodiscard]] constexpr bn::fixed_rect interact_range_at(int index)
{
    return bn::fixed_rect(centers[index].x(), centers[index].y(), interact_width, interact_height);
}

[[nodiscard]] bn::span<const bn::fixed_rect> solid_boxes();

class bin
{
public:
    explicit bin(int index);

    void set_camera(const bn::camera_ptr& camera);
    void set_visible(bool visible);

    // Brighter body palette while this bin is the active interact target.
    void set_in_range(bool in_range);

    // I-05-style red flash for empty / blocked A or wrong-bin B.
    void flash_deny();

    // Refresh count label + empty icon state from inventory stock.
    void sync_stock();

    void update();

    [[nodiscard]] int index() const;
    [[nodiscard]] bool is_toner() const;
    [[nodiscard]] bool is_psu() const;
    [[nodiscard]] int part_tiles_index() const;
    [[nodiscard]] bn::fixed_point position() const;
    [[nodiscard]] bn::fixed_rect solid() const;
    [[nodiscard]] bn::fixed_rect interact() const;
    [[nodiscard]] bool in_interact_range(const bn::fixed_point& point) const;

    [[nodiscard]] bn::sprite_ptr& sprite();

private:
    int _index;
    bool _visible;
    bool _in_range;
    int _deny_frames;
    int _shown_stock;
    bn::sprite_ptr _sprite;
    bn::optional<bn::sprite_ptr> _part_icon;
    bn::sprite_text_generator _count_text;
    bn::vector<bn::sprite_ptr, 2> _count_sprites;
    bn::optional<bn::camera_ptr> _camera;

    void _apply_body_palette();
    void _sync_part_icon(int stock);
    void _sync_count_label(int stock);
};

// Highlight only the nearest bin in interact range (none if out of all ranges).
void update_range_highlights(const bn::fixed_point& player_pos, bn::span<bin> bins);

}

#endif
