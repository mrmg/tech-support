#ifndef COFFEE_H
#define COFFEE_H

#include "bn_camera_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"

#include "shift.h"

// Office-floor coffee station: hold-A brew refills sprint energy (J-08).
namespace coffee
{

constexpr bn::fixed solid_width = 24;
constexpr bn::fixed solid_height = 28;

// Slightly larger than the solid so the player can stand at the machine face.
constexpr bn::fixed interact_width = 40;
constexpr bn::fixed interact_height = 44;

// Center aisle below the desk rows — away from supply bins and door zone.
inline constexpr bn::fixed_point center(0, 88);

// Short brew (faster than desk reboot hold).
inline constexpr int brew_duration_seconds = 1;
inline constexpr int brew_duration_frames = brew_duration_seconds * shift::frames_per_second;

[[nodiscard]] constexpr bn::fixed_rect solid_box()
{
    return bn::fixed_rect(center.x(), center.y(), solid_width, solid_height);
}

[[nodiscard]] constexpr bn::fixed_rect interact_range()
{
    return bn::fixed_rect(center.x(), center.y(), interact_width, interact_height);
}

[[nodiscard]] bn::span<const bn::fixed_rect> solid_boxes();

class station
{
public:
    station();

    void set_camera(const bn::camera_ptr& camera);
    void set_visible(bool visible);

    // Brighter body while player is in interact range.
    void set_in_range(bool in_range);

    // Clear brew progress and hide the bar (notepad open / room leave).
    void cancel_brew();

    // Hold-A in range fills brew progress; returns true on the frame brew completes.
    // Caller refills player energy on true. No-op when out of range or A released.
    [[nodiscard]] bool update_brew(const bn::fixed_point& player_pos, const bn::camera_ptr& camera);

    [[nodiscard]] bn::fixed_point position() const;
    [[nodiscard]] bn::fixed_rect solid() const;
    [[nodiscard]] bn::fixed_rect interact() const;
    [[nodiscard]] bool in_interact_range(const bn::fixed_point& point) const;
    [[nodiscard]] bool brewing() const;

    [[nodiscard]] bn::sprite_ptr& sprite();

private:
    bool _visible;
    bool _in_range;
    int _brew_frames;
    bn::sprite_ptr _sprite;
    bn::optional<bn::sprite_ptr> _bar;

    void _apply_body_palette();
    void _hide_bar();
    void _show_bar(int progress_frames, const bn::camera_ptr& camera);
};

}

#endif
