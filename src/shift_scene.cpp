#include "shift_scene.h"

#include "bn_bg_palettes.h"
#include "bn_camera_ptr.h"
#include "bn_color.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "camera_follow.h"
#include "campaign.h"
#include "common_variable_8x16_sprite_font.h"
#include "desk.h"
#include "fix_interaction.h"
#include "notepad.h"
#include "office.h"
#include "player.h"
#include "shift.h"
#include "ticket.h"
#include "world_cues.h"

namespace
{

struct shift_summary
{
    int fixed_count;
    int still_open_count;
    // Per-spawn classification for results notepad (desk + issue + outcome).
    bn::vector<ticket::history_entry, ticket::max_history> history;
};

enum class shift_end_choice
{
    retry,
    title,
};

bn::string<8> format_mm_ss(int total_seconds)
{
    if(total_seconds < 0)
    {
        total_seconds = 0;
    }

    const int minutes = total_seconds / 60;
    const int seconds = total_seconds % 60;
    bn::string<8> text;
    text.append(bn::to_string<2>(minutes));
    text.append(':');

    if(seconds < 10)
    {
        text.append('0');
    }

    text.append(bn::to_string<2>(seconds));
    return text;
}

// C-02 Day HUD: top-left "Day N", separate from A-06 mm:ss timer (top-center).
// Day is static for the shift; keep it in its own sprite vector so timer redraws
// do not rebuild / collide with it (sprite budget: ~6 glyphs each).
constexpr bn::fixed day_hud_x = -112;
constexpr bn::fixed day_hud_y = -72;
constexpr bn::fixed timer_hud_x = 0;
constexpr bn::fixed timer_hud_y = -72;

void draw_day_hud(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& day_sprites)
{
    day_sprites.clear();

    bn::string<8> day_text;
    day_text.append("Day ");
    day_text.append(bn::to_string<2>(campaign::current_day()));

    text_generator.set_left_alignment();
    text_generator.generate(day_hud_x, day_hud_y, day_text, day_sprites);
}

void redraw_timer_hud(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& timer_sprites,
                      int remaining_seconds)
{
    timer_sprites.clear();
    text_generator.set_center_alignment();
    text_generator.generate(timer_hud_x, timer_hud_y, format_mm_ss(remaining_seconds), timer_sprites);
}

// A = retry shift; B = return to title (same on pass and fail).
// Results notepad: OK/X list, completion %, pass/fail copy (threshold in shift_results.h).
shift_end_choice show_shift_over_screen(const shift_summary& summary)
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 4, 6));

    const bn::span<const ticket::history_entry> history(summary.history.data(), summary.history.size());
    const notepad::results_overlay results(history, summary.fixed_count, summary.still_open_count);

    while(true)
    {
        if(bn::keypad::a_pressed())
        {
            return shift_end_choice::retry;
        }

        if(bn::keypad::b_pressed())
        {
            return shift_end_choice::title;
        }

        bn::core::update();
    }
}

shift_summary play_one_shift()
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 4, 6));

    bn::regular_bg_ptr office_bg = office::create_background();
    office_bg.set_priority(3);

    player walk_player;
    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);

    bn::vector<desk::entity, desk::count> desks;

    for(int index = 0; index < desk::count; ++index)
    {
        desks.emplace_back(index);
        desks.back().set_camera(camera);
    }

    // Desks start idle; spawner sets broken when a ticket binds; A-10 hold-to-reboot clears.
    ticket::spawner tickets;
    notepad::overlay tickets_pad;
    world_cues::edge_indicators edge_cues;
    fix_interaction::hold_to_reboot reboot_fix;

    office_bg.set_camera(camera);
    walk_player.sprite().set_camera(camera);

    bn::sprite_text_generator hud_text(common::variable_8x16_sprite_font);
    hud_text.set_bg_priority(0);
    // Screen-fixed HUD: Day N (left) + mm:ss (center). Separate vectors = no layout clash.
    bn::vector<bn::sprite_ptr, 8> day_sprites;
    bn::vector<bn::sprite_ptr, 8> timer_sprites;
    draw_day_hud(hud_text, day_sprites);

    int remaining_frames = shift::duration_frames;
    int shown_seconds = -1;

    const bn::size map_dims = office::map_dimensions();
    update_camera_follow(camera, walk_player.position(), map_dims);

    while(remaining_frames > 0)
    {
        if(bn::keypad::select_pressed())
        {
            tickets_pad.toggle();

            if(tickets_pad.is_open())
            {
                tickets_pad.refresh(tickets.open_tickets());
            }
        }

        // While notepad is open: pause shift timer, spawn/urgency clocks, movement, and fix.
        if(tickets_pad.is_open())
        {
            reboot_fix.reset();
        }
        else
        {
            walk_player.update();
            tickets.update();
            reboot_fix.update(walk_player.position(), tickets, camera);
            --remaining_frames;
        }

        for(int index = 0; index < desk::count; ++index)
        {
            desks[index].set_broken(tickets.desk_has_open_ticket(index));
            desks[index].set_urgency(tickets.urgency_for_desk(index));
            desks[index].update();
        }

        update_camera_follow(camera, walk_player.position(), map_dims);

        // Edge issue icons for open tickets whose desks are off camera.
        edge_cues.update(tickets.open_tickets(), camera, ! tickets_pad.is_open());

        const int remaining_seconds =
            (remaining_frames + shift::frames_per_second - 1) / shift::frames_per_second;

        if(remaining_seconds != shown_seconds)
        {
            shown_seconds = remaining_seconds;
            redraw_timer_hud(hud_text, timer_sprites, remaining_seconds);
        }

        bn::core::update();
    }

    // Classify every spawn at the bell (open → failed; cleared → fixed). No mid-shift fail.
    tickets.classify_at_bell();

    shift_summary summary;
    summary.fixed_count = tickets.fixed_count();
    summary.still_open_count = tickets.open_count();

    for(const ticket::history_entry& entry : tickets.history())
    {
        summary.history.push_back(entry);
    }

    return summary;
}

}

void run_shift_scene()
{
    while(true)
    {
        // Scope ends the office so SHIFT OVER is a clean screen.
        const shift_summary summary = play_one_shift();
        const shift_end_choice choice = show_shift_over_screen(summary);

        switch(choice)
        {
        case shift_end_choice::retry:
            break;

        case shift_end_choice::title:
            return;

        default:
            return;
        }
    }
}
