#ifndef SERVER_RACK_H
#define SERVER_RACK_H

#include "bn_camera_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"

#include "shift.h"

// Server-room rack: solid + sprite + hold-A reset (Phase E-02 / E-03).
// Completing hold sets server_reset_done; shift scene clears open needs_server_reset tickets.
namespace server_rack
{

// Center-right of the server map — clear of the left-wall return door.
constexpr bn::fixed_point center(80, 16);

constexpr bn::fixed solid_width = 32;
constexpr bn::fixed solid_height = 36;

constexpr bn::fixed interact_width = 48;
constexpr bn::fixed interact_height = 52;

// Hold duration matches desk reboot so the progress bar feels familiar (H-04: 2s).
inline constexpr int hold_duration_seconds = 2;
inline constexpr int hold_duration_frames = hold_duration_seconds * shift::frames_per_second;

[[nodiscard]] constexpr bn::fixed_rect solid_box()
{
    return bn::fixed_rect(center.x(), center.y(), solid_width, solid_height);
}

[[nodiscard]] constexpr bn::fixed_rect interact_range()
{
    return bn::fixed_rect(center.x(), center.y(), interact_width, interact_height);
}

class entity
{
public:
    entity();

    void set_camera(const bn::camera_ptr& camera);
    void set_visible(bool visible);

    // Hold-A in range fills the progress bar; on complete sets server_reset_done
    // and swaps to the green-LED sprite. Release / leave range resets progress.
    // No-op once reset_done (still shows done visual).
    void update(const bn::fixed_point& player_pos, const bn::camera_ptr& camera);

    // Clear in-progress hold (e.g. notepad opened). Keeps server_reset_done.
    void cancel_hold();

    void reset_shift();

    [[nodiscard]] bool server_reset_done() const;
    [[nodiscard]] bn::fixed_point position() const;
    [[nodiscard]] bn::fixed_rect solid() const;
    [[nodiscard]] bn::fixed_rect interact() const;
    [[nodiscard]] bool in_interact_range(const bn::fixed_point& point) const;

    [[nodiscard]] bn::sprite_ptr& sprite();

private:
    bn::sprite_ptr _sprite;
    bool _server_reset_done;
    int _progress_frames;
    bn::optional<bn::sprite_ptr> _bar;

    void _sync_visual();
    void _hide_bar();
    void _show_bar(int progress_frames, const bn::camera_ptr& camera);
};

}

#endif
