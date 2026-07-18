#include "title_scene.h"

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

namespace
{

void redraw_title(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& text_sprites)
{
    text_sprites.clear();
    text_generator.set_center_alignment();
    text_generator.generate(0, -24, "TECH SUPPORT", text_sprites);
    text_generator.generate(0, 16, "Press A", text_sprites);

    // C-03 playtest: show day + L/R hint (C-05 may replace with a proper intro).
    bn::string<16> day_line;
    day_line.append("Day ");
    day_line.append(bn::to_string<2>(campaign::current_day()));
    text_generator.generate(0, 32, day_line, text_sprites);
    text_generator.generate(0, 48, "L/R day", text_sprites);
}

}

void run_title_scene()
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 8, 16));

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    bn::vector<bn::sprite_ptr, 48> text_sprites;
    redraw_title(text_generator, text_sprites);

    while(! bn::keypad::a_pressed() && ! bn::keypad::start_pressed())
    {
        // Debug jump: L/R selects day so denser later-day spawns are playtestable before C-04.
        if(bn::keypad::l_pressed() && campaign::current_day() > 1)
        {
            campaign::set_day(campaign::current_day() - 1);
            redraw_title(text_generator, text_sprites);
        }
        else if(bn::keypad::r_pressed() && campaign::current_day() < campaign::max_days)
        {
            campaign::set_day(campaign::current_day() + 1);
            redraw_title(text_generator, text_sprites);
        }

        bn::core::update();
    }
}
