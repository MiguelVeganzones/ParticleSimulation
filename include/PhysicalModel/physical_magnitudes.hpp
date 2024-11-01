#ifndef INCLUDED_PHYSICAL_MAGNIFUDES
#define INCLUDED_PHYSICAL_MAGNIFUDES

#include "casts.hpp"
#include <iostream>
#include <string_view>
#include <type_traits>

namespace pm
{

enum struct PhysicalMagnitudeUnits
{
    m,
    m_s,
    m_s2,
    rad, // Radians are adimensional... Look away
    rad_s,
    rad_s2,
    kg,
};

template <std::size_t N, std::floating_point F, PhysicalMagnitudeUnits Units>
struct physical_magnitude
{
    using value_type                         = F;
    inline static constexpr auto s_dimension = N;
    inline static constexpr auto units       = Units;
    using container_t                        = std::array<value_type, N>;
    container_t value;

    [[nodiscard]]
    auto operator[](this auto&& self, std::integral auto idx) -> decltype(auto)
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(self.s_dimension));
        return std::forward<decltype(self)>(self).value[idx];
    }

    [[nodiscard]]
    auto cbegin() const -> container_t::const_iterator
    {
        return std::cbegin(value);
    }

    [[nodiscard]]
    auto cend() const -> container_t::const_iterator
    {
        return std::cend(value);
    }

    [[nodiscard]]
    auto begin(this auto&& self) -> decltype(auto)
    {
        return std::begin(std::forward<decltype(self)>(self).value);
    }

    [[nodiscard]]
    auto end(this auto&& self) -> decltype(auto)
    {
        return std::end(std::forward<decltype(self)>(self).value);
    }

    [[nodiscard]]
    constexpr auto operator<=>(physical_magnitude const&) const = default;
};

template <std::floating_point F, PhysicalMagnitudeUnits Units>
struct physical_magnitude<1, F, Units>
{
    using value_type                         = F;
    inline static constexpr auto s_dimension = 1;
    inline static constexpr auto units       = Units;
    value_type                   value;
    [[nodiscard]]
    constexpr auto operator<=>(physical_magnitude const&) const = default;
};

template <std::size_t N, std::floating_point F>
using position = physical_magnitude<N, F, PhysicalMagnitudeUnits::m>;
template <std::size_t N, std::floating_point F>
using distance = physical_magnitude<N, F, PhysicalMagnitudeUnits::m>;
template <std::size_t N, std::floating_point F>
using linear_velocity = physical_magnitude<N, F, PhysicalMagnitudeUnits::m_s>;
template <std::size_t N, std::floating_point F>
using angular_position = physical_magnitude<N, F, PhysicalMagnitudeUnits::rad>;
template <std::size_t N, std::floating_point F>
using linear_acceleration = physical_magnitude<N, F, PhysicalMagnitudeUnits::m_s2>;
template <std::size_t N, std::floating_point F>
using angular_velocity = physical_magnitude<N, F, PhysicalMagnitudeUnits::rad_s>;
template <std::size_t N, std::floating_point F>
using angular_acceleration = physical_magnitude<N, F, PhysicalMagnitudeUnits::rad_s2>;
template <std::floating_point F>
using mass = physical_magnitude<1, F, PhysicalMagnitudeUnits::kg>;

template <std::size_t N, std::floating_point F, PhysicalMagnitudeUnits U>
auto operator+(
    physical_magnitude<N, F, U> const pma,
    physical_magnitude<N, F, U>       pmb
) noexcept -> physical_magnitude<N, F, U>
{
    return pma.value + pmb.value;
}

template <std::size_t N, std::floating_point F, PhysicalMagnitudeUnits U>
auto operator-(
    physical_magnitude<N, F, U> const pma,
    physical_magnitude<N, F, U>       pmb
) noexcept -> physical_magnitude<N, F, U>
{
    return pma.value - pmb.value;
}

template <std::size_t N, std::floating_point F, PhysicalMagnitudeUnits U>
auto operator*(physical_magnitude<N, F, U> const pm, F scalar) noexcept
    -> physical_magnitude<N, F, U>
{
    return pm.value * scalar;
}

template <std::size_t N, std::floating_point F, PhysicalMagnitudeUnits U>
auto operator/(physical_magnitude<N, F, U> const pm, F scalar) noexcept
    -> physical_magnitude<N, F, U>
{
    return pm.value / scalar;
}

template <std::size_t N, std::floating_point F, PhysicalMagnitudeUnits U>
auto operator<<(std::ostream& os, physical_magnitude<N, F, U> const pm) noexcept
    -> std::ostream&
{
    constexpr auto unit_name = [](PhysicalMagnitudeUnits unit
                               ) noexcept -> std::string_view {
        switch (unit)
        {
        case PhysicalMagnitudeUnits::m: return "m";
        case PhysicalMagnitudeUnits::m_s: return "m/s";
        case PhysicalMagnitudeUnits::m_s2: return "m/s^2";
        case PhysicalMagnitudeUnits::rad: return "rad/s";
        case PhysicalMagnitudeUnits::rad_s: return "rad/s";
        case PhysicalMagnitudeUnits::rad_s2: return "rad/s^2";
        case PhysicalMagnitudeUnits::kg: return "kg";
        default: return "unknown";
        }
    };
    constexpr auto unit = unit_name(U); // Force compile time evaluation
    if constexpr (N == 1)
    {
        os << pm.value << '[' << unit << ']';
    }
    else
    {
        os << "{ ";
        for (auto const v : pm.value)
        {
            os << v << ", ";
        }
        os << "}[" << unit << ']';
    }
    return os;
}

} // namespace pm

#endif // INCLUDED_PHYSICAL_MAGNIFUDES
