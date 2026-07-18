#include "fix_interaction.h"

#include "bn_color.h"
#include "bn_keypad.h"
#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_desk.h"
#include "bn_sprite_items_printer.h"
#include "bn_sprite_items_progress_bar.h"

#include "inventory.h"
#include "sfx.h"

namespace fix_interaction
{

namespace
{

constexpr int bar_frame_count = 9;
constexpr bn::fixed bar_offset_y = -28;

// I-05: red empty track while wrong/missing part; brief bounce + target flash after a fix.
constexpr int success_pop_frames = 14;
constexpr bn::fixed success_pop_scale_peak = 1.35;
constexpr bn::fixed deny_fade_intensity = 0.9;
constexpr bn::color deny_fade_color(31, 4, 4);

[[nodiscard]] bn::fixed_point target_center(int target_id)
{
    if(printer::is_target_id(target_id))
    {
        return printer::definition_table[printer::index_from_target(target_id)].center;
    }

    return desk::definition_table[target_id].center;
}

[[nodiscard]] int target_in_fix_range(const bn::fixed_point& player_pos, const ticket::spawner& tickets)
{
    int best_target = -1;
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

        // Toner faults never bind to desks; skip if a desk somehow has needs_toner.
        if(tickets.issue_type_for_desk(desk_id) == ticket::type::needs_toner)
        {
            continue;
        }

        if(! desk::interact_range_at(desk_id).contains(player_pos))
        {
            continue;
        }

        const bn::fixed_point delta = desk::definition_table[desk_id].center - player_pos;
        const bn::fixed dist_sq = delta.x() * delta.x() + delta.y() * delta.y();

        if(best_target < 0 || dist_sq < best_dist_sq)
        {
            best_target = desk_id;
            best_dist_sq = dist_sq;
        }
    }

    for(int index = 0; index < printer::count; ++index)
    {
        const int id = printer::target_id(index);

        if(! tickets.desk_has_open_ticket(id))
        {
            continue;
        }

        // Printers accept toner only — PSU is computer-desk only (J-06).
        if(tickets.issue_type_for_desk(id) != ticket::type::needs_toner)
        {
            continue;
        }

        if(! printer::interact_range_at(index).contains(player_pos))
        {
            continue;
        }

        const bn::fixed_point delta = printer::definition_table[index].center - player_pos;
        const bn::fixed dist_sq = delta.x() * delta.x() + delta.y() * delta.y();

        if(best_target < 0 || dist_sq < best_dist_sq)
        {
            best_target = id;
            best_dist_sq = dist_sq;
        }
    }

