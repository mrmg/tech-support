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

    // C-05: title reflects session day; A continues mid-campaign (no reset).
    bn::string<16> day_line;
    day_line.append("Day ");
    day_line.append(bn::to_string<2>(campaign::current_day()));
    text_generator.generate(0, 8, day_line, text_sprites);
    text_generator.generate(0, 32, "Press A", text_sprites);
    text_generator.generate(0, 48, "SELECT: New Game", text_sprites);
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
        // Explicit new game only — returning to title mid-campaign must not reset.
        if(bn::keypad::select_pressed())
        {
            campaign::reset();
            redraw_title(text_generator, text_sprites);
        }

        bn::core::update();
    }
}
