#include "fix_interaction.h"

#include "bn_keypad.h"
#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_progress_bar.h"

#include "desk.h"
#include "inventory.h"
#include "sfx.h"

namespace fix_interaction
{

namespace
{

constexpr int bar_frame_count = 9;
constexpr bn::fixed bar_offset_y = -28;

[[nodiscard]] int desk_in_fix_range(const bn::fixed_point& player_pos, const ticket::spawner& tickets)
{
    int best_desk = -1;
    bn::fixed best_dist_sq = 0;

    for(int desk_id = 0; desk_id < desk::count; ++desk_id)
    {
        if(! tickets.desk_has_open_ticket(desk_id))
        {
            continue;
        }

        // Cross-room tickets clear only at the server rack, not via desk hold-A.
        if(ticket::requires_server_reset(tickets.issue_type_for_desk(desk_id)))
        {
            continue;
        }

        if(! desk::interact_range_at(desk_id).contains(player_pos))
        {
            continue;
        }

        const bn::fixed_point delta = desk::definition_table[desk_id].center - player_pos;
        const bn::fixed dist_sq = delta.x() * delta.x() + delta.y() * delta.y();

        if(best_desk < 0 || dist_sq < best_dist_sq)
        {
            best_desk = desk_id;
            best_dist_sq = dist_sq;
        }
    }

    return best_desk;
}

[[nodiscard]] int bar_graphics_index(int progress_frames)
{
    if(progress_frames <= 0)
    {
        return 0;
    }

    // Map 1..hold_duration_frames onto frames 1..8 (frame 0 = empty track).
    int filled = (progress_frames * (bar_frame_count - 1) + reboot::hold_duration_frames - 1) /
                 reboot::hold_duration_frames;

    if(filled < 1)
    {
        filled = 1;
    }

    if(filled > bar_frame_count - 1)
    {
        filled = bar_frame_count - 1;
    }

    return filled;
}

}

hold_to_reboot::hold_to_reboot() :
    _progress_frames(0),
    _active_desk_id(-1)
{
}

void hold_to_reboot::reset()
{
    _progress_frames = 0;
    _active_desk_id = -1;
    _hide_bar();
}

void hold_to_reboot::update(const bn::fixed_point& player_pos, ticket::spawner& tickets,
                            carry::slot& carried, const bn::camera_ptr& camera)
{
    if(! bn::keypad::a_held())
    {
        reset();
        return;
    }

    const int desk_id = desk_in_fix_range(player_pos, tickets);

    if(desk_id < 0)
    {
        reset();
        return;
    }

    const ticket::type issue = tickets.issue_type_for_desk(desk_id);

    // Needs-part: hold only progresses with the matching carried part (wrong/missing = no progress).
    if(ticket::requires_part(issue) && carried.held() != ticket::required_part(issue))
    {
        reset();
        return;
    }

    if(desk_id != _active_desk_id)
    {
        _progress_frames = 0;
        _active_desk_id = desk_id;
    }

    ++_progress_frames;
    _show_bar(desk_id, _progress_frames, camera);

    if(_progress_frames >= reboot::hold_duration_frames)
    {
        tickets.clear_desk(desk_id);

        if(ticket::requires_part(issue))
        {
            // F-01: install permanently spends one unit of matching stock.
            inventory::consume(carried.held());
            carried.clear();
        }

        sfx::play_fix_complete();
        reset();
    }
}

void hold_to_reboot::_hide_bar()
{
    if(_bar)
    {
        _bar->set_visible(false);
    }
}

void hold_to_reboot::_show_bar(int desk_id, int progress_frames, const bn::camera_ptr& camera)
{
    const bn::fixed_point world = desk::definition_table[desk_id].center;
    const bn::fixed_point pos(world.x(), world.y() + bar_offset_y);
    const int graphics_index = bar_graphics_index(progress_frames);

    if(! _bar)
    {
        bn::sprite_ptr bar = bn::sprite_items::progress_bar.create_sprite(pos, graphics_index);
        bar.set_camera(camera);
        bar.set_bg_priority(0);
        bar.set_z_order(-20);
        _bar = bar;
    }
    else
    {
        _bar->set_position(pos);
        _bar->set_camera(camera);
        _bar->set_tiles(bn::sprite_items::progress_bar.tiles_item().create_tiles(graphics_index));
        _bar->set_visible(true);
    }
}

}
