#pragma once

#include "particle_concepts.hpp"
#include "physical_constants.hpp"
#include "physical_magnitudes.hpp"
#include "utils.hpp"

#ifndef DEBUG_PRINT_INTERACTION
#define DEBUG_PRINT_INTERACTION (false)
#endif

namespace pm::interaction
{

using namespace particle_concepts;

enum class InteractionType
{
    Gravitational,
    Electrostatic
};

template <Particle Particle_Type>
struct gravitational_interaction
{
    inline static constexpr auto s_interaction_type = InteractionType::Gravitational;
    using particle_t                                = Particle_Type;
    using value_type                                = typename particle_t::value_type;
    using acceleration_t                            = typename particle_t::acceleration_t;
    using position_t                                = typename particle_t::position_t;
    using mass_t                                    = typename particle_t::mass_t;

    inline static constexpr auto epsilon = static_cast<value_type>(4.5e-1);

    inline static auto acceleration_contribution(
        particle_t const& a,
        particle_t const& b
    ) noexcept -> acceleration_t
    {
        const auto distance = utils::distance(a.position(), b.position());
        const auto d        = utils::l2_norm(distance.value());
        return acceleration_t{ (pm::physical_parameters<value_type>::G *
                                b.mass().magnitude() / (d * d * d + epsilon)) *
                               distance };
    }
};

template <Particle Particle_Type>
struct electrostatic_interaction
{
    inline static constexpr auto s_interaction_type = InteractionType::Electrostatic;
    using particle_t                                = Particle_Type;
    using value_type                                = typename particle_t::value_type;
    using acceleration_t                            = typename particle_t::acceleration_t;
    using position_t                                = typename particle_t::position_t;
    using charge_t                                  = typename particle_t::charge_t;

    inline static constexpr auto epsilon = static_cast<value_type>(8e-1);

    inline static auto acceleration_contribution(
        particle_t const& a,
        particle_t const& b
    ) noexcept -> acceleration_t
    {

        const auto distance = utils::distance(a.position(), b.position());
        const auto d        = utils::l2_norm(distance.value());
        return acceleration_t{ (pm::physical_constants_<value_type>::K *
                                b.charge().magnitude() * a.charge().magnitude() /
                                a.mass().magnitude() / (d * d * d + epsilon)) /
                               distance };
    }
};

namespace detail
{

template <Particle Particle_Type, InteractionType Interaction>
struct interaction;

template <Particle Particle_Type>
struct interaction<Particle_Type, InteractionType::Gravitational>
{
    using type = gravitational_interaction<Particle_Type>;
};

template <Particle Particle_Type>
struct interaction<Particle_Type, InteractionType::Electrostatic>
{
    using type = electrostatic_interaction<Particle_Type>;
};

template <Particle Particle_Type, InteractionType Interaction>
using interaction_t = typename interaction<Particle_Type, Interaction>::type;

} // namespace detail

template <Particle Particle_Type, InteractionType Interaction>
using particle_interaction_t = detail::interaction_t<Particle_Type, Interaction>;

} // namespace pm::interaction
