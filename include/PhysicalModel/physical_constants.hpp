#pragma once

#include <concepts>

namespace pm
{

template <std::floating_point F>
struct physical_constants
{
    static_assert(std::is_same_v<F, double>);
    inline static constexpr auto G = static_cast<F>(6.6743e-11);
};

} // namespace pm
