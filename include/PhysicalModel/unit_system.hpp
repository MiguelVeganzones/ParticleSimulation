#pragma once

#include <ostream>
#include <string_view>

namespace pm::units
{

enum struct Units
{
    s,
    m,
    m_s,
    m_s2,
    rad,
    rad_s,
    rad_s2,
    kg,
    newton,
    joule,
    coulomb,
    _runtime_unit_,
};

template <Units U>
[[nodiscard]]
inline constexpr auto repr() noexcept -> std::string_view
{
#ifdef USE_UNIT_SYSTEM
    constexpr auto unit_name = [](units::Units unit) noexcept -> std::string_view {
        switch (unit)
        {
        case units::Units::s: return "s";
        case units::Units::m: return "m";
        case units::Units::m_s: return "m/s";
        case units::Units::m_s2: return "m/s^2";
        case units::Units::rad: return "rad";
        case units::Units::rad_s: return "rad/s";
        case units::Units::rad_s2: return "rad/s^2";
        case units::Units::kg: return "kg";
        case units::Units::newton: return "N";
        case units::Units::joule: return "J";
        case units::Units::coulomb: return "C";
        case units::Units::_runtime_unit_: return "?";
        default: return "UNKNOWN";
        }
    };
    return unit_name(U);
#else
    return "";
#endif
}

} // namespace pm::units
