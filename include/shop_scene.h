#ifndef SHOP_SCENE_H
#define SHOP_SCENE_H

// Between-day IT supply shop (F-03/F-04). Notepad-ish catalog: buy toner/PSU with budget.
// Shown after a mid-campaign pass (before next day intro). Skip on fail / title / final pass.
// Buys queue pending orders (visible as +N); stock delivers on campaign advance (next day).
// Controls: Up/Down select, A buy, B leave.
void run_shop_scene();

#endif
