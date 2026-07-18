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

}
