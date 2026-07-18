#ifndef FIX_INTERACTION_H
#define FIX_INTERACTION_H

#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_ptr.h"

#include "carry.h"
#include "desk.h"
#include "printer.h"
#include "shift.h"
#include "ticket.h"

// Hold-A-in-range fix: progress fills only while A is held and player stays in range.
// Reboot: no part required. Needs-part: correct carried part required; consumed on complete.
// needs_server_reset: not clearable here (server-room rack only).
// Toner installs clear at printers only (J-03). PSU installs clear at computer desks only (J-06).
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

    // Clear hold progress, success pop, and hide the bar (e.g. notepad opened).
    void reset();

    // Advance hold while A is held in a desk/printer interact range with an open ticket.
    // Needs-part: only progresses when carried matches required_part; on complete consumes
    // inventory stock + clears carry, clears the ticket, target idle. Wrong/missing part: no
    // progress + red empty-bar deny. Success: invert flash + bar bounce + SFX.
    // Reboot: hold-A without a part (unchanged). Release A or leave range resets to zero.
    void update(const bn::fixed_point& player_pos, ticket::spawner& tickets, carry::slot& carried,
                const bn::camera_ptr& camera, bn::span<desk::entity> desks,
                bn::span<printer::entity> printers);

private:
    int _progress_frames;
    int _active_target_id;
    int _success_frames;
    int _success_target_id;
    bn::optional<bn::sprite_ptr> _bar;
    bn::optional<bn::sprite_palette_ptr> _deny_palette;
    bn::optional<bn::sprite_ptr> _success_target_sprite;
    bn::optional<bn::sprite_palette_item> _success_restore_palette;

    void _hide_bar();
    void _show_bar(int target_id, int progress_frames, const bn::camera_ptr& camera);
    void _show_deny_bar(int target_id, const bn::camera_ptr& camera);
    void _begin_success_pop(int target_id, const bn::camera_ptr& camera, bn::sprite_ptr& target_sprite,
                            const bn::sprite_palette_item& restore_palette);
    void _tick_success_pop(const bn::camera_ptr& camera);
    void _restore_bar_look();
    void _clear_success_target_flash();
};

}

#endif
