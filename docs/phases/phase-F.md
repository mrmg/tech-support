# Phase F — Stock & budget

**Unlock:** Phase D complete (parts exist). Prefer after E; may follow D.  
**Uses Phase C between-day moments.**

## Goal

Between days: spend IT budget to order parts (toner, PSU, …). Stock depletes when used; zero stock blocks matching needs-part fixes until the order arrives next day.

## In scope

- Budget currency from shift performance (e.g. based on completion %)
- Stock counts for at least two part types
- Between-day shop UI (after pass, before next day intro — or on fail skip shop)
- Orders arrive next day (clear rule)

## Out of scope

- Deep accounting sim
- Phase G sacking

## Acceptance criteria

- [ ] Player can spend budget on at least two item types
- [ ] Zero stock blocks the matching fix until ordered
- [ ] Orders arrive on a clear rule (next day)

## Tickets

### - [ ] F-01 — Budget + stock model

- **Done when:** Session tracks budget and stock for ≥2 part types; using a part in D consumes stock; closet pickup fails or is blocked at zero stock.
- **Notes:** Named starting stock/budget constants.
- **mGBA:** Exhaust stock → cannot pick up that part.

### - [ ] F-02 — Earn budget from shift results

- **Done when:** Passing/failing a day adjusts budget (pass grants more; formula documented). 
- **Notes:** Simple integer formula.
- **mGBA:** After a pass, budget increased (debug or shop shows it).

### - [ ] F-03 — Between-day shop UI

- **Done when:** After a **pass** (before next day intro), shop screen lets player buy ≥2 item types with budget; A/B or D-pad to purchase/leave.
- **Notes:** Skip shop on fail (retry same day). Notepad-ish styling OK.
- **mGBA:** Pass → shop → buy toner → leave → Day N+1.

### - [ ] F-04 — Orders arrive next day

- **Done when:** Purchases increase stock available for the **next** day (or immediately if “instant stock” is clearer — prefer **next day** per design). Pending orders visible in shop/HUD.
- **Notes:** Document rule in code comments + phase notes.
- **mGBA:** Buy when stock 0 → next day can pick up part.

### - [ ] F-05 — Phase F pass (integration)

- **Done when:** Acceptance criteria met; F-01…F-04 `- [x]`; README updated; jank for G listed.
- **mGBA:** Full economy loop across two days.
