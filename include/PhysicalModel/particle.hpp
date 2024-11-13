#ifndef INLCLUDED_PARTICLE
#define INLCLUDED_PARTICLE

#include "concepts.hpp"
#include "ndtree.hpp"
#include "physical_magnitudes.hpp"
#include <chrono>
#include <tuple>
#include <type_traits>

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
    constexpr ndparticle(mass_t m, position_t pos, velocity_t vel, acceleration_t acc) :
        m_id{ ID++ },
        m_mass{ m },
        m_position{ pos },
        m_velocity{ vel },
        m_acceleration{ acc }
    {
    }

public:
    constexpr auto id() const noexcept -> id_t
    {
        return m_id;
    }

    constexpr auto update_position(utility::concepts::Duration auto delta_t
    ) noexcept -> void
    {
        using duration_t = std::chrono::duration<float>; // float seconds
        const auto d_t   = std::chrono::duration_cast<duration_t>(delta_t).count();
        const auto d_v   = m_acceleration * d_t;
        m_position += (m_velocity + value_type{ 0.5 } * d_v) * d_t;
        m_velocity += d_v;
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
    constexpr auto acceleration(this auto&& self) noexcept -> auto&&
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
    constexpr auto acceleration() noexcept -> acceleration_t&
    {
        return m_acceleration;
    }

    [[nodiscard]]
    constexpr auto properties() noexcept -> decltype(auto)
    {
        return std::tie(m_mass, m_velocity, m_acceleration);
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
    constexpr auto acceleration() const noexcept -> acceleration_t const&
    {
        return m_acceleration;
    }

    [[nodiscard]]
    constexpr auto properties() const noexcept -> decltype(auto)
    {
        return std::tie(m_mass, m_velocity, m_acceleration);
    }
#endif

    constexpr auto operator==(ndparticle const&) const -> bool = default;
    constexpr auto operator!=(ndparticle const&) const -> bool = default;

private:
    id_t           m_id;
    mass_t         m_mass;
    position_t     m_position;
    velocity_t     m_velocity;
    acceleration_t m_acceleration;
};

template <std::size_t N, std::floating_point F>
auto operator<<(std::ostream& os, ndparticle<N, F> pp) noexcept -> std::ostream&
{
    os << "ID: " << pp.id() << '\n'
       << pp.position() << '\n'
       << pp.mass() << '\n'
       << pp.velocity() << '\n'
       << pp.acceleration();
    return os;
}

}; // namespace pm::particle

#endif // INLCLUDED_PARTICLE
