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

    inline static constexpr auto epsilon = static_cast<value_type>(8e-1);

    inline static auto acceleration_contribution(
        particle_t const& a,
        particle_t const& b
    ) noexcept -> acceleration_t
    {
        const auto distance = utils::distance(a.position(), b.position());
        const auto d        = utils::l2_norm(distance.value());
#if DEBUG_PRINT_INTERACTION
        std::cout << "Mj: " << mass2.magnitude() << '\n';
        std::cout << "D: " << distance << '\n';
        std::cout << "d3: " << d * d * d << '\n';
#endif
        return acceleration_t{ pm::physical_constants<value_type>::G *
                               b.mass().magnitude() * distance /
                               std::pow(d * d + epsilon * epsilon, value_type{ 1.5 }) };
    }
};

} // namespace pm::interaction
