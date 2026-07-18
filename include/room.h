#ifndef ROOM_H
#define ROOM_H

#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_regular_bg_ptr.h"
#include "bn_size.h"
#include "bn_span.h"

// Multi-room floor plans for one shift (Phase E). Swap BG + solids + entities by id.
namespace room
{

enum class id
{
    office,
    server,
};

constexpr int map_width = 512;
constexpr int map_height = 256;

[[nodiscard]] constexpr id other(id room_id)
{
    switch(room_id)
    {
    case id::office:
        return id::server;

    case id::server:
        return id::office;

    default:
        return id::office;
    }
}

bn::regular_bg_ptr create_background(id room_id);
bn::size map_dimensions(id room_id);

// Collision solids for the active room (office keeps supply bins).
[[nodiscard]] bn::span<const bn::fixed_rect> solid_boxes(id room_id);

// Walk-into trigger on each map (right door on office, left door on server).
[[nodiscard]] bn::fixed_rect door_zone(id room_id);

// Safe spawn just inside the destination door (outside its trigger).
[[nodiscard]] bn::fixed_point enter_spawn(id destination);

}

#endif
