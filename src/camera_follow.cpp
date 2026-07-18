#include "camera_follow.h"

#include "bn_display.h"
#include "bn_fixed.h"

void update_camera_follow(bn::camera_ptr& camera, const bn::fixed_point& target,
                          const bn::size& map_dimensions)
{
    const int x_limit = (map_dimensions.width() - bn::display::width()) / 2;
    const int y_limit = (map_dimensions.height() - bn::display::height()) / 2;

    bn::fixed cam_x = target.x();
    bn::fixed cam_y = target.y();

    if(cam_x < -x_limit)
    {
        cam_x = -x_limit;
    }
    else if(cam_x > x_limit)
    {
        cam_x = x_limit;
    }

    if(cam_y < -y_limit)
    {
        cam_y = -y_limit;
    }
    else if(cam_y > y_limit)
    {
        cam_y = y_limit;
    }

    camera.set_position(cam_x, cam_y);
}
