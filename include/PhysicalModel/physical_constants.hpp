#pragma once

#include <concepts>

namespace pm
{

template <std::floating_point F>
struct physical_constants_
{
    // Only for small ones, improve check
    static_assert(std::is_same_v<F, double>);
    inline static constexpr auto G = static_cast<F>(6.6743e-11);
};

template <std::floating_point F>
struct physical_parameters
{
    inline static F G = physical_constants_<F>::G;

    inline static auto set_gravitational_constant(F big_g) noexcept -> void
    {
        G = big_g;
    }

    inline static auto reset() noexcept
    {
        G = physical_constants_<F>::G;
    }
};

} // namespace pm
