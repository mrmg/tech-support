#include "office.h"

#include "bn_array.h"

#include "bn_regular_bg_items_office_bg.h"
#include "closet.h"
#include "coffee.h"
#include "desk.h"
#include "printer.h"

namespace office
{

namespace
{

// Map coords are centered at (0,0). Keep up/down walk lanes between desk rows
// and a wide center corridor between left/right columns.
// Right wall is split for the Phase E door gap (y 20..60) into the server room.
constexpr bn::array<bn::fixed_rect, 5 + desk::count + printer::count + closet::count + 1> solid_list = {
    // Outer walls (top band matches prior soft ceiling).
    bn::fixed_rect(0, -96, map_width, 64),
    bn::fixed_rect(0, 124, map_width, 8),
    bn::fixed_rect(-252, 0, 8, map_height),
    bn::fixed_rect(252, -54, 8, 148),
    bn::fixed_rect(252, 94, 8, 68),
    // Desk solids from desk::definition_table.
    desk::solid_box_at(0),
    desk::solid_box_at(1),
    desk::solid_box_at(2),
    desk::solid_box_at(3),
    // Printers at opposite ends of the office floor (J-03).
    printer::solid_box_at(0),
    printer::solid_box_at(1),
    // Adjacent toner + PSU supply bins (J-04) — office floor only.
    closet::solid_box_at(0),
    closet::solid_box_at(1),
    // Coffee station (J-08) — center aisle, office floor only.
    coffee::solid_box(),
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
