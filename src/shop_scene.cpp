#include "shop_scene.h"

#include "bn_array.h"
#include "bn_bg_palettes.h"
#include "bn_bpp_mode.h"
#include "bn_color.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_notepad_bg.h"
#include "carry.h"
#include "common_variable_8x16_sprite_font.h"
#include "inventory.h"

namespace
{

// Same dark-brown ink on cream as notepad/results (font index 0 = green key).
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

// Catalog order for D-pad selection (toner then PSU).
constexpr bn::array<carry::part, 2> catalog = {
    carry::part::toner,
    carry::part::psu,
};

void redraw(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& text_sprites,
            int selected)
{
    text_sprites.clear();

    constexpr bn::fixed left_x = 20;
    constexpr bn::fixed header_y = -62;
    constexpr bn::fixed budget_y = -48;
    constexpr bn::fixed first_row_y = -18;
    constexpr bn::fixed row_step = 16;
    constexpr bn::fixed prompt_y = 40;
    constexpr bn::fixed note_y = 54;

    text_generator.set_left_alignment();
    text_generator.generate(left_x, header_y, "IT Supply Order", text_sprites);

    bn::string<20> budget_line;
    budget_line.append("Budget: ");
    budget_line.append(bn::to_string<4>(inventory::budget()));
    text_generator.generate(left_x, budget_y, budget_line, text_sprites);

    for(int index = 0; index < catalog.size(); ++index)
    {
        const carry::part part = catalog[index];
        const bn::fixed y = first_row_y + index * row_step;

        // "  toner  $15  x3 +2" — x = on-hand stock; +N = pending next-day delivery.
        bn::string<32> row;
        row.append(index == selected ? "> " : "  ");
        row.append(carry::part_label(part));
        row.append("  $");
        row.append(bn::to_string<3>(inventory::price_of(part)));
        row.append("  x");
        row.append(bn::to_string<3>(inventory::stock_of(part)));

        const int pending = inventory::pending_of(part);

        if(pending > 0)
        {
            row.append(" +");
            row.append(bn::to_string<3>(pending));
        }

        text_generator.generate(left_x, y, row, text_sprites);
    }

    text_generator.generate(left_x, prompt_y, "A: Buy  B: Leave", text_sprites);
    // F-04: purchases queue pending; stock lands after campaign::advance.
    text_generator.generate(left_x, note_y, "(+N next day)", text_sprites);
}

}

void run_shop_scene()
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 4, 6));

    bn::optional<bn::regular_bg_ptr> panel_bg = bn::regular_bg_items::notepad_bg.create_bg(0, 0);
    panel_bg->set_priority(0);

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    text_generator.set_bg_priority(0);
    text_generator.set_palette_item(ink_palette_item);

    // One glyph ≈ one sprite; header + budget + 2 rows + prompts can exceed 80.
    bn::vector<bn::sprite_ptr, 128> text_sprites;
    int selected = 0;

    redraw(text_generator, text_sprites, selected);

    // Drain results A so it does not instantly buy the first item.
    while(bn::keypad::a_held() || bn::keypad::b_held())
    {
        bn::core::update();
    }

    while(true)
    {
        if(bn::keypad::up_pressed())
        {
            if(selected > 0)
            {
                --selected;
                redraw(text_generator, text_sprites, selected);
            }
        }
        else if(bn::keypad::down_pressed())
        {
            if(selected < catalog.size() - 1)
            {
                ++selected;
                redraw(text_generator, text_sprites, selected);
            }
        }
        else if(bn::keypad::a_pressed())
        {
            if(inventory::try_buy(catalog[selected]))
            {
                redraw(text_generator, text_sprites, selected);
            }
        }
        else if(bn::keypad::b_pressed())
        {
            break;
        }

        bn::core::update();
    }

    // Drain B so day intro does not need an extra release wait for this press.
    while(bn::keypad::b_held() || bn::keypad::a_held())
    {
        bn::core::update();
    }
}
