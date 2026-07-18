#include "office.h"

#include "bn_array.h"

#include "bn_regular_bg_items_office_bg.h"
#include "closet.h"
#include "desk.h"

namespace office
{

namespace
{

// Map coords are centered at (0,0). Keep up/down walk lanes between desk rows
// and a wide center corridor between left/right columns.
constexpr bn::array<bn::fixed_rect, 4 + desk::count + 1> solid_list = {
    // Outer walls (top band matches prior soft ceiling).
    bn::fixed_rect(0, -96, map_width, 64),
    bn::fixed_rect(0, 124, map_width, 8),
    bn::fixed_rect(-252, 0, 8, map_height),
    bn::fixed_rect(252, 0, 8, map_height),
    // Desk solids from desk::definition_table.
    desk::solid_box_at(0),
    desk::solid_box_at(1),
    desk::solid_box_at(2),
    desk::solid_box_at(3),
    // Same-map storage cupboard (Phase D).
    closet::solid_box(),
};

}

bn::regular_bg_ptr create_background()
{
    return bn::regular_bg_items::office_bg.create_bg(0, 0);
}

bn::size map_dimensions()
{
    return bn::size(map_width, map_height);
}

bn::span<const bn::fixed_rect> solid_boxes()
{
    return bn::span<const bn::fixed_rect>(solid_list.data(), solid_list.size());
}

}
