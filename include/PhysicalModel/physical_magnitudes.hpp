#ifndef INCLUDED_PHYSICAL_MAGNIFUDES
#define INCLUDED_PHYSICAL_MAGNIFUDES

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

template <std::floating_point F, PhysicalMagnitudeUnits Units>
struct physical_magnitude
{
    using value_type                   = F;
    inline static constexpr auto units = Units;
    value_type                   value;
    [[nodiscard]]
    constexpr auto operator<=>(physical_magnitude const&) const = default;
};

template <std::floating_point F>
using distance = physical_magnitude<F, PhysicalMagnitudeUnits::m>;
template <std::floating_point F>
using linear_velocity = physical_magnitude<F, PhysicalMagnitudeUnits::m_s>;
template <std::floating_point F>
using linear_acceleration = physical_magnitude<F, PhysicalMagnitudeUnits::m_s2>;
template <std::floating_point F>
using angular_position = physical_magnitude<F, PhysicalMagnitudeUnits::rad>;
template <std::floating_point F>
using angular_velocity = physical_magnitude<F, PhysicalMagnitudeUnits::rad_s>;
template <std::floating_point F>
using angular_acceleration = physical_magnitude<F, PhysicalMagnitudeUnits::rad_s2>;
template <std::floating_point F>
using mass = physical_magnitude<F, PhysicalMagnitudeUnits::kg>;

template <std::floating_point F, PhysicalMagnitudeUnits U>
auto operator+(physical_magnitude<F, U> const pma, physical_magnitude<F, U> pmb) noexcept
    -> physical_magnitude<F, U>
{
    return pma.value + pmb.value;
}

template <std::floating_point F, PhysicalMagnitudeUnits U>
auto operator-(physical_magnitude<F, U> const pma, physical_magnitude<F, U> pmb) noexcept
    -> physical_magnitude<F, U>
{
    return pma.value - pmb.value;
}

template <std::floating_point F, PhysicalMagnitudeUnits U>
auto operator*(physical_magnitude<F, U> const pm, F scalar) noexcept
    -> physical_magnitude<F, U>
{
    return pm.value * scalar;
}

template <std::floating_point F, PhysicalMagnitudeUnits U>
auto operator/(physical_magnitude<F, U> const pm, F scalar) noexcept
    -> physical_magnitude<F, U>
{
    return pm.value / scalar;
}

template <std::floating_point F, PhysicalMagnitudeUnits U>
auto operator<<(std::ostream& os, physical_magnitude<F, U> const pm) noexcept
    -> std::ostream&
{
    constexpr auto unit_name = [](PhysicalMagnitudeUnits unit
                               ) noexcept -> std::string_view {
        switch (unit)
        {
        case PhysicalMagnitudeUnits::m: return "m";
        case PhysicalMagnitudeUnits::m_s: return "m/s";
        case PhysicalMagnitudeUnits::m_s2: return "m/s^2";
        case PhysicalMagnitudeUnits::rad: return "rad";
        case PhysicalMagnitudeUnits::rad_s: return "rad/s";
        case PhysicalMagnitudeUnits::rad_s2: return "rad/s^2";
        case PhysicalMagnitudeUnits::kg: return "kg";
        default: return "unknown";
        }
    };
    constexpr auto unit = unit_name(U); // Force compile time evaluation
    os << pm.value << '[' << unit << ']';
    return os;
}

} // namespace pm

#endif // INCLUDED_PHYSICAL_MAGNIFUDES
