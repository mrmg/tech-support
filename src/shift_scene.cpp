#include "shift_scene.h"

#include "bn_bg_palettes.h"
#include "bn_bpp_mode.h"
#include "bn_camera_ptr.h"
#include "bn_color.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_span.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "camera_follow.h"
#include "campaign.h"
#include "carry.h"
#include "closet.h"
#include "coffee.h"
#include "common_variable_8x16_sprite_font.h"
#include "credits_scene.h"
#include "day_intro_scene.h"
#include "desk.h"
#include "fix_interaction.h"
#include "inventory.h"
#include "music.h"
#include "notepad.h"
#include "player.h"
#include "printer.h"
#include "reputation.h"
#include "room.h"
#include "sacked_scene.h"
#include "server_rack.h"
#include "sfx.h"
#include "shift.h"
#include "shift_results.h"
#include "shop_scene.h"
#include "ticket.h"
#include "world_cues.h"

#include "bn_sprite_items_energy_bar.h"

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
// G-02: top-right danger line when near_sack (anger static for the shift).
constexpr bn::fixed danger_hud_x = 112;
constexpr bn::fixed danger_hud_y = -72;
// J-08: small energy bar under Day HUD (screen-fixed).
constexpr bn::fixed energy_hud_x = -96;
constexpr bn::fixed energy_hud_y = -58;
constexpr int energy_bar_frame_count = 9;
constexpr int energy_blink_half_period = 8;

// Red HUD ink (font index 0 = transparent key).
constexpr bn::color danger_hud_colors[] = {
    bn::color(0, 31, 0),
    bn::color(28, 4, 4),
    bn::color(24, 2, 2),
    bn::color(20, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
};

// I-06 timer heat: amber under 30s; red (blink) under 10s.
constexpr bn::color timer_amber_colors[] = {
    bn::color(0, 31, 0),
    bn::color(31, 22, 4),
    bn::color(28, 18, 2),
    bn::color(24, 14, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
    bn::color(0, 0, 0),
};

constexpr int timer_blink_half_period = 8;

constexpr bn::sprite_palette_item danger_hud_palette_item(danger_hud_colors, bn::bpp_mode::BPP_4);
constexpr bn::sprite_palette_item timer_amber_palette_item(timer_amber_colors, bn::bpp_mode::BPP_4);
constexpr bn::sprite_palette_item timer_red_palette_item(danger_hud_colors, bn::bpp_mode::BPP_4);

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
                      int remaining_seconds, bool blink_on)
{
    timer_sprites.clear();

    // I-06: under 10s, alternate visible red digits with empty (blink).
    if(remaining_seconds < 10 && ! blink_on)
    {
        return;
    }

    if(remaining_seconds < 10)
    {
        text_generator.set_palette_item(timer_red_palette_item);
    }
    else if(remaining_seconds < 30)
    {
        text_generator.set_palette_item(timer_amber_palette_item);
    }
    else
    {
        text_generator.set_palette_item(common::variable_8x16_sprite_font.item().palette_item());
    }

    text_generator.set_center_alignment();
    text_generator.generate(timer_hud_x, timer_hud_y, format_mm_ss(remaining_seconds), timer_sprites);
}

// G-02: red "HR!" when anger already in the danger band entering the shift.
// Uses a dedicated generator so Day/timer keep the default font palette.
void draw_danger_hud(bn::ivector<bn::sprite_ptr>& danger_sprites)
{
    danger_sprites.clear();

    if(! reputation::near_sack())
    {
        return;
    }

    bn::sprite_text_generator danger_text(common::variable_8x16_sprite_font, danger_hud_palette_item);
    danger_text.set_bg_priority(0);
    danger_text.set_right_alignment();
    danger_text.generate(danger_hud_x, danger_hud_y, "HR!", danger_sprites);
}

// J-08: map energy → energy_bar frame (0 empty … 8 full). Blink near exhaustion.
[[nodiscard]] int energy_bar_graphics_index(int energy)
{
    if(energy <= 0)
    {
        return 0;
    }

    int filled = (energy * (energy_bar_frame_count - 1) + player::max_energy - 1) / player::max_energy;

    if(filled < 1)
    {
        filled = 1;
    }

    if(filled > energy_bar_frame_count - 1)
    {
        filled = energy_bar_frame_count - 1;
    }

    return filled;
}

void sync_energy_hud(bn::optional<bn::sprite_ptr>& energy_bar, int energy, bool blink_on)
{
    const bool warn = energy <= player::warn_energy;
    const bool hide_blink = warn && ! blink_on;

    if(hide_blink)
    {
        if(energy_bar)
        {
            energy_bar->set_visible(false);
        }

        return;
    }

    const int graphics_index = energy_bar_graphics_index(energy);

    if(! energy_bar)
    {
        bn::sprite_ptr bar =
            bn::sprite_items::energy_bar.create_sprite(energy_hud_x, energy_hud_y, graphics_index);
        bar.set_bg_priority(0);
        bar.set_z_order(-50);
        energy_bar = bar;
    }
    else
    {
        energy_bar->set_tiles(bn::sprite_items::energy_bar.tiles_item().create_tiles(graphics_index));
        energy_bar->set_visible(true);
    }
}

// A = continue (retry / next day / restart); B = title.
// Results notepad: Day N, OK/X list, %, pass/fail/complete (threshold in shift_results.h).
shift_end_choice show_shift_over_screen(const shift_summary& summary)
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 4, 6));
    music::stop();
    sfx::play_shift_end();

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

