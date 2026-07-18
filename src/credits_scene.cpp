#include "credits_scene.h"

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
#include "bn_vector.h"

#include "bn_regular_bg_items_notepad_bg.h"
#include "common_variable_8x16_sprite_font.h"
#include "sfx.h"

namespace
{

// Same dark-brown ink on cream as notepad / shop.
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

}

void run_credits_scene()
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 4, 6));
    sfx::play_ui_open();

    bn::optional<bn::regular_bg_ptr> panel_bg = bn::regular_bg_items::notepad_bg.create_bg(0, 0);
    panel_bg->set_priority(0);

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    text_generator.set_bg_priority(0);
    text_generator.set_palette_item(ink_palette_item);

    bn::vector<bn::sprite_ptr, 128> text_sprites;

    constexpr bn::fixed left_x = 20;
    constexpr bn::fixed y0 = -58;
    constexpr bn::fixed step = 12;

    text_generator.set_left_alignment();
    text_generator.generate(left_x, y0, "CREDITS", text_sprites);
    text_generator.generate(left_x, y0 + step * 2, "Engine", text_sprites);
    text_generator.generate(left_x, y0 + step * 3, "  Butano", text_sprites);
    text_generator.generate(left_x, y0 + step * 4, "  devkitARM", text_sprites);
    text_generator.generate(left_x, y0 + step * 6, "Assets", text_sprites);
    text_generator.generate(left_x, y0 + step * 7, "  Placeholder GFX/SFX", text_sprites);
    text_generator.generate(left_x, y0 + step * 9, "Thanks", text_sprites);
    text_generator.generate(left_x, y0 + step * 10, "  GValiente / Butano", text_sprites);
    text_generator.generate(left_x, y0 + step * 11, "  gbadev", text_sprites);
    text_generator.generate(left_x, y0 + step * 13, "A/B: Back", text_sprites);

    while(bn::keypad::a_held() || bn::keypad::b_held() || bn::keypad::start_held())
    {
        bn::core::update();
    }

    while(true)
    {
        if(bn::keypad::a_pressed() || bn::keypad::b_pressed() || bn::keypad::start_pressed())
        {
            break;
        }

        bn::core::update();
    }

    while(bn::keypad::a_held() || bn::keypad::b_held() || bn::keypad::start_held())
    {
        bn::core::update();
    }
}
