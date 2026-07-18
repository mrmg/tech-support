#ifndef PLAYER_H
#define PLAYER_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_fixed_size.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"

// Walkable tech-support player with AABB collision against room solids.
class player
{
public:
    // Tunable walk speed in pixels per frame.
    static constexpr bn::fixed move_speed = 1.5;

    player();

    // Solids come from the active room (office floor or server room).
    void update(bn::span<const bn::fixed_rect> solids);
    void set_position(const bn::fixed_point& position);
    [[nodiscard]] bn::fixed_point position() const;
    [[nodiscard]] bn::sprite_ptr& sprite();

private:
    void _update_animation(bool moving);

    bn::sprite_ptr _sprite;
    int _anim_wait = 0;
    int _walk_frame = 0;

    static constexpr bn::fixed_size _hitbox_size{10, 12};
    static constexpr int _idle_frame = 0;
    static constexpr int _walk_frame_count = 4;
    static constexpr int _walk_anim_wait = 5;
};

#endif
