#include "room.h"

#include "bn_array.h"

#include "bn_regular_bg_items_office_bg.h"
#include "bn_regular_bg_items_server_bg.h"
#include "office.h"
#include "server_rack.h"

namespace room
{

namespace
{

// Office → server: right-wall door. Server → office: left-wall door.
// Zones sit in the wall gap; spawns sit just inside so re-entry is not instant.
constexpr bn::fixed_rect office_door(236, 40, 28, 40);
constexpr bn::fixed_rect server_door(-236, 40, 28, 40);
constexpr bn::fixed_point office_enter_spawn(210, 40);
constexpr bn::fixed_point server_enter_spawn(-210, 40);

// Server room: outer walls + rack solid. Left wall split around return door (gap y 20..60).
constexpr bn::array<bn::fixed_rect, 6> server_solid_list = {
    bn::fixed_rect(0, -96, map_width, 64),
    bn::fixed_rect(0, 124, map_width, 8),
    bn::fixed_rect(-252, -54, 8, 148),
    bn::fixed_rect(-252, 94, 8, 68),
    bn::fixed_rect(252, 0, 8, map_height),
    server_rack::solid_box(),
};

}

bn::regular_bg_ptr create_background(id room_id)
{
    switch(room_id)
    {
    case id::office:
        return bn::regular_bg_items::office_bg.create_bg(0, 0);

    case id::server:
        return bn::regular_bg_items::server_bg.create_bg(0, 0);

    default:
        return bn::regular_bg_items::office_bg.create_bg(0, 0);
    }
}

bn::size map_dimensions(id)
{
    return bn::size(map_width, map_height);
}

bn::span<const bn::fixed_rect> solid_boxes(id room_id)
{
    switch(room_id)
    {
    case id::office:
        return office::solid_boxes();

    case id::server:
        return bn::span<const bn::fixed_rect>(server_solid_list.data(), server_solid_list.size());

    default:
        return office::solid_boxes();
    }
}

bn::fixed_rect door_zone(id room_id)
{
    switch(room_id)
    {
    case id::office:
        return office_door;

    case id::server:
        return server_door;

    default:
        return office_door;
    }
}

bn::fixed_point enter_spawn(id destination)
{
    switch(destination)
    {
    case id::office:
        return office_enter_spawn;

    case id::server:
        return server_enter_spawn;

    default:
        return office_enter_spawn;
    }
}

}
