#include "player.h"

#include "bn_keypad.h"

#include "bn_sprite_items_player.h"
#include "collision.h"

player::player() :
    _sprite(bn::sprite_items::player.create_sprite(0, 40))
{
}

void player::update(bn::span<const bn::fixed_rect> solids)
{
    bn::fixed dx = 0;
    bn::fixed dy = 0;

    if(bn::keypad::left_held())
    {
        dx -= move_speed;
    }
    else if(bn::keypad::right_held())
    {
        dx += move_speed;
    }

    if(bn::keypad::up_held())
    {
        dy -= move_speed;
    }
    else if(bn::keypad::down_held())
    {
        dy += move_speed;
    }

    const bn::fixed_point next =
        collision::move_and_slide(_sprite.position(), dx, dy, _hitbox_size, solids);
    _sprite.set_position(next);
}

void player::set_position(const bn::fixed_point& position)
{
    _sprite.set_position(position);
}

bn::fixed_point player::position() const
{
    return _sprite.position();
}

bn::sprite_ptr& player::sprite()
{
    return _sprite;
}
