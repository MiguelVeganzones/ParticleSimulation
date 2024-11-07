#ifndef INLCLUDED_PARTICLE
#define INLCLUDED_PARTICLE

#include "ndtree.hpp"
#include "physical_magnitudes.hpp"
#include <tuple>
#include <type_traits>

namespace particle
{

template <std::size_t N, std::floating_point F>
class ndparticle
{
public:
    using value_type                         = F;
    inline static constexpr auto s_dimension = N;
    using position_t                         = pm::position<s_dimension, value_type>;
    using mass_t                             = pm::mass<value_type>;
    using velocity_t     = pm::linear_velocity<s_dimension, value_type>;
    using acceleration_t = pm::linear_acceleration<s_dimension, value_type>;

public:
    constexpr ndparticle(
        position_t const& pos,
        mass_t            m,
        velocity_t        vel,
        acceleration_t    acc
    ) :
        m_position{ pos },
        m_mass{ m },
        m_velocity{ vel },
        m_acceleration{ acc }
    {
    }

public:
#if __GNUC__ > 14
    [[nodiscard]]
    constexpr auto position(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).m_position;
    }

    [[nodiscard]]
    constexpr auto mass(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).m_mass;
    }

    [[nodiscard]]
    constexpr auto velocity(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).m_velocity;
    }

    [[nodiscard]]
    constexpr auto acceleration(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).m_acceleration;
    }

    [[nodiscard]]
    constexpr auto properties(this auto&& self) noexcept -> decltype(auto)
    {
        return std::tie(
            std::forward<decltype(self)>(self).m_mass,
            std::forward<decltype(self)>(self).m_velocity,
            std::forward<decltype(self)>(self).m_acceleration
        );
    }
#else
    [[nodiscard]]
    constexpr auto position() noexcept -> decltype(auto)
    {
        return m_position;
    }

    [[nodiscard]]
    constexpr auto mass() noexcept -> decltype(auto)
    {
        return m_mass;
    }

    [[nodiscard]]
    constexpr auto velocity() noexcept -> decltype(auto)
    {
        return m_velocity;
    }

    [[nodiscard]]
    constexpr auto acceleration() noexcept -> decltype(auto)
    {
        return m_acceleration;
    }

    [[nodiscard]]
    constexpr auto properties() noexcept -> decltype(auto)
    {
        return std::tie(m_mass, m_velocity, m_acceleration);
    }

    [[nodiscard]]
    constexpr auto position() const noexcept -> decltype(auto)
    {
        return m_position;
    }

    [[nodiscard]]
    constexpr auto mass() const noexcept -> decltype(auto)
    {
        return m_mass;
    }

    [[nodiscard]]
    constexpr auto velocity() const noexcept -> decltype(auto)
    {
        return m_velocity;
    }

    [[nodiscard]]
    constexpr auto acceleration() const noexcept -> decltype(auto)
    {
        return m_acceleration;
    }

    [[nodiscard]]
    constexpr auto properties() const noexcept -> decltype(auto)
    {
        return std::tie(m_mass, m_velocity, m_acceleration);
    }
#endif

private:
    position_t     m_position;
    mass_t         m_mass;
    velocity_t     m_velocity;
    acceleration_t m_acceleration;
};

template <std::size_t N, std::floating_point F>
auto operator<<(std::ostream& os, ndparticle<N, F> pp) noexcept -> std::ostream&
{
    os << pp.position() << '\n'
       << pp.mass() << '\n'
       << pp.velocity() << '\n'
       << pp.acceleration();
    return os;
}

}; // namespace particle

#endif // INLCLUDED_PARTICLE
