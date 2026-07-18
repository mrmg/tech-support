#ifndef WORLD_CUES_H
#define WORLD_CUES_H

#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

#include "desk.h"
#include "room.h"
#include "ticket.h"

// On-screen broken PC is handled by desk visuals; this drives off-screen edge cues.
namespace world_cues
{

class edge_indicators
{
public:
    edge_indicators();

    // Place simple issue icons on the screen edge toward the ticket's actionable target.
    // Same-room: desk (office) or rack (server reset). Other-room: door in the current map.
    // Higher urgency flashes faster / scales up slightly. When `enabled` is false
    // (e.g. notepad open), all indicators are hidden.
    void update(bn::span<const ticket::instance> open_tickets, const bn::camera_ptr& camera,
                bool enabled, room::id current_room);

private:
    bn::vector<bn::optional<bn::sprite_ptr>, desk::count> _icons;
    int _flash_frame;
};

}

#endif
