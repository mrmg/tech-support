#include "sacked_scene.h"

#include "bn_bg_palettes.h"
#include "bn_bpp_mode.h"
#include "bn_color.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "common_variable_8x16_sprite_font.h"
#include "sfx.h"

namespace
{

// Red ink — distinct from title/day-intro cream-on-blue and results brown notepad.
constexpr bn::color sacked_ink_colors[] = {
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

constexpr bn::sprite_palette_item sacked_ink_palette_item(sacked_ink_colors, bn::bpp_mode::BPP_4);

}

void run_sacked_scene()
{
    // Darker than title (4,8,16) — reads as run-over, not day-fail retry.
    bn::bg_palettes::set_transparent_color(bn::color(8, 2, 2));
    sfx::play_sacked();

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font, sacked_ink_palette_item);
    bn::vector<bn::sprite_ptr, 32> text_sprites;

    text_generator.set_center_alignment();
    text_generator.generate(0, -24, "SACKED", text_sprites);
    text_generator.generate(0, 0, "HR called it", text_sprites);
    text_generator.generate(0, 32, "A/B: Title", text_sprites);

    // Drain results A/B so the same press does not instantly leave.
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
