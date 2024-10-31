#ifndef INLCLUDED_PARTICLE
#define INLCLUDED_PARTICLE

#include "../Containers/ndtree.hpp" // Tight coupling to ndpoint. Improve?
#include "physical_magnitudes.hpp"
#include <type_traits>

namespace particle
{

template <std::size_t N, std::floating_point F>
class properties
{
public:
    using value_type                         = F;
    inline static constexpr auto a_dimension = N;
    using position_t                         = ndt::ndpoint<N, value_type>;
    using mass_t                             = pm::mass<value_type>;
    using velocity_t                         = pm::linear_velocity<value_type>;
    using acceleration_t                     = pm::linear_acceleration<value_type>;

public:
    constexpr properties(
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
    [[nodiscard]]
    constexpr auto position(this auto&& self) -> decltype(auto)
    {
        return self.m_position;
    }

    [[nodiscard]]
    constexpr auto mass(this auto&& self) -> decltype(auto)
    {
        return self.m_mass;
    }

    [[nodiscard]]
    constexpr auto velocity(this auto&& self) -> decltype(auto)
    {
        return self.m_velocity;
    }

    [[nodiscard]]
    constexpr auto acceleration(this auto&& self) -> decltype(auto)
    {
        return self.m_acceleration;
    }

private:
    position_t     m_position;
    mass_t         m_mass;
    velocity_t     m_velocity;
    acceleration_t m_acceleration;
};

template <std::size_t N, std::floating_point F>
auto operator<<(std::ostream& os, properties<N, F> pp) noexcept -> std::ostream&
{
    os << pp.position() << '\n'
       << pp.mass() << '\n'
       << pp.velocity() << '\n'
       << pp.acceleration() << '\n';
    return os;
}

}; // namespace particle

#endif // INLCLUDED_PARTICLE
