#pragma once

#include "particle.hpp"
#include "particle_concepts.hpp"
#include <algorithm>
#include <ranges>
#include <utility>

namespace pm::utils
{

// Definitely premature optimization
// std::Sqrt is pretty expensive and solution is nice tbf
template <concepts::Position Position_Type>
[[gnu::const, nodiscard]]
auto distance(Position_Type const& p1, Position_Type const& p2) noexcept
    -> magnitudes::
        distance<Position_Type::s_dimension, typename Position_Type::value_type>
{
    using distance_t = magnitudes::
        distance<Position_Type::s_dimension, typename Position_Type::value_type>;
    return distance_t(p1.value() - p2.value());
}

[[gnu::const, nodiscard]]
auto l2_norm(concepts::Vector auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    return std::sqrt(std::ranges::fold_left(v, 0, [](const auto acc, const auto e) {
        return e * e + acc;
    }));
}

[[gnu::const, nodiscard]]
auto normalize(concepts::Vector auto const& v) noexcept
    -> std::pair<
        std::remove_cvref_t<decltype(v)>,
        typename std::remove_cvref_t<decltype(v)>::value_type>
{
    const auto norm = l2_norm(std::forward<decltype(v)>(v));
    return { v / norm, norm };
}

} // namespace pm::utils
