#ifndef SHIFT_H
#define SHIFT_H

// Shift timing (not campaign "day" — see Phase C). Tweak duration_seconds for playtests.
namespace shift
{
    // Default length of one shift in real-time seconds. Set back to 120 after debug tweaks.
    inline constexpr int duration_seconds = 120;

    // GBA / Butano run at 60 frames per second.
    inline constexpr int frames_per_second = 60;

    inline constexpr int duration_frames = duration_seconds * frames_per_second;
}

#endif
