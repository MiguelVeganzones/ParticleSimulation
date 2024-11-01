#ifndef INCLUDED_PHYSICAL_MAGNIFUDES
#define INCLUDED_PHYSICAL_MAGNIFUDES

#include "casts.hpp"
#include <concepts>
#include <functional>
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

template <typename T>
concept physical_magnitude_concept = requires {
    T::s_dimension;
    T::units;
    typename T::value_type;
};

template <std::size_t N, std::floating_point F, PhysicalMagnitudeUnits Units>
struct physical_magnitude
{
    using value_type                         = F;
    inline static constexpr auto s_dimension = N;
    inline static constexpr auto s_units     = Units;
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
    inline static constexpr auto s_units     = Units;
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

auto operator+(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
{
    return operator_impl(
        std::forward<decltype(pma)>(pma), std::forward<decltype(pmb)>(pmb), std::plus<>{}
    );
}

auto operator-(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
{
    return operator_impl(
        std::forward<decltype(pma)>(pma), std::forward<decltype(pmb)>(pmb), std::minus<>{}
    );
}

auto operator*(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
{
    return operator_impl(
        std::forward<decltype(pma)>(pma),
        std::forward<decltype(pmb)>(pmb),
        std::multiplies<>{}
    );
}

auto operator/(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
{
    return operator_impl(
        std::forward<decltype(pma)>(pma),
        std::forward<decltype(pmb)>(pmb),
        std::divides<>{}
    );
}

auto operator_impl(auto&& pma, auto&& pmb, auto&& binary_op) noexcept -> decltype(auto)
    requires(
        (std::is_floating_point_v<decltype(pma)> &&
         physical_magnitude_concept<decltype(pmb)>) ||
        (physical_magnitude_concept<decltype(pma)> &&
         std::is_floating_point_v<decltype(pmb)>) ||
        (physical_magnitude_concept<decltype(pma)> &&
         physical_magnitude_concept<decltype(pmb)> &&
         decltype(pma)::s_units == decltype(pmb)::s_units)
    )
{
    constexpr auto at_idx = [](auto&&             v,
                               std::integral auto idx) noexcept -> decltype(auto) {
        if constexpr (std::ranges::range<decltype(v)>)
        {
            return v[idx];
        }
        else
        {
            return v;
        }
    };
    if constexpr (std::ranges::range<decltype(pma)>)
    {
        decltype(pma) ret;
        for (auto i = decltype(decltype(pma)::s_dimension){ 0 };
             i != decltype(pma)::s_dimension;
             ++i)
        {
            ret[i] = binary_op(pma[i], at_idx(pmb, i));
        }
        return ret;
    }
    else if constexpr (std::ranges::range<decltype(pmb)>)
    {
        decltype(pmb) ret;
        for (auto i = decltype(decltype(pmb)::s_dimension){ 0 };
             i != decltype(pmb)::s_dimension;
             ++i)
        {
            ret[i] = binary_op(at_idx(pma, i), pmb[i]);
        }
        return ret;
    }
    else
    {
        utility::error_handling::assert_unreachable();
    }
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
