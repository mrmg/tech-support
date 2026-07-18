#ifndef CLOSET_H
#define CLOSET_H

#include "bn_camera_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_sprite_ptr.h"

// Same-map storage cupboard: solid + sprite on the office floor (Phase D).
namespace closet
{

// Center aisle, just below the top wall band — reachable without a room change.
constexpr bn::fixed_point center(0, -40);

constexpr bn::fixed solid_width = 32;
constexpr bn::fixed solid_height = 36;

// Slightly larger than the solid so the player can stand at the doors (D-02).
constexpr bn::fixed interact_width = 48;
constexpr bn::fixed interact_height = 52;

[[nodiscard]] constexpr bn::fixed_rect solid_box()
{
    return bn::fixed_rect(center.x(), center.y(), solid_width, solid_height);
}

[[nodiscard]] constexpr bn::fixed_rect interact_range()
{
    return bn::fixed_rect(center.x(), center.y(), interact_width, interact_height);
}

class entity
{
public:
    entity();

    void set_camera(const bn::camera_ptr& camera);

    [[nodiscard]] bn::fixed_point position() const;
    [[nodiscard]] bn::fixed_rect solid() const;
    [[nodiscard]] bn::fixed_rect interact() const;
    [[nodiscard]] bool in_interact_range(const bn::fixed_point& point) const;

    // I-06: brighter palette while player stands in interact range (carry pickup zone).
    void set_in_range(bool in_range);

    [[nodiscard]] bn::sprite_ptr& sprite();

private:
    bn::sprite_ptr _sprite;
    bool _in_range;
};

}

#endif
