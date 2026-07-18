#include "notepad.h"

#include "bn_array.h"
#include "bn_bpp_mode.h"
#include "bn_color.h"
#include "bn_sprite_palette_item.h"
#include "bn_string.h"
#include "bn_string_view.h"

#include "bn_regular_bg_items_notepad_bg.h"
#include "common_variable_8x16_sprite_font.h"
#include "desk.h"

namespace notepad
{

namespace
{

// Dark brown ink on cream paper (font index 0 stays transparent green-key style).
constexpr bn::color ink_colors[] = {
    bn::color(0, 31, 0),
    bn::color(7, 5, 3),
    bn::color(10, 7, 5),
    bn::color(14, 9, 6),
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

constexpr bn::sprite_palette_item ink_palette_item(ink_colors, bn::bpp_mode::BPP_4);

constexpr bn::array<bn::string_view, desk::count> desk_labels = {
    "Desk 1",
    "Desk 2",
    "Desk 3",
    "Desk 4",
};

[[nodiscard]] bn::string_view issue_line(ticket::type issue_type)
{
    switch(issue_type)
    {
    case ticket::type::reboot:
        return "reboot";

    default:
        return "?";
    }
}

[[nodiscard]] bn::string_view desk_label(int desk_id)
{
    if(desk_id < 0 || desk_id >= desk::count)
    {
        return "Desk ?";
    }

    return desk_labels[desk_id];
}

// Clear urgency readout: digit plus bangs as pressure climbs (still fixable at max).
[[nodiscard]] bn::string<6> urgency_label(int urgency_level)
{
    int level = urgency_level;

    if(level < 0)
    {
        level = 0;
    }
    else if(level > ticket::urgency::max_level)
    {
        level = ticket::urgency::max_level;
    }

    bn::string<6> text;
    text.append(bn::to_string<2>(level));

    if(level >= 7)
    {
        text.append("!!");
    }
    else if(level >= 4)
    {
        text.append('!');
    }

    return text;
}

}

overlay::overlay() :
    _open(false),
    _text_generator(common::variable_8x16_sprite_font)
{
    _text_generator.set_bg_priority(0);
    _text_generator.set_palette_item(ink_palette_item);
}

void overlay::toggle()
{
    set_open(! _open);
}

void overlay::set_open(bool open)
{
    if(open == _open)
    {
        return;
    }

    _open = open;

    if(_open)
    {
        _show();
    }
    else
    {
        _hide();
    }
}

bool overlay::is_open() const
{
    return _open;
}

void overlay::refresh(bn::span<const ticket::instance> open_tickets)
{
    if(! _open)
    {
        return;
    }

    _draw_list(open_tickets);
}

void overlay::_show()
{
    _panel_bg = bn::regular_bg_items::notepad_bg.create_bg(0, 0);
    _panel_bg->set_priority(0);
}

void overlay::_hide()
{
    _text_sprites.clear();
    _panel_bg.reset();
}

void overlay::_draw_list(bn::span<const ticket::instance> open_tickets)
{
    _text_sprites.clear();

    // Panel sits on the right; text is screen-fixed (no camera).
    constexpr bn::fixed left_x = 20;
    constexpr bn::fixed urgency_x = 108;
    constexpr bn::fixed header_y = -60;
    constexpr bn::fixed first_row_y = -42;
    constexpr bn::fixed row_step = 14;

    _text_generator.set_left_alignment();
    _text_generator.generate(left_x, header_y, "TICKETS", _text_sprites);

    _text_generator.set_right_alignment();
    _text_generator.generate(urgency_x, header_y, "URG", _text_sprites);

    int row = 0;

    for(const ticket::instance& entry : open_tickets)
    {
        if(! entry.open)
        {
            continue;
        }

        const bn::fixed y = first_row_y + row * row_step;
        bn::string<24> left_line;
        left_line.append(desk_label(entry.desk_id));
        left_line.append(' ');
        left_line.append(issue_line(entry.issue_type));

        _text_generator.set_left_alignment();
        _text_generator.generate(left_x, y, left_line, _text_sprites);

        _text_generator.set_right_alignment();
        _text_generator.generate(urgency_x, y, urgency_label(entry.urgency), _text_sprites);

        ++row;
    }

    if(row == 0)
    {
        _text_generator.set_left_alignment();
        _text_generator.generate(left_x, first_row_y, "No open tickets", _text_sprites);
    }
}

namespace
{

// Font has no ✓/✗ glyphs; OK/X are the readable stand-ins for fixed/failed.
[[nodiscard]] bn::string_view outcome_mark(ticket::outcome result)
{
    switch(result)
    {
    case ticket::outcome::fixed:
        return "OK";

    case ticket::outcome::failed:
        return "X";

    case ticket::outcome::pending:
        return "?";

    default:
        return "?";
    }
}

bn::string<20> format_aggregate_header(int fixed_count, int still_open_count)
{
    bn::string<20> text;
    text.append("F:");
    text.append(bn::to_string<3>(fixed_count));
    text.append(" O:");
    text.append(bn::to_string<3>(still_open_count));
    return text;
}

}

results_overlay::results_overlay(bn::span<const ticket::history_entry> history, int fixed_count,
                                 int still_open_count) :
    _text_generator(common::variable_8x16_sprite_font)
{
    _text_generator.set_bg_priority(0);
    _text_generator.set_palette_item(ink_palette_item);

    _panel_bg = bn::regular_bg_items::notepad_bg.create_bg(0, 0);
    _panel_bg->set_priority(0);

    _draw(history, fixed_count, still_open_count);
}

void results_overlay::_draw(bn::span<const ticket::history_entry> history, int fixed_count,
                            int still_open_count)
{
    _text_sprites.clear();

    // Same ruled panel as mid-shift notepad; outcome column replaces urgency.
    constexpr bn::fixed left_x = 20;
    constexpr bn::fixed mark_x = 108;
    constexpr bn::fixed header_y = -60;
    constexpr bn::fixed aggregate_y = -46;
    constexpr bn::fixed first_row_y = -28;
    constexpr bn::fixed row_step = 14;
    constexpr bn::fixed prompt_y = 52;

    _text_generator.set_left_alignment();
    _text_generator.generate(left_x, header_y, "RESULTS", _text_sprites);
    _text_generator.generate(left_x, aggregate_y, format_aggregate_header(fixed_count, still_open_count),
                             _text_sprites);

    const int history_count = history.size();
    const int visible =
        history_count < max_visible_rows ? history_count : max_visible_rows;

    for(int index = 0; index < visible; ++index)
    {
        const ticket::history_entry& entry = history[index];
        const bn::fixed y = first_row_y + index * row_step;

        bn::string<24> left_line;
        left_line.append(desk_label(entry.desk_id));
        left_line.append(' ');
        left_line.append(issue_line(entry.issue_type));

        _text_generator.set_left_alignment();
        _text_generator.generate(left_x, y, left_line, _text_sprites);

        _text_generator.set_right_alignment();
        _text_generator.generate(mark_x, y, outcome_mark(entry.result), _text_sprites);
    }

    const bn::fixed footer_y = first_row_y + visible * row_step;

    if(history_count > max_visible_rows)
    {
        bn::string<16> more;
        more.append('+');
        more.append(bn::to_string<4>(history_count - max_visible_rows));
        more.append(" more");

        _text_generator.set_left_alignment();
        _text_generator.generate(left_x, footer_y, more, _text_sprites);
    }
    else if(history_count == 0)
    {
        _text_generator.set_left_alignment();
        _text_generator.generate(left_x, first_row_y, "No tickets", _text_sprites);
    }

    // Keep A/B on the notepad (same as A-12 inputs; B-03 may refine copy).
    _text_generator.set_left_alignment();
    _text_generator.generate(left_x, prompt_y, "A: Retry", _text_sprites);
    _text_generator.generate(left_x, prompt_y + 14, "B: Title", _text_sprites);
}

}
