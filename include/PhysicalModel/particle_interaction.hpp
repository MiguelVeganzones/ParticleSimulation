#pragma once

#include "particle_concepts.hpp"
#include <ranges>

namespace pm::interaction
{

template <concepts::Position Position_Type>
auto l2_distance(Position_Type const& p1, Position_Type const& p2) noexcept ->
    typename Position_Type::value_type
{
    typename Position_Type::value_type sqd{};
    for (auto i = decltype(Position_Type::s_dimension){ 0 };
         i != Position_Type::s_dimension;
         ++i)
    {
        const auto d = (p1[i] - p2[i]);
        sqd += d * d;
    }
    return std::sqrt(sqd) * Position_Type::s_units;
}

template <concepts::Particle Particle_Type>
auto gravitational_interaction(Particle_Type const& p1, Particle_Type const& p2) noexcept
    -> pm::magnitudes::energy<typename Particle_Type::value_type>
{
    using value_type    = typename Particle_Type::value_type;
    const auto distance = l2_distance(p1.position(), p2.position());
    return physical_constants<value_type>::G() * p1.mass() * p2.mass() / distance /
           distance;
}

} // namespace pm::interaction
