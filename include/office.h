#ifndef OFFICE_H
#define OFFICE_H

#include "bn_fixed_rect.h"
#include "bn_regular_bg_ptr.h"
#include "bn_size.h"
#include "bn_span.h"

// Placeholder office floor (3/4 depth bands). Map is larger than the GBA screen.
namespace office
{

constexpr int map_width = 512;
constexpr int map_height = 256;

bn::regular_bg_ptr create_background();
bn::size map_dimensions();

// Wall bands + desk solids from the desk table.
[[nodiscard]] bn::span<const bn::fixed_rect> solid_boxes();

}

#endif
