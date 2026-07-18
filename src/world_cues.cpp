#include "world_cues.h"

#include "bn_display.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"

#include "bn_sprite_items_part_icon.h"
#include "bn_sprite_items_ticket_badge.h"
#include "closet.h"
#include "desk.h"
#include "printer.h"
#include "server_rack.h"

namespace world_cues
{

namespace
{

// Keep the 16×16 icon fully inside the screen.
constexpr bn::fixed edge_pad = 12;

// Target center must be this far outside the view before an edge cue appears.
constexpr bn::fixed on_screen_margin_x = 28;
constexpr bn::fixed on_screen_margin_y = 24;

// Soft flash: low urgency stays solid; higher urgency blinks faster.
constexpr int flash_base_period = 36;
constexpr int flash_min_period = 8;
constexpr int flash_urgency_step = 3;

// Art key: >= 100 → part_icon frame (key - 100); else ticket_badge frame.
// Matches desk/printer badge + bin + carry glyph contract (J-04 / J-06).
constexpr int part_art_base = 100;
constexpr int no_art_key = -1;

[[nodiscard]] int flash_period_for_urgency(int urgency_level)
{
    if(urgency_level <= 1)
    {
        return 0;
    }

    int period = flash_base_period - urgency_level * flash_urgency_step;

    if(period < flash_min_period)
    {
        period = flash_min_period;
    }

    return period;
}

[[nodiscard]] bn::fixed scale_for_urgency(int urgency_level)
{
    // Subtle grow at high pressure (still readable at the edge).
    if(urgency_level >= 7)
    {
        return bn::fixed(1.25);
    }

    if(urgency_level >= 4)
    {
        return bn::fixed(1.1);
    }

    return bn::fixed(1);
}

[[nodiscard]] bn::fixed clamp_fixed(bn::fixed value, bn::fixed min_value, bn::fixed max_value)
{
    if(value < min_value)
    {
        return min_value;
    }

    if(value > max_value)
    {
        return max_value;
    }

    return value;
}

[[nodiscard]] bool target_on_screen(const bn::fixed_point& screen_pos)
{
    const bn::fixed half_w = bn::display::width() / 2;
    const bn::fixed half_h = bn::display::height() / 2;

    return screen_pos.x() > -half_w + on_screen_margin_x &&
           screen_pos.x() < half_w - on_screen_margin_x &&
           screen_pos.y() > -half_h + on_screen_margin_y &&
           screen_pos.y() < half_h - on_screen_margin_y;
}

[[nodiscard]] bn::fixed_point edge_position(const bn::fixed_point& screen_pos)
{
    const bn::fixed half_w = bn::display::width() / 2;
    const bn::fixed half_h = bn::display::height() / 2;
    const bn::fixed min_x = -half_w + edge_pad;
    const bn::fixed max_x = half_w - edge_pad;
    const bn::fixed min_y = -half_h + edge_pad;
    const bn::fixed max_y = half_h - edge_pad;

    return bn::fixed_point(clamp_fixed(screen_pos.x(), min_x, max_x),
                           clamp_fixed(screen_pos.y(), min_y, max_y));
}

[[nodiscard]] bool is_known_target(int target_id)
{
    return (target_id >= 0 && target_id < desk::count) || printer::is_target_id(target_id) ||
           ticket::is_global_server_target(target_id);
}

// Resolve where the edge icon should point for this ticket in the active room.
// Cross-room tickets cue the door on the current map (no pathfinding).
[[nodiscard]] bn::fixed_point cue_world_target(const ticket::instance& entry, room::id current_room)
{
    const bool needs_rack = ticket::requires_server_reset(entry.issue_type);

    if(needs_rack)
    {
        if(current_room == room::id::server)
        {
            return server_rack::center;
        }

        // Office: guide toward the door into the server room.
        return room::door_zone(room::id::office).position();
    }

    if(current_room == room::id::office)
    {
        if(printer::is_target_id(entry.target_id))
        {
            return printer::definition_table[printer::index_from_target(entry.target_id)].center;
        }

        return desk::definition_table[entry.target_id].center;
    }

    // Server room + office desk/printer ticket: guide back through the return door.
    return room::door_zone(room::id::server).position();
}

[[nodiscard]] int cue_art_key(ticket::type issue_type)
{
    switch(issue_type)
    {
    case ticket::type::needs_toner:
        return part_art_base + closet::toner_tiles_index;

    case ticket::type::needs_psu:
        return part_art_base + closet::psu_tiles_index;

    case ticket::type::needs_server_reset:
        return desk::badge_server;

    case ticket::type::reboot:
    default:
        return desk::badge_reboot;
    }
}

[[nodiscard]] bn::sprite_ptr create_cue_sprite(int art_key, const bn::fixed_point& pos)
{
    if(art_key >= part_art_base)
    {
        return bn::sprite_items::part_icon.create_sprite(pos, art_key - part_art_base);
    }

    return bn::sprite_items::ticket_badge.create_sprite(pos, art_key);
}

}

edge_indicators::edge_indicators() :
    _flash_frame(0)
{
    for(int index = 0; index < ticket::max_open; ++index)
    {
        _icons.emplace_back();
        _icon_keys.push_back(no_art_key);
    }
}

void edge_indicators::update(bn::span<const ticket::instance> open_tickets,
                             const bn::camera_ptr& camera, bool enabled, room::id current_room)
{
    bool slot_used[ticket::max_open] = {};

    ++_flash_frame;

    if(enabled)
    {
        const bn::fixed_point camera_pos = camera.position();

        for(const ticket::instance& entry : open_tickets)
        {
            if(! entry.open || ! is_known_target(entry.target_id))
            {
                continue;
            }

            const bn::fixed_point world = cue_world_target(entry, current_room);
            const bn::fixed_point screen(world.x() - camera_pos.x(), world.y() - camera_pos.y());

            if(target_on_screen(screen))
            {
                continue;
            }

            // Dense target ids 0..max_open-1 (desks, printers, global outage).
            const int slot = entry.target_id;

            if(slot < 0 || slot >= ticket::max_open)
            {
                continue;
            }

            slot_used[slot] = true;
            const bn::fixed_point pos = edge_position(screen);
            const bn::fixed scale = scale_for_urgency(entry.urgency);
            const int art_key = cue_art_key(entry.issue_type);

            if(! _icons[slot] || _icon_keys[slot] != art_key)
            {
                bn::sprite_ptr icon = create_cue_sprite(art_key, pos);
                icon.set_bg_priority(0);
                icon.set_z_order(-10);
                icon.set_scale(scale);
                _icons[slot] = icon;
                _icon_keys[slot] = art_key;
            }
            else
            {
                _icons[slot]->set_position(pos);
                _icons[slot]->set_scale(scale);
            }

            const int period = flash_period_for_urgency(entry.urgency);

            if(period <= 0)
            {
                _icons[slot]->set_visible(true);
            }
            else
            {
                const bool flash_on = (_flash_frame / (period / 2)) % 2 == 0;
                _icons[slot]->set_visible(flash_on);
            }
        }
    }

    for(int index = 0; index < ticket::max_open; ++index)
    {
        if(_icons[index] && ! slot_used[index])
        {
            _icons[index]->set_visible(false);
        }
    }
}

}
