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
        _sprite.set_horizontal_flip(true);
    }
    else if(bn::keypad::right_held())
    {
        dx += move_speed;
        _sprite.set_horizontal_flip(false);
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

    _update_animation(dx != 0 || dy != 0);
}

void player::_update_animation(bool moving)
{
    if(moving)
    {
        ++_anim_wait;

        if(_anim_wait >= _walk_anim_wait)
        {
            _anim_wait = 0;
            _walk_frame = (_walk_frame + 1) % _walk_frame_count;
            _sprite.set_tiles(bn::sprite_items::player.tiles_item(), _walk_frame);
        }
    }
    else if(_walk_frame != _idle_frame || _anim_wait != 0)
    {
        _anim_wait = 0;
        _walk_frame = _idle_frame;
        _sprite.set_tiles(bn::sprite_items::player.tiles_item(), _idle_frame);
    }
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