    return best_target;
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

[[nodiscard]] bn::fixed success_pop_scale(int frames_left)
{
    // Peak mid-pop, ease back to 1 at the ends.
    const int half = success_pop_frames / 2;
    const int from_peak = frames_left > half ? frames_left - half : half - frames_left;
    const bn::fixed t = bn::fixed(half - from_peak) / half;

    return 1 + (success_pop_scale_peak - 1) * t;
}

}

hold_to_reboot::hold_to_reboot() :
    _progress_frames(0),
    _active_target_id(-1),
    _success_frames(0),
    _success_target_id(-1)
{
}

void hold_to_reboot::reset()
{
    _progress_frames = 0;
    _active_target_id = -1;
    _success_frames = 0;
    _success_target_id = -1;
    _clear_success_target_flash();
    _restore_bar_look();
    _hide_bar();
}

void hold_to_reboot::update(const bn::fixed_point& player_pos, ticket::spawner& tickets,
                            carry::slot& carried, const bn::camera_ptr& camera,
                            bn::span<desk::entity> desks, bn::span<printer::entity> printers)
{
    // I-05: finish the success bounce even if A was released.
    if(_success_frames > 0)
    {
        _tick_success_pop(camera);
        return;
    }

    if(! bn::keypad::a_held())
    {
        reset();
        return;
    }

    const int target_id = target_in_fix_range(player_pos, tickets);

    if(target_id < 0)
    {
        reset();
        return;
    }

    const ticket::type issue = tickets.issue_type_for_desk(target_id);

    // Needs-part: wrong/missing → no progress, obvious red empty-bar deny.
    if(ticket::requires_part(issue) && carried.held() != ticket::required_part(issue))
    {
        _progress_frames = 0;
        _active_target_id = target_id;
        _show_deny_bar(target_id, camera);
        return;
    }

    if(target_id != _active_target_id)
    {
        _progress_frames = 0;
        _active_target_id = target_id;
    }

    ++_progress_frames;
    _restore_bar_look();
    _show_bar(target_id, _progress_frames, camera);

    if(_progress_frames >= reboot::hold_duration_frames)
    {
        tickets.clear_desk(target_id);

        if(ticket::requires_part(issue))
        {
            // F-01: install permanently spends one unit of matching stock.
            const carry::part installed = carried.held();
            inventory::consume(installed);
            // J-05: teaching delivery no longer re-grants after a successful install.
            inventory::mark_teaching_incident_complete(installed);
            carried.clear();
        }

        sfx::play_fix_complete();

        if(printer::is_target_id(target_id))
        {
            printer::entity& printer_entity = printers[printer::index_from_target(target_id)];
            _begin_success_pop(target_id, camera, printer_entity.sprite(),
                               bn::sprite_items::printer.palette_item());
        }
        else
        {
            _begin_success_pop(target_id, camera, desks[target_id].sprite(),
                               bn::sprite_items::desk.palette_item());
        }
    }
}

void hold_to_reboot::_hide_bar()
{
    if(_bar)
    {
        _bar->set_visible(false);
    }
}

void hold_to_reboot::_restore_bar_look()
{
    if(! _bar)
    {
        return;
    }

    _bar->set_palette(bn::sprite_items::progress_bar.palette_item());
    _bar->set_scale(1);
}

void hold_to_reboot::_clear_success_target_flash()
{
    if(_success_target_sprite && _success_restore_palette)
    {
        _success_target_sprite->set_palette(*_success_restore_palette);
        _success_target_sprite.reset();
        _success_restore_palette.reset();
    }
}

void hold_to_reboot::_show_bar(int target_id, int progress_frames, const bn::camera_ptr& camera)
{
    const bn::fixed_point world = target_center(target_id);
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

void hold_to_reboot::_show_deny_bar(int target_id, const bn::camera_ptr& camera)
{
    _show_bar(target_id, 0, camera);

    // Unique cached palette so the red fade does not tint the rack progress bar.
    if(! _deny_palette)
    {
        bn::sprite_palette_ptr deny_palette =
            bn::sprite_palette_ptr::create_new(bn::sprite_items::progress_bar.palette_item());
        deny_palette.set_fade(deny_fade_color, deny_fade_intensity);
        _deny_palette = deny_palette;
    }

    _bar->set_palette(*_deny_palette);
    _bar->set_scale(1);
}

void hold_to_reboot::_begin_success_pop(int target_id, const bn::camera_ptr& camera,
                                        bn::sprite_ptr& target_sprite,
                                        const bn::sprite_palette_item& restore_palette)
{
    _progress_frames = 0;
    _active_target_id = -1;
    _success_target_id = target_id;
    _success_frames = success_pop_frames;

    // Brief inverted flash (unique palette — shared target palette stays untouched).
    _clear_success_target_flash();
    bn::sprite_palette_ptr flash_palette = bn::sprite_palette_ptr::create_new(restore_palette);
    flash_palette.set_inverted(true);
    target_sprite.set_palette(flash_palette);
    _success_target_sprite = target_sprite;
    _success_restore_palette = restore_palette;

    _restore_bar_look();
    _show_bar(target_id, reboot::hold_duration_frames, camera);
    _bar->set_scale(success_pop_scale(_success_frames));
}

void hold_to_reboot::_tick_success_pop(const bn::camera_ptr& camera)
{
    --_success_frames;

    if(_success_frames <= 0)
    {
        reset();
        return;
    }

    _show_bar(_success_target_id, reboot::hold_duration_frames, camera);
    _bar->set_scale(success_pop_scale(_success_frames));
}

}
