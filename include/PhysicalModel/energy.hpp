#pragma once

#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include "physical_constants.hpp"
#include "physical_magnitudes.hpp"
#include "utils.hpp"
#include <algorithm>
#include <ranges>

namespace pm::energy
{

template <particle_concepts::Particle Particle_Type>
[[nodiscard]]
inline static constexpr auto compute_kinetic_energy(Particle_Type const& p) noexcept
    -> magnitudes::energy<typename Particle_Type::value_type>
{
    using particle_t = Particle_Type;
    using value_type = typename particle_t::value_type;
    using energy_t   = magnitudes::energy<value_type>;

    const auto v = utils::l2_norm(p.velocity().value());
    return energy_t{ value_type{ 0.5f } * p.mass().magnitude() * v * v };
}

[[nodiscard]]
inline static constexpr auto compute_kinetic_energy(
    std::ranges::input_range auto&& particles
) noexcept
    -> magnitudes::energy<
        typename std::ranges::range_value_t<decltype(particles)>::value_type>
    requires particle_concepts::Particle<std::ranges::range_value_t<decltype(particles)>>
{
    using particle_t = std::ranges::range_value_t<decltype(particles)>;
    using value_type = typename particle_t::value_type;
    using energy_t   = magnitudes::energy<value_type>;

    return std::ranges::fold_left(
        particles,
        energy_t{},
        [](auto const& acc, auto const& p) {
            return energy_t{ acc + compute_kinetic_energy(p) };
        }
    );
}

[[nodiscard]]
inline static constexpr auto compute_gravitational_potential_energy(
    std::ranges::input_range auto&& particles
) noexcept
    -> magnitudes::energy<
        typename std::ranges::range_value_t<decltype(particles)>::value_type>
    requires particle_concepts::Particle<std::ranges::range_value_t<decltype(particles)>>
{
    using particle_t = std::ranges::range_value_t<decltype(particles)>;
    using value_type = typename particle_t::value_type;
    using energy_t   = magnitudes::energy<value_type>;

    const auto size = std::ranges::size(particles);

    energy_t e{};
    for (std::size_t i = 0; i < size - 1; ++i)
    {
        auto const& p1 = particles[i];
        for (std::size_t j = i + 1; j < size; ++j)
        {
            auto const& p2 = particles[j];
            const auto  r =
                utils::l2_norm(utils::distance(p1.position(), p2.position()).value());
            e = std::move(e) - physical_constants<value_type>::G * p1.mass().magnitude() *
                                   p2.mass().magnitude() / r;
        }
    }
    return e;
}

} // namespace pm::energy
