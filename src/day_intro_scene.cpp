#include "day_intro_scene.h"

#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "campaign.h"
#include "common_variable_8x16_sprite_font.h"
#include "ticket.h"

void run_day_intro_scene()
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 8, 16));

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    bn::vector<bn::sprite_ptr, 32> text_sprites;

    text_generator.set_center_alignment();

    bn::string<16> day_line;
    day_line.append("Day ");
    day_line.append(bn::to_string<2>(campaign::current_day()));
    text_generator.generate(0, -8, day_line, text_sprites);
    text_generator.generate(0, 16, "Press A", text_sprites);

    // Wait for release so title/results A does not instantly dismiss the intro.
    while(bn::keypad::a_held() || bn::keypad::start_held())
    {
        ticket::spawn::tick_rng();
        bn::core::update();
    }

    while(! bn::keypad::a_pressed() && ! bn::keypad::start_pressed())
    {
        ticket::spawn::tick_rng();
        bn::core::update();
    }

    // Drain held confirm so shift hold-to-reboot does not start on the same press.
    while(bn::keypad::a_held() || bn::keypad::start_held())
    {
        ticket::spawn::tick_rng();
        bn::core::update();
    }
}
