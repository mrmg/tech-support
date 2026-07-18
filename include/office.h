#ifndef OFFICE_H
#define OFFICE_H

#include "bn_fixed_rect.h"
#include "bn_regular_bg_ptr.h"
#include "bn_size.h"
#include "bn_span.h"

// Main office floor (3/4 depth bands). Map is larger than the GBA screen.
// Multi-room entry: room::id::office (Phase E). Closet stays on this floor.
namespace office
{

constexpr int map_width = 512;
constexpr int map_height = 256;

bn::regular_bg_ptr create_background();
bn::size map_dimensions();

// Wall bands + desk solids + supply-bin solids (right wall has door gap).
[[nodiscard]] bn::span<const bn::fixed_rect> solid_boxes();

}

#endif
