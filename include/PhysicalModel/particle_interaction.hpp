#pragma once

#include "particle_concepts.hpp"
#include "physical_constants.hpp"
#include "physical_magnitudes.hpp"
#include "utils.hpp"

#define DEBUG_PRINT (false)

namespace pm::interaction
{

using namespace particle_concepts;

template <Particle Particle_Type>
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
#if DEBUG_PRINT
    std::cout << "D: " << distance << '\n';
    std::cout << "M: " << magnitude << '\n';
    std::cout << "U: " << unit_vector << '\n';
#endif
    return force_t{ unit_vector * magnitude };
}

template <Particle Particle_Type, std::size_t N>
auto update_acceleration(Particle_Type& p, std::span<Particle_Type, N> particles) noexcept
    -> void
{
    using force_t =
        magnitudes::force<Particle_Type::s_dimension, typename Particle_Type::value_type>;
    force_t force{};
    for (auto const& other : particles)
    {
        if (p.id() != other.id()) [[likely]]
        {
            force += gravitational_interaction(p, other);
        }
    }
    p.acceleration() = force / p.mass();
}

} // namespace pm::interaction
