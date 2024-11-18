#pragma once

#include "particle_concepts.hpp"
#include "physical_constants.hpp"
#include "physical_magnitudes.hpp"
#include "utils.hpp"

#ifndef DEBUG_PRINT
#define DEBUG_PRINT (false)
#endif

// TODO
#define OPTIMIZE_AWAY_MASS(x)

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
    const auto distance   = utils::distance(p1.position(), p2.position());
    auto [unit_vector, r] = utils::normalize(distance.value());
    constexpr auto r_s    = 1e-3;
    // TODO: Optimize out p1_mass - Read note
    const auto magnitude = pm::physical_constants<value_type>::G * p1.mass().value() *
                           p2.mass().magnitude() * r /
                           std::pow(r * r + r_s * r_s, value_type{ 1.5 });
#if DEBUG_PRINT
    std::cout << "D_v:\t" << unit_vector << '\n';
    std::cout << "D_m:\t" << distance << '\n';
    std::cout << "G_f:\t" << magnitude << '\n';
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
    // TODO: Optimize out p1_mass - Read note
    p.acceleration() = force / p.mass().magnitude();
#if DEBUG_PRINT
    std::cout << "M:\t" << p.mass() << '\n';
    std::cout << "F:\t" << force << '\n';
    std::cout << "A:\t" << p.acceleration() << '\n';
#endif
}

} // namespace pm::interaction
