#ifndef CAMERA_FOLLOW_H
#define CAMERA_FOLLOW_H

#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_size.h"

// Keep the camera on the target, clamped so the screen never shows past the map.
void update_camera_follow(bn::camera_ptr& camera, const bn::fixed_point& target,
                          const bn::size& map_dimensions);

#endif
