#pragma once

#include "concepts.hpp"
#include "physical_magnitudes.hpp"
#include <tuple>

#ifndef DEBUG_PRINT
#define DEBUG_PRINT (false)
#endif

namespace pm::particle
{

template <std::size_t N, std::floating_point F>
class ndparticle
{
public:
    using value_type                         = F;
    using size_type                          = decltype(N);
    using id_t                               = std::size_t;
    inline static constexpr auto s_dimension = N;
    using position_t      = magnitudes::position<s_dimension, value_type>;
    using mass_t          = magnitudes::mass<value_type>;
    using velocity_t      = magnitudes::linear_velocity<s_dimension, value_type>;
    using acceleration_t  = magnitudes::linear_acceleration<s_dimension, value_type>;
    inline static auto ID = 0uz;

public:
    constexpr ndparticle(mass_t m, position_t pos, velocity_t vel) :
        m_id{ ID++ },
        m_mass{ std::move(m) },
        m_position{ std::move(pos) },
        m_velocity{ std::move(vel) }
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
    constexpr auto properties() noexcept -> decltype(auto)
    {
        return std::tie(m_mass, m_velocity);
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
    constexpr auto properties() const noexcept -> decltype(auto)
    {
        return std::tie(m_mass, m_velocity);
    }
#endif

    constexpr auto operator==(ndparticle const&) const -> bool = default;
    constexpr auto operator!=(ndparticle const&) const -> bool = default;

private:
    id_t       m_id;
    mass_t     m_mass;
    position_t m_position;
    velocity_t m_velocity;
};

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