// F-02: budget earn after Phase B evaluate (75% gate). Runs before shop / advance.
void apply_shift_budget_earn(const shift_summary& summary)
{
    const shift_results::evaluation eval =
        shift_results::evaluate(summary.fixed_count, summary.still_open_count);

    // Pass grants more than fail; formula in inventory::budget_earn_for_shift.
    inventory::add_budget(inventory::budget_earn_for_shift(eval.completion_percent, eval.passed));
}

// G-01: anger update after Phase B evaluate. Runs before results so the notepad shows the new value.
void apply_shift_reputation(const shift_summary& summary)
{
    const shift_results::evaluation eval =
        shift_results::evaluate(summary.fixed_count, summary.still_open_count);

    reputation::apply_shift_outcome(eval.completion_percent, summary.still_open_count, eval.passed);
}

// C-04 campaign progression after results (and F-03 shop when shown).
// Fail: day unchanged. Pass: advance (or reset on final-day complete).
void apply_shift_campaign_progress(const shift_summary& summary)
{
    const shift_results::evaluation eval =
        shift_results::evaluate(summary.fixed_count, summary.still_open_count);

    if(! eval.passed)
    {
        return;
    }

    if(campaign::current_day() >= campaign::max_days)
    {
        // Final-day pass: campaign complete → Day 1 + starting stock/budget/anger.
        // Shift earn (and any prior shop) is wiped by reset — intentional fresh-run baseline.
        campaign::reset();
        inventory::reset();
        reputation::reset();
        return;
    }

    campaign::advance();
    // F-04: shop orders from Day N become closet stock for Day N+1.
    inventory::deliver_pending();
}

void set_main_floor_entities_visible(bn::vector<desk::entity, desk::count>& desks,
                                     bn::vector<printer::entity, printer::count>& printers,
                                     bn::span<closet::bin> supply_bins, coffee::station& coffee_station,
                                     bool visible)
{
    for(desk::entity& desk_entity : desks)
    {
        desk_entity.set_visible(visible);
    }

    for(printer::entity& printer_entity : printers)
    {
        printer_entity.set_visible(visible);
    }

    for(closet::bin& supply_bin : supply_bins)
    {
        supply_bin.set_visible(visible);
    }

    coffee_station.set_visible(visible);
}

// I-02: map open ticket kind → ticket_badge frame (server-reset still badges the desk).
[[nodiscard]] int type_badge_tiles(ticket::type issue)
{
    switch(issue)
    {
    case ticket::type::needs_toner:
        return desk::badge_toner;

    case ticket::type::needs_psu:
        return desk::badge_psu;

    case ticket::type::needs_server_reset:
        return desk::badge_server;

    case ticket::type::reboot:
    default:
        return desk::badge_reboot;
    }
}

