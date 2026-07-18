#ifndef SHIFT_H
#define SHIFT_H

// Shift timing (not campaign "day" — see Phase C). Tweak duration_seconds for playtests.
namespace shift
{
    // Default length in real-time seconds (H-04: 120). Live shifts read
    // campaign::day_difficulty::shift_duration_seconds; keep this in sync for debug / docs.
    inline constexpr int duration_seconds = 120;

    // GBA / Butano run at 60 frames per second.
    inline constexpr int frames_per_second = 60;

    inline constexpr int duration_frames = duration_seconds * frames_per_second;
}

#endif
