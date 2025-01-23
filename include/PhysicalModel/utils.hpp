#pragma once

#include "particle_concepts.hpp"
#include "physical_magnitudes.hpp"
#include <algorithm>
#include <cmath>
#include <execution>
#include <numeric>
#include <ranges>
#include <utility>

namespace pm::utils
{

template <particle_concepts::Position Position_Type>
[[gnu::pure, nodiscard]]
auto distance(Position_Type const& p1, Position_Type const& p2) noexcept -> magnitudes::
    distance<Position_Type::s_dimension, typename Position_Type::value_type>
{
    using distance_t = magnitudes::
        distance<Position_Type::s_dimension, typename Position_Type::value_type>;
    return distance_t(p2.value() - p1.value());
}

[[gnu::pure, nodiscard]]
auto l2_norm_sq(particle_concepts::Vector auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    using value_type = typename std::remove_cvref_t<decltype(v)>::value_type;
    constexpr auto N = std::remove_cvref_t<decltype(v)>::s_dimension;
    value_type     ret{};
    for (auto i = decltype(N){}; i != N; ++i)
    {
        ret += v[i] * v[i];
    }
    return ret;
}

[[gnu::pure, nodiscard]]
auto l2_norm(particle_concepts::Vector auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    return std::sqrt(l2_norm_sq(v));
}

[[gnu::pure, nodiscard]]
auto linfinity_norm(particle_concepts::Vector auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    return std::ranges::max(v);
}

[[gnu::pure, nodiscard]]
auto normalize(particle_concepts::Vector auto const& v) noexcept -> std::pair<
    std::remove_cvref_t<decltype(v)>,
    typename std::remove_cvref_t<decltype(v)>::value_type>
{
    const auto norm = l2_norm(std::forward<decltype(v)>(v));
    return { v / norm, norm };
}

} // namespace pm::utils
