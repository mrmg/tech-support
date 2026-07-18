#include "world_cues.h"

#include "bn_display.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"

#include "bn_sprite_items_issue_icon.h"

namespace world_cues
{

namespace
{

// Keep the 16×16 icon fully inside the screen.
constexpr bn::fixed edge_pad = 12;

// Desk center must be this far outside the view before an edge cue appears.
constexpr bn::fixed on_screen_margin_x = 28;
constexpr bn::fixed on_screen_margin_y = 24;

// Soft flash: low urgency stays solid; higher urgency blinks faster.
constexpr int flash_base_period = 36;
constexpr int flash_min_period = 8;
constexpr int flash_urgency_step = 3;

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

[[nodiscard]] bool desk_on_screen(const bn::fixed_point& screen_pos)
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

}

edge_indicators::edge_indicators() :
    _flash_frame(0)
{
    for(int index = 0; index < desk::count; ++index)
    {
        _icons.emplace_back();
    }
}

void edge_indicators::update(bn::span<const ticket::instance> open_tickets,
                             const bn::camera_ptr& camera, bool enabled)
{
    bool slot_used[desk::count] = {};

    ++_flash_frame;

    if(enabled)
    {
        const bn::fixed_point camera_pos = camera.position();

        for(const ticket::instance& entry : open_tickets)
        {
            if(! entry.open || entry.desk_id < 0 || entry.desk_id >= desk::count)
            {
                continue;
            }

            const bn::fixed_point world = desk::definition_table[entry.desk_id].center;
            const bn::fixed_point screen(world.x() - camera_pos.x(), world.y() - camera_pos.y());

            if(desk_on_screen(screen))
            {
                continue;
            }

            const int slot = entry.desk_id;
            slot_used[slot] = true;
            const bn::fixed_point pos = edge_position(screen);
            const bn::fixed scale = scale_for_urgency(entry.urgency);

            if(! _icons[slot])
            {
                bn::sprite_ptr icon = bn::sprite_items::issue_icon.create_sprite(pos);
                icon.set_bg_priority(0);
                icon.set_z_order(-10);
                icon.set_scale(scale);
                _icons[slot] = icon;
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

    for(int index = 0; index < desk::count; ++index)
    {
        if(_icons[index] && ! slot_used[index])
        {
            _icons[index]->set_visible(false);
        }
    }
}

}
