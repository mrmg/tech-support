#ifndef COLLISION_H
#define COLLISION_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_fixed_size.h"
#include "bn_span.h"

// Axis AABB helpers for player / furniture solids.
namespace collision
{

[[nodiscard]] bool overlaps_any(const bn::fixed_rect& box, bn::span<const bn::fixed_rect> solids);

// Apply dx then dy separately so the player slides along walls/desks.
[[nodiscard]] bn::fixed_point move_and_slide(const bn::fixed_point& position, bn::fixed dx, bn::fixed dy,
                                             const bn::fixed_size& hitbox_size,
                                             bn::span<const bn::fixed_rect> solids);

}

#endif
