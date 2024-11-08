#pragma once

#include <type_traits>

namespace pm::concepts
{

template <typename P>
concept Particle = requires(P p) {
    { p.mass() } -> std::same_as<typename P::mass_t>;
    { p.position() } -> std::same_as<typename P::position_t>;
};

template <typename P>
concept Position = requires(P p) {
    typename P::value_type;
    std::begin(p);
    std::end(p);
    P::s_dimension;
};

template <typename P>
concept Magnitude = requires(P p) {
    typename P::value_type;
    P::s_dimension;
};

} // namespace pm::concepts