// E-01/E-02: instant room swap — BG + solids; bins/printers/coffee office-only; rack server-only.
void transition_to_room(room::id destination, bn::regular_bg_ptr& floor_bg, bn::camera_ptr& camera,
                        player& walk_player, bn::vector<desk::entity, desk::count>& desks,
                        bn::vector<printer::entity, printer::count>& printers,
                        bn::span<closet::bin> supply_bins, coffee::station& coffee_station,
                        server_rack::entity& rack, room::id& current_room)
{
    current_room = destination;
    floor_bg = room::create_background(current_room);
    floor_bg.set_priority(3);
    floor_bg.set_camera(camera);
    walk_player.set_position(room::enter_spawn(current_room));
    set_main_floor_entities_visible(desks, printers, supply_bins, coffee_station,
                                    current_room == room::id::office);
    rack.set_visible(current_room == room::id::server);
}

shift_summary play_one_shift()
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 4, 6));
    // H-02: ensure bed is running (title already started it; retry/next day restarts).
    music::play_bed();

    room::id current_room = room::id::office;
    bn::regular_bg_ptr floor_bg = room::create_background(current_room);
    floor_bg.set_priority(3);

    player walk_player;
    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);

    bn::vector<desk::entity, desk::count> desks;

    for(int index = 0; index < desk::count; ++index)
    {
        desks.emplace_back(index);
        desks.back().set_camera(camera);
    }

    // J-03: two printers at opposite ends of the office floor.
    bn::vector<printer::entity, printer::count> printers;

    for(int index = 0; index < printer::count; ++index)
    {
        printers.emplace_back(index);
        printers.back().set_camera(camera);
    }

    // J-04: adjacent toner + PSU bins on main office floor only (hidden in server room).
    bn::vector<closet::bin, closet::count> supply_bins;

    for(int index = 0; index < closet::count; ++index)
    {
        supply_bins.emplace_back(index);
        supply_bins.back().set_camera(camera);
    }

    // J-08: coffee station on office floor (hold-A brew refills sprint energy).
    coffee::station coffee_station;
    coffee_station.set_camera(camera);

    // E-02: server rack in server room only (hold-A sets server_reset_done for the shift).
    server_rack::entity rack;
    rack.set_camera(camera);
    rack.set_visible(false);

    // D-02/I-04/J-04: one carried part (bin A collect/swap, matching B return; icon follows).
    carry::slot carried;
    carried.set_camera(camera);

    // J-08: energy starts full every shift including retries (fresh player + explicit refill).
    walk_player.refill_energy();

    // C-03: day index selects spawn + shift length (Day 1 == Phase A baseline).
    const campaign::day_difficulty day = campaign::difficulty_for_day(campaign::current_day());
    const ticket::spawn::params spawn_params = ticket::spawn::from_seconds(
        day.first_spawn_seconds, day.interval_seconds, day.interval_shrink_seconds,
        day.min_interval_seconds);

    // Desks start idle; spawner sets broken when a ticket binds; A-10 hold-to-reboot clears.
    ticket::spawner tickets(spawn_params);
    notepad::overlay tickets_pad;
    world_cues::edge_indicators edge_cues;
    fix_interaction::hold_to_reboot reboot_fix;

    floor_bg.set_camera(camera);
    walk_player.sprite().set_camera(camera);

    bn::sprite_text_generator hud_text(common::variable_8x16_sprite_font);
    hud_text.set_bg_priority(0);
    // Screen-fixed HUD: Day N (left) + mm:ss (center) + G-02 HR! (right when near_sack).
    bn::vector<bn::sprite_ptr, 8> day_sprites;
    bn::vector<bn::sprite_ptr, 8> timer_sprites;
    bn::vector<bn::sprite_ptr, 8> danger_sprites;
    bn::optional<bn::sprite_ptr> energy_bar;
    draw_day_hud(hud_text, day_sprites);
    draw_danger_hud(danger_sprites);
    sync_energy_hud(energy_bar, walk_player.energy(), true);

    int remaining_frames = day.shift_duration_seconds * shift::frames_per_second;
    int shown_seconds = -1;
    bool shown_timer_blink_on = true;
    int shown_energy = -1;
    bool shown_energy_blink_on = true;
    int energy_blink_frames = 0;

    bn::size map_dims = room::map_dimensions(current_room);
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

        // While notepad is open: pause shift timer, spawn/urgency, movement, sprint drain, brew, fix.
        if(tickets_pad.is_open())
        {
            reboot_fix.reset();
            rack.cancel_hold();
            coffee_station.cancel_brew();
        }
        else
        {
            walk_player.update(room::solid_boxes(current_room));

            // Door zone → other room (instant). Bins / desks / coffee office-only; rack server-only.
            if(room::door_zone(current_room).contains(walk_player.position()))
            {
                transition_to_room(room::other(current_room), floor_bg, camera, walk_player, desks,
                                   printers, bn::span<closet::bin>(supply_bins.data(), supply_bins.size()),
                                   coffee_station, rack, current_room);
                map_dims = room::map_dimensions(current_room);
                reboot_fix.reset();
                coffee_station.cancel_brew();
            }

            tickets.update();

            if(current_room == room::id::office)
            {
                bn::span<closet::bin> bins_span(supply_bins.data(), supply_bins.size());
                // I-06/J-04: highlight only the nearest bin in interact range.
                closet::update_range_highlights(walk_player.position(), bins_span);

                for(closet::bin& supply_bin : supply_bins)
                {
                    supply_bin.sync_stock();
                    supply_bin.update();
                }

                const bool at_coffee = coffee_station.in_interact_range(walk_player.position());
                coffee_station.set_in_range(at_coffee);

                // Coffee hold-A wins in its range so brew never fights bin A or desk fix.
                if(at_coffee)
                {
                    reboot_fix.reset();

                    if(coffee_station.update_brew(walk_player.position(), camera))
                    {
                        walk_player.refill_energy();
                        sfx::play_fix_complete();
                    }
                }
                else
                {
                    coffee_station.cancel_brew();
                    // Bin pickup before hold-to-fix so A pressed at a bin fills carry.
                    carried.update_at_bins(walk_player.position(), bins_span);
                    reboot_fix.update(walk_player.position(), tickets, carried, camera,
                                       bn::span<desk::entity>(desks.data(), desks.size()),
                                       bn::span<printer::entity>(printers.data(), printers.size()));
                }
            }
            else
            {
                for(closet::bin& supply_bin : supply_bins)
                {
                    supply_bin.set_in_range(false);
                }

                coffee_station.set_in_range(false);
                coffee_station.cancel_brew();
                reboot_fix.reset();
                // E-02/E-03/J-07: hold-A at rack → green LEDs; clears the one global outage.
                const bool rack_was_done = rack.server_reset_done();
                rack.update(walk_player.position(), camera);

                if(rack.server_reset_done() && ! rack_was_done)
                {
                    tickets.clear_server_reset_tickets();
                    sfx::play_fix_complete();
                }
            }

            // Outage open after an earlier rack clear → re-arm (Day 4 only schedules one).
            if(tickets.has_open_server_reset() && rack.server_reset_done())
            {
                rack.reset_shift();
            }

            --remaining_frames;
        }

        carried.update_follow(walk_player.position());

        // J-07: one global outage paints every computer with the shared server error.
        const bool outage_active = tickets.has_open_server_reset();
        const int outage_urgency = tickets.server_outage_urgency();

        for(int index = 0; index < desk::count; ++index)
        {
            const bool open = tickets.desk_has_open_ticket(index);
            desks[index].set_broken(open || outage_active);
            desks[index].set_urgency(outage_active ? outage_urgency : tickets.urgency_for_desk(index));

            if(outage_active)
            {
                desks[index].set_type_badge(desk::badge_server);
            }
            else if(open)
            {
                desks[index].set_type_badge(type_badge_tiles(tickets.issue_type_for_desk(index)));
            }
            else
            {
                desks[index].clear_type_badge();
            }

            desks[index].update();
        }

        for(int index = 0; index < printer::count; ++index)
        {
            const int target_id = printer::target_id(index);
            const bool open = tickets.desk_has_open_ticket(target_id);
            printers[index].set_broken(open);
            printers[index].set_urgency(tickets.urgency_for_desk(target_id));

            if(open)
            {
                printers[index].set_type_badge(type_badge_tiles(tickets.issue_type_for_desk(target_id)));
            }
            else
            {
                printers[index].clear_type_badge();
            }

            printers[index].update();
        }

        update_camera_follow(camera, walk_player.position(), map_dims);

        // Edge issue icons toward actionable target (desk/printer/rack) or the door if other room.
        edge_cues.update(tickets.open_tickets(), camera, ! tickets_pad.is_open(), current_room);

        const int remaining_seconds =
            (remaining_frames + shift::frames_per_second - 1) / shift::frames_per_second;
        const bool timer_blink_on = ((remaining_frames / timer_blink_half_period) % 2) == 0;
        const bool timer_needs_redraw =
            remaining_seconds != shown_seconds ||
            (remaining_seconds < 10 && timer_blink_on != shown_timer_blink_on);

        if(timer_needs_redraw)
        {
            shown_seconds = remaining_seconds;
            shown_timer_blink_on = timer_blink_on;
            redraw_timer_hud(hud_text, timer_sprites, remaining_seconds, timer_blink_on);
        }

        // J-08: energy bar; blink when low / exhausted (ticks even while notepad pauses shift).
        ++energy_blink_frames;
        const int energy = walk_player.energy();
        const bool energy_blink_on = ((energy_blink_frames / energy_blink_half_period) % 2) == 0;
        const bool energy_needs_redraw =
            energy != shown_energy ||
            (energy <= player::warn_energy && energy_blink_on != shown_energy_blink_on);

        if(energy_needs_redraw)
        {
            shown_energy = energy;
            shown_energy_blink_on = energy_blink_on;
            sync_energy_hud(energy_bar, energy, energy_blink_on);
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
        // J-05: free Day 2 toner / Day 3 PSU before intro (once per campaign; safe retry).
        inventory::deliver_tutorials_for_day(campaign::current_day());

        // C-05: brief Day N card before every shift (title start, retry, next day).
        run_day_intro_scene();

        // Scope ends the office so results are a clean screen.
        const shift_summary summary = play_one_shift();
        // G-01: apply anger before results so "Anger N/100" reflects this shift.
        apply_shift_reputation(summary);
        // Results first; G-03 sack check runs on dismiss (not mid-shift).
        const shift_end_choice choice = show_shift_over_screen(summary);

        // G-03: at sack line after apply → sacked game over (not retry / shop / title-resume).
        if(reputation::at_sack_threshold())
        {
            run_sacked_scene();
            campaign::reset();
            inventory::reset();
            reputation::reset();
            return;
        }

        const shift_results::evaluation eval =
            shift_results::evaluate(summary.fixed_count, summary.still_open_count);

        // Earn first so the shop sees the post-shift budget.
        apply_shift_budget_earn(summary);

        // F-03: pass + continue → shop before next-day intro. Skip fail / title / final pass
        // (final-day reset would wipe purchases; title leaves the between-day flow).
        const bool continue_run = choice == shift_end_choice::retry;
        const bool mid_campaign_pass =
            eval.passed && campaign::current_day() < campaign::max_days;
        const bool campaign_complete =
            eval.passed && campaign::current_day() >= campaign::max_days;

        if(continue_run && mid_campaign_pass)
        {
            run_shop_scene();
        }

        // H-03: roll credits after a final-day pass (before Day 1 reset / title return).
        if(campaign_complete)
        {
            run_credits_scene();
        }

        // Advance/reset after results (+ shop / credits). Results copy still showed Day N.
        apply_shift_campaign_progress(summary);

        switch(choice)
        {
        case shift_end_choice::retry:
            // Fail → same day; pass → next day; final pass → Day 1 after reset.
            break;

        case shift_end_choice::title:
            // Session day kept — title A resumes without campaign::reset().
            return;

        default:
            // Exhaustive for shift_end_choice; catch new variants at compile/runtime.
            return;
        }
    }
}
