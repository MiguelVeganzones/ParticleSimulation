#ifndef INCLUDED_PHYSICAL_MAGNIFUDES
#define INCLUDED_PHYSICAL_MAGNIFUDES

#include "casts.hpp"
#include "unit_system.hpp"
#include <concepts>
#include <functional>
#include <iostream>
#include <string_view>
#include <type_traits>

namespace pm
{

template <typename T>
concept physical_magnitude_concept = requires(T t) {
    T::s_dimension;
    T::s_units;
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

template <typename T1, typename T2>
auto operator_impl(T1&& pma, T2&& pmb, auto&& binary_op) noexcept -> decltype(auto)
    requires(
        (std::is_floating_point_v<T1> && physical_magnitude_concept<T2>) ||
        (physical_magnitude_concept<T1> && std::is_floating_point_v<T2>) ||
        (physical_magnitude_concept<T1> && physical_magnitude_concept<T2> &&
         T1::s_units == T2::s_units)
    )
{
    if constexpr (std::ranges::range<decltype(pma)> && std::ranges::range<decltype(pmb)>)
    {
        static_assert(T1::s_dimension == T2::s_dimension);
        T1 ret{};
        for (auto i = decltype(T1::s_dimension){ 0 }; i != T1::s_dimension; ++i)
        {
            ret[i] = binary_op(pma[i], pmb[i]);
        }
        return ret;
    }
    else if constexpr (std::ranges::range<decltype(pma)>)
    {
        T1 ret{};
        for (auto i = decltype(T1::s_dimension){ 0 }; i != T1::s_dimension; ++i)
        {
            ret[i] = binary_op(pma[i], pmb);
        }
        return ret;
    }
    else if constexpr (std::ranges::range<decltype(pmb)>)
    {
        T2 ret{};
        for (auto i = decltype(T2::s_dimension){ 0 }; i != T2::s_dimension; ++i)
        {
            ret[i] = binary_op(pma, pmb[i]);
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
    if constexpr (N == 1)
    {
        os << pm.value << U;
    }
    else
    {
        os << "{ ";
        std::size_t n{ 0 };
        for (auto const v : pm.value)
        {
            os << v << (++n > != N ? ", " : ' ');
        }
        os << '}' << unit;
    }
}

return os;
}

} // namespace pm

#endif // INCLUDED_PHYSICAL_MAGNIFUDES
