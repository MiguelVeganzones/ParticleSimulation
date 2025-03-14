#pragma once

#include <concepts>
#include <iterator>

namespace pm::particle_concepts
{

template <typename P>
concept Particle = requires(P p) {
    { p.mass() } -> std::convertible_to<typename P::mass_t>;
    { p.position() } -> std::convertible_to<typename P::position_t>;
};

template <typename P>
concept Position = requires(P p) {
    typename P::value_type;
    std::begin(p);
    std::end(p);
    P::s_dimension;
    p[0];
};

template <typename M>
concept Magnitude = requires(M m) {
    M::_disambiguator_physical_magnitude_;
    M::s_dimension;
#ifdef USE_UNIT_SYSTEM
    M::s_units;
#endif
    typename M::value_type;
    std::begin(m);
    std::end(m);
};

template <typename V>
concept Vector = requires(V v) {
    V::_disambiguator_physical_vector_;
    V::s_dimension;
    typename V::value_type;
};


template <typename I>
concept Interaction = requires {
    typename I::particle_t;
    typename I::acceleration_t;
    {
        I::acceleration_contribution(
            std::declval<typename I::particle_t>(), std::declval<typename I::particle_t>()
        )
    } -> std::same_as<typename I::acceleration_t>;
};

} // namespace pm::particle_concepts
