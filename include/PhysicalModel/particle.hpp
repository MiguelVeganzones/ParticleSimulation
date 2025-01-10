#pragma once

#include "concepts.hpp"
#include "particle_concepts.hpp"
#include "physical_magnitudes.hpp"
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <tuple>

#ifndef DEBUG_PRINT
#define DEBUG_PRINT (false)
#endif

namespace pm::particle
{

enum struct ParticleType
{
    real,
    fictitious,
};

template <std::size_t N, std::floating_point F>
class ndparticle
{
public:
    using value_type                         = F;
    using size_type                          = decltype(N);
    using id_t                               = std::int64_t;
    inline static constexpr auto s_dimension = N;
    using position_t        = magnitudes::position<s_dimension, value_type>;
    using mass_t            = magnitudes::mass<value_type>;
    using charge_t          = magnitudes::charge<value_type>;
    using velocity_t        = magnitudes::linear_velocity<s_dimension, value_type>;
    using acceleration_t    = magnitudes::linear_acceleration<s_dimension, value_type>;
    using runtime_1d_unit_t = magnitudes::runtime_unit<1, value_type>;
    using runtime_nd_unit_t = magnitudes::runtime_unit<s_dimension, value_type>;
    inline static id_t           ID                     = 0;
    inline static constexpr id_t fictitious_particle_id = -1;

public:
    constexpr ndparticle(
        mass_t       m,
        position_t   pos,
        velocity_t   vel,
        ParticleType type   = ParticleType::real,
        charge_t     charge = charge_t{}
    ) :
        m_id{ type == ParticleType::real ? ID++ : fictitious_particle_id },
        m_mass{ std::move(m) },
        m_position{ std::move(pos) },
        m_velocity{ std::move(vel) },
        m_charge{ std::move(charge) }
    {
    }

    constexpr ndparticle() :
        m_id{ fictitious_particle_id },
        m_mass{},
        m_position{},
        m_velocity{},
        m_charge{}
    {
    }

public:
    constexpr auto id() const noexcept -> id_t
    {
        return m_id;
    }


#if __GNUC__ >= 14
    [[nodiscard]]
    constexpr auto position(this auto&& self) noexcept -> auto&&
    {
        return std::forward<decltype(self)>(self).m_position;
    }

    [[nodiscard]]
    constexpr auto mass(this auto&& self) noexcept -> auto&&
    {
        return std::forward<decltype(self)>(self).m_mass;
    }

    [[nodiscard]]
    constexpr auto velocity(this auto&& self) noexcept -> auto&&
    {
        return std::forward<decltype(self)>(self).m_velocity;
    }

    [[nodiscard]]
    constexpr auto charge(this auto&& self) noexcept -> auto&&
    {
        return std::forward<decltype(self)>(self).m_charge;
    }

    [[nodiscard]]
    constexpr auto properties(this auto&& self) noexcept -> decltype(auto)
    {
        return std::tie(
            std::forward<decltype(self)>(self).m_mass,
            std::forward<decltype(self)>(self).m_velocity
        );
    }
#else
    [[nodiscard]]
    constexpr auto position() noexcept -> position_t&
    {
        return m_position;
    }

    [[nodiscard]]
    constexpr auto mass() noexcept -> mass_t&
    {
        return m_mass;
    }

    [[nodiscard]]
    constexpr auto velocity() noexcept -> velocity_t&
    {
        return m_velocity;
    }

    [[nodiscard]]
    constexpr auto charge() noexcept -> charge_t&
    {
        return m_charge;
    }

    [[nodiscard]]
    constexpr auto properties() noexcept -> decltype(auto)
    {
        return std::tie(m_mass, m_velocity, m_charge);
    }

    [[nodiscard]]
    constexpr auto position() const noexcept -> position_t const&
    {
        return m_position;
    }

    [[nodiscard]]
    constexpr auto mass() const noexcept -> mass_t const&
    {
        return m_mass;
    }

    [[nodiscard]]
    constexpr auto velocity() const noexcept -> velocity_t const&
    {
        return m_velocity;
    }

    [[nodiscard]]
    constexpr auto charge() const noexcept -> charge_t const&
    {
        return m_charge;
    }

    [[nodiscard]]
    constexpr auto properties() const noexcept -> decltype(auto)
    {
        return std::tie(m_mass, m_velocity, m_charge);
    }

#endif

    [[nodiscard]]
    auto repr() const noexcept -> std::string
    {
        std::ostringstream ss;
        ss << "ID: " << id() << ", mass: " << mass() << ", pos " << position()
           << ", vel: " << velocity();
        return ss.str();
    }

    constexpr auto operator==(ndparticle const&) const -> bool = default;
    constexpr auto operator!=(ndparticle const&) const -> bool = default;

private:
    id_t       m_id;
    mass_t     m_mass;
    position_t m_position;
    velocity_t m_velocity;
    charge_t   m_charge;
};

[[nodiscard]]
auto merge(std::ranges::input_range auto&& particles) noexcept
    -> std::optional<std::ranges::range_value_t<decltype(particles)>>
    requires particle_concepts::Particle<std::ranges::range_value_t<decltype(particles)>>
{
    using particle_t        = std::ranges::range_value_t<decltype(particles)>;
    using mass_t            = typename particle_t::mass_t;
    using position_t        = typename particle_t::position_t;
    using velocity_t        = typename particle_t::velocity_t;
    using runtime_1d_unit_t = typename particle_t::runtime_1d_unit_t;
    using runtime_nd_unit_t = typename particle_t::runtime_nd_unit_t;

    const auto size =
        std::distance(std::ranges::begin(particles), std::ranges::end(particles));
    if (size == 0)
    {
        return std::nullopt;
    }
    else if (size == 1)
    {
        return *particles.begin();
    }
    const auto total_mass = mass_t(std::ranges::fold_left(
        particles,
        runtime_1d_unit_t{},
        [](auto const& acc, auto const& p) { return acc + p.mass(); }
    ));
    const auto merged_pos = position_t(std::ranges::fold_left(
        particles,
        runtime_nd_unit_t{},
        [&total_mass](auto const& acc, auto const& p) {
            return acc + p.mass().magnitude() / total_mass.magnitude() * p.position();
        }
    ));
    const auto merged_vel = velocity_t(std::ranges::fold_left(
        particles,
        runtime_nd_unit_t{},
        [&total_mass](auto const& acc, auto const& p) {
            return acc + p.mass().magnitude() / total_mass.magnitude() * p.velocity();
        }
    ));
    return particle_t(total_mass, merged_pos, merged_vel, ParticleType::fictitious);
}

template <std::size_t N, std::floating_point F>
auto operator<<(std::ostream& os, ndparticle<N, F> pp) noexcept -> std::ostream&
{
    os << "ID: " << pp.id() << '\n'
       << pp.position() << '\n'
       << pp.mass() << '\n'
       << pp.velocity();
    return os;
}

}; // namespace pm::particle
