#ifndef FIX_INTERACTION_H
#define FIX_INTERACTION_H

#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"

#include "carry.h"
#include "shift.h"
#include "ticket.h"

// Hold-A-in-range fix: progress fills only while A is held and player stays in range.
// Reboot: no part required. Needs-part: correct carried part required; consumed on complete.
// needs_server_reset: not clearable here (server-room rack only).
namespace fix_interaction
{

namespace reboot
{
    // Tunable hold duration for reboot and install fixes (shared progress bar).
    // H-04: kept at 2s — long enough to read, short enough with travel + parts.
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
    // Needs-part: only progresses when carried matches required_part; on complete consumes
    // inventory stock + clears carry, clears the ticket, desk idle. Wrong/missing part: no progress.
    // Reboot: hold-A without a part (unchanged). Release A or leave range resets to zero.
    void update(const bn::fixed_point& player_pos, ticket::spawner& tickets, carry::slot& carried,
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
