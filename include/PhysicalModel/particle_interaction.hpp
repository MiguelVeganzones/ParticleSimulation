#pragma once

#include "particle_concepts.hpp"
#include "physical_constants.hpp"
#include "physical_magnitudes.hpp"
#include "utils.hpp"
#include <algorithm>
#include <ranges>

#ifndef DEBUG_PRINT_INTERACTION
#define DEBUG_PRINT_INTERACTION (false)
#endif

namespace pm::interaction
{

using namespace particle_concepts;

template <Particle Particle_Type>
struct gravitational_interaction_calculator
{
    using particle_t     = Particle_Type;
    using value_type     = typename particle_t::value_type;
    using acceleration_t = typename particle_t::acceleration_t;
    using position_t     = typename particle_t::position_t;
    using mass_t         = typename particle_t::mass_t;

    auto acceleration_contribution(
        position_t const& pos1,
        position_t const& pos2,
        mass_t const&     mass2
    ) noexcept -> acceleration_t
    {
        const auto     distance = utils::distance(pos1, pos2);
        const auto     d        = utils::l2_norm(distance.value());
        constexpr auto rs       = value_type{ 8e-2 };
#if DEBUG_PRINT_INTERACTION
        std::cout << "Mj: " << mass2.magnitude() << '\n';
        std::cout << "D: " << distance << '\n';
        std::cout << "d3: " << d * d * d << '\n';
#endif
        return acceleration_t{ pm::physical_constants<value_type>::G * mass2.magnitude() *
                               distance / std::pow(d * d + rs * rs, value_type{ 1.5 }) };
    }

    auto get_acceleration(
        std::size_t           idx,
        std::span<mass_t>     mass,
        std::span<position_t> pos
    ) noexcept
        -> magnitudes::linear_acceleration<
            Particle_Type::s_dimension,
            typename Particle_Type::value_type>

    {
        assert(std::ranges::size(mass) == std::ranges::size(pos));
        acceleration_t acc{};
        for (std::size_t i = 0; i != std::ranges::size(mass); ++i)
        {
            if (i != idx) [[likely]]
            {
                acc =
                    std::move(acc) + acceleration_contribution(pos[idx], pos[i], mass[i]);
            }
        }
#if DEBUG_PRINT_INTERACTION
        std::cout << "Acc " << idx << ": " << acc << '\n';
#endif
        return acc;
    }
};

} // namespace pm::interaction
