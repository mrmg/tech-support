#ifndef CARRY_H
#define CARRY_H

#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_sprite_ptr.h"
#include "bn_string_view.h"

#include "closet.h"

// Single-slot carry for supply-bin parts (pickup at bins; consume on install).
namespace carry
{

// Parts available from the supply bins (stock gated by inventory — Phase F).
enum class part
{
    none,
    toner,
    psu,
};

// Pickup rule (J-04): one item; per-bin A/B (no A-to-cycle cupboard).
// - A at a bin: empty hands → take that bin's part when stock > 0; holding the other
//   part → swap only when this bin has stock; already holding this part → no-op.
//   Empty/blocked A uses Phase I deny flash on the bin.
// - B at a bin: return only when carrying that bin's matching part (stock unchanged).
// Refuse-if-full is not used; A succeeds only when the target bin has stock.

[[nodiscard]] constexpr bn::string_view part_label(part value)
{
    switch(value)
    {
    case part::none:
        return "";

    case part::toner:
        return "TONER";

    case part::psu:
        return "PSU";

    default:
        return "";
    }
}

class slot
{
public:
    slot();

    [[nodiscard]] part held() const;
    [[nodiscard]] bool empty() const;

    void clear();
    void set(part value);

    // Attach icon to the shift camera so it scrolls with the player.
    void set_camera(const bn::camera_ptr& camera);

    // Bin A/B pickup while player is in a bin interact range. No-op when notepad is
    // open (caller should skip). Updates part icon visibility/frame.
    void update_at_bins(const bn::fixed_point& player_pos, bn::span<closet::bin> bins);

    // I-04: held part icon follows player (above head, slight bob). Call each frame.
    void update_follow(const bn::fixed_point& player_pos);

private:
    part _held;
    bn::optional<bn::sprite_ptr> _icon;
    bn::optional<bn::camera_ptr> _camera;
    int _bob_frame;

    void _sync_icon();
};

}

#endif
