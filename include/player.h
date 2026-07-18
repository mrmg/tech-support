#ifndef PLAYER_H
#define PLAYER_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_fixed_size.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"

#include "shift.h"

// Walkable tech-support player with AABB collision against room solids.
// J-08: R sprints (drains energy); zero energy halves walk speed until coffee.
class player
{
public:
    // Tunable walk speed in pixels per frame (Phase I baseline).
    static constexpr bn::fixed move_speed = 1.5;

    // Sprint while R held with energy remaining — faster than Phase I walk.
    static constexpr bn::fixed sprint_speed = 2.25;

    // Movement when energy is empty (until coffee refill).
    static constexpr bn::fixed exhausted_speed = move_speed / 2;

    // ~3s of continuous sprint at 60fps; ordinary walking does not drain.
    static constexpr int max_energy = 3 * shift::frames_per_second;

    // HUD / warning band: prominent cues only near exhaustion.
    static constexpr int warn_energy = max_energy / 4;

    player();

    // Solids come from the active room (office floor or server room).
    // Caller must skip this while the notepad is open (pauses fatigue).
    void update(bn::span<const bn::fixed_rect> solids);

    void set_position(const bn::fixed_point& position);
    [[nodiscard]] bn::fixed_point position() const;
    [[nodiscard]] bn::sprite_ptr& sprite();

    // Full energy at the start of every shift / retry.
    void refill_energy();
    [[nodiscard]] int energy() const;
    [[nodiscard]] bool is_exhausted() const;
    [[nodiscard]] bool is_low_energy() const;

private:
    void _update_animation(bool moving);
    [[nodiscard]] bn::fixed _current_speed(bool wants_sprint) const;

    bn::sprite_ptr _sprite;
    int _anim_wait = 0;
    int _walk_frame = 0;
    int _energy = max_energy;

    static constexpr bn::fixed_size _hitbox_size{10, 12};
    static constexpr int _idle_frame = 0;
    static constexpr int _walk_frame_count = 4;
    static constexpr int _walk_anim_wait = 5;
};

#endif
