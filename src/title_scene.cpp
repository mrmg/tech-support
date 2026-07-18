#include "title_scene.h"

#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "common_variable_8x16_sprite_font.h"

void run_title_scene()
{
    bn::bg_palettes::set_transparent_color(bn::color(4, 8, 16));

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    text_generator.set_center_alignment();

    bn::vector<bn::sprite_ptr, 32> text_sprites;
    text_generator.generate(0, -24, "TECH SUPPORT", text_sprites);
    text_generator.generate(0, 16, "Press A", text_sprites);

    while(! bn::keypad::a_pressed() && ! bn::keypad::start_pressed())
    {
        bn::core::update();
    }
}
