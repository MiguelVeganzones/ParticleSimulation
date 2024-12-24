#ifndef INCLUDED_ROOT_PLOTTING
#define INCLUDED_ROOT_PLOTTING

namespace root_plotting
{

auto plot(
    int                n,
    const float* const x,
    const float* const y1,
    const float* const y2,
    const float* const y3,
    const float* const y4
) -> void;

auto plot(
    int                n,
    const float* const x,
    const float* const y1,
    const float* const y2,
    const float* const y3
) -> void;

auto plot(int n, const float* const x, const float* const y1, const float* const y2)
    -> void;

auto plot(int n, const float* const x, const float* const y) -> void;
} // namespace root_plotting

#endif // INCLUDED_ROOT_PLOTTING
