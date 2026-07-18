#ifndef FIX_INTERACTION_H
#define FIX_INTERACTION_H

#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"

#include "shift.h"
#include "ticket.h"

// Hold-A-in-range reboot: progress fills only while A is held and player stays in range.
namespace fix_interaction
{

namespace reboot
{
    // Tunable hold duration for Phase A reboot fixes.
    inline constexpr int hold_duration_seconds = 2;
    inline constexpr int hold_duration_frames = hold_duration_seconds * shift::frames_per_second;
}

class hold_to_reboot
{
public:
    hold_to_reboot();

    // Clear hold progress and hide the bar (e.g. notepad opened).
    void reset();

    // Advance hold while A is held in a desk interact range with an open ticket.
    // Release A or leave range resets progress to zero. Completing clears the ticket.
    void update(const bn::fixed_point& player_pos, ticket::spawner& tickets,
                const bn::camera_ptr& camera);

private:
    int _progress_frames;
    int _active_desk_id;
    bn::optional<bn::sprite_ptr> _bar;

    void _hide_bar();
    void _show_bar(int desk_id, int progress_frames, const bn::camera_ptr& camera);
};

}

#endif
