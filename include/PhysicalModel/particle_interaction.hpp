#pragma once

#include "particle_concepts.hpp"
#include "physical_constants.hpp"
#include "physical_magnitudes.hpp"
#include "utils.hpp"
#include <ranges>

namespace pm::interaction
{

template <concepts::Particle Particle_Type>
auto gravitational_interaction(Particle_Type const& p1, Particle_Type const& p2) noexcept
    -> magnitudes::force<Particle_Type::s_dimension, typename Particle_Type::value_type>
{
    using value_type = typename Particle_Type::value_type;
    using force_t    = magnitudes::force<Particle_Type::s_dimension, value_type>;
    [[maybe_unused]]
    const auto distance            = utils::distance(p1.position(), p2.position());
    const auto [unit_vector, norm] = utils::normalize(distance.value());
    const auto magnitude = pm::physical_constants<value_type>::G * p1.mass().value() *
                           p2.mass().value() / norm / norm;
    return force_t{ unit_vector * magnitude };
}

template <concepts::Particle Particle_Type>
auto calculate_force() noexcept -> void
{
}

} // namespace pm::interaction
