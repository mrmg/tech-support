#ifndef SHIFT_RESULTS_H
#define SHIFT_RESULTS_H

// End-of-shift completion % and pass/fail (Phase B / B-03).
namespace shift_results
{

// Named configurable pass threshold. Edit here to change the bar (default 75).
inline constexpr int pass_threshold_percent = 75;

struct evaluation
{
    int completion_percent;
    bool passed;
};

// Locked formula: (fixed / (fixed + failed)) * 100.
// Zero tickets spawned → 100% / pass so empty shifts do not soft-lock.
[[nodiscard]] constexpr evaluation evaluate(int fixed_count, int failed_count)
{
    const int total = fixed_count + failed_count;

    if(total <= 0)
    {
        return evaluation{100, true};
    }

    const int percent = (fixed_count * 100) / total;
    return evaluation{percent, percent >= pass_threshold_percent};
}

}

#endif
