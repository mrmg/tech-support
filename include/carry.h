#ifndef CARRY_H
#define CARRY_H

#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_string_view.h"

#include "closet.h"

// Single-slot carry for Phase D parts (closet pickup; consume on desk install).
namespace carry
{

// Parts available from the closet (stock gated by inventory — Phase F).
enum class part
{
    none,
    toner,
    psu,
};

// Pickup rule (D-02): **replace**, stock-gated (F-01).
// - A in closet range: empty → toner if stock > 0, else PSU if stock > 0; already holding →
//   swap to the other type only when that type has stock. Blocked when target stock is 0.
// - B in closet range: return part (clear slot). Does not change stock (consume is on install).
// Refuse-if-full is not used; A succeeds only when the target part has stock.

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

    // Closet A/B pickup while player is in interact range. No-op when notepad is open
    // (caller should skip). Updates part icon visibility/frame.
    void update_at_closet(const bn::fixed_point& player_pos, const closet::entity& storage);

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
