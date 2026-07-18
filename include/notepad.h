#ifndef NOTEPAD_H
#define NOTEPAD_H

#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "ticket.h"

// Select-toggled ticket notepad overlay (pauses shift while open).
namespace notepad
{

class overlay
{
public:
    overlay();

    void toggle();
    void set_open(bool open);
    [[nodiscard]] bool is_open() const;

    // Rebuild the ruled list from current open tickets.
    void refresh(bn::span<const ticket::instance> open_tickets);

private:
    bool _open;
    bn::optional<bn::regular_bg_ptr> _panel_bg;
    bn::sprite_text_generator _text_generator;
    bn::vector<bn::sprite_ptr, 48> _text_sprites;

    void _show();
    void _hide();
    void _draw_list(bn::span<const ticket::instance> open_tickets);
};

}

#endif
