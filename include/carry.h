#ifndef CARRY_H
#define CARRY_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_string_view.h"

#include "closet.h"

// Single-slot carry for Phase D parts (closet pickup; consume on desk install).
namespace carry
{

// Parts available from the infinite-stock closet (Phase D).
enum class part
{
    none,
    toner,
    psu,
};

// Pickup rule (D-02): **replace**.
// - A in closet range: empty → toner; already holding → swap to the other type.
// - B in closet range: return part (clear slot). Infinite stock — no inventory cost.
// Refuse-if-full is not used; A always succeeds at the closet.

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

    // Closet A/B pickup while player is in interact range. No-op when notepad is open
    // (caller should skip). Updates HUD icon visibility/frame.
    void update_at_closet(const bn::fixed_point& player_pos, const closet::entity& storage);

    // Keep HUD icon on screen (screen-space, no camera).
    void update_hud();

private:
    part _held;
    bn::optional<bn::sprite_ptr> _hud_icon;

    void _sync_hud();
};

}

#endif
