#pragma once

#include "casts.hpp"
#include <concepts>
#include <functional>
#include <iostream>
#include <string_view>
#include <type_traits>

#ifndef USE_UNIT_SYSTEM
#define USE_UNIT_SYSTEM 0
#endif

#if USE_UNIT_SYSTEM
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/ostream.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#endif

namespace pm
{

#if USE_UNIT_SYSTEM
using namespace mp_units;
constexpr auto m      = si::metre;
constexpr auto m_s    = si::metre / si::second;
constexpr auto m_s2   = si::metre / si::second / si::second;
constexpr auto rad    = si::radian;
constexpr auto rad_s  = si::radian / si::second;
constexpr auto rad_s2 = si::radian / si::second / si::second;
constexpr auto kg     = si::kilogram;
constexpr auto newton = si::newton;
#else
constexpr auto m      = 1;
constexpr auto m_s    = 1;
constexpr auto m_s2   = 1;
constexpr auto rad    = 1;
constexpr auto rad_s  = 1;
constexpr auto rad_s2 = 1;
constexpr auto kg     = 1;
constexpr auto newton = 1;
#endif


template <typename T>
concept physical_magnitude_concept = requires {
    std::remove_reference_t<T>::s_dimension;
    std::remove_reference_t<T>::s_units;
    typename std::remove_reference_t<T>::value_type;
};

template <std::size_t N, std::floating_point F, auto Unit>
struct physical_magnitude
{
    using value_type                         = F;
    inline static constexpr auto s_dimension = N;
    inline static constexpr auto s_units     = Unit;
    using container_t                        = std::array<value_type, N>;
    container_t value;

    inline auto assert_in_bounds(std::integral auto const idx) const -> void
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(s_dimension));
    }

#if __GNUC__ >= 14
    [[nodiscard]]
    auto operator[](this auto&& self, std::integral auto idx) -> decltype(auto)
    {
        std::forward<decltype(self)>(self).assert_in_bounds(idx);
        return std::forward<decltype(self)>(self).value[idx];
    }
#else
    [[nodiscard]]
    auto operator[](std::integral auto idx) const -> value_type
    {
        assert_in_bounds(idx);
        return value[idx];
    }

    [[nodiscard]]
    auto operator[](std::integral auto idx) -> value_type&
    {
        assert_in_bounds(idx);
        return value[idx];
    }
#endif

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

#if __GNUC__ >= 14
    [[nodiscard]]
    auto begin(this auto&& self) noexcept -> decltype(auto)
    {
        return std::begin(std::forward<decltype(self)>(self).value);
    }

    [[nodiscard]]
    auto end(this auto&& self) noexcept -> decltype(auto)
    {
        return std::end(std::forward<decltype(self)>(self).value);
    }
#else
    [[nodiscard]]
    auto begin() const noexcept -> container_t::const_iterator
    {
        return std::begin(value);
    }

    [[nodiscard]]
    auto end() const noexcept -> container_t::const_iterator
    {
        return std::end(value);
    }

    [[nodiscard]]
    auto begin() -> container_t::iterator
    {
        return std::begin(value);
    }

    [[nodiscard]]
    auto end() -> container_t::iterator
    {
        return std::end(value);
    }
#endif

    [[nodiscard]]
    constexpr auto operator<=>(physical_magnitude const&) const = default;
};

template <std::floating_point F, auto Unit>
struct physical_magnitude<1, F, Unit>
{
    using value_type                         = F;
    inline static constexpr auto s_dimension = 1;

#if USE_UNIT_SYSTEM
    inline static constexpr quantity s_units = 1 * Unit;
#else
    inline static constexpr auto s_units = 1 * Unit;
#endif
    value_type value;
    [[nodiscard]]
    constexpr auto operator<=>(physical_magnitude const&) const = default;
};

template <std::size_t N, std::floating_point F>
using position = physical_magnitude<N, F, m>;
template <std::size_t N, std::floating_point F>
using distance = physical_magnitude<N, F, m>;
template <std::size_t N, std::floating_point F>
using linear_velocity = physical_magnitude<N, F, m_s>;
template <std::size_t N, std::floating_point F>
using linear_acceleration = physical_magnitude<N, F, m_s2>;
template <std::size_t N, std::floating_point F>
using angular_position = physical_magnitude<N, F, rad>;
template <std::size_t N, std::floating_point F>
using angular_velocity = physical_magnitude<N, F, rad_s>;
template <std::size_t N, std::floating_point F>
using angular_acceleration = physical_magnitude<N, F, rad_s2>;
template <std::floating_point F>
using mass = physical_magnitude<1, F, kg>;
template <std::floating_point F>
using energy = physical_magnitude<1, F, newton>;

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
        T1 ret{};
        for (auto i = decltype(T1::s_dimension){ 0 }; i != T1::s_dimension; ++i)
        {
            ret[i] = binary_op(pma[i], at_idx(pmb, i));
        }
        return ret;
    }
    else if constexpr (std::ranges::range<decltype(pmb)>)
    {
        T2 ret{};
        for (auto i = decltype(T2::s_dimension){ 0 }; i != T2::s_dimension; ++i)
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

template <std::size_t N, std::floating_point F, auto U>
auto operator<<(std::ostream& os, physical_magnitude<N, F, U> const pm) noexcept
    -> std::ostream&
{
    if constexpr (N == 1)
    {
        os << pm.value;
#if USE_UNIT_SYSTEM
        os << '[' << decltype(pm)::s_units << ']';
#endif
    }
    else
    {
        os << "{ ";
        std::size_t n{ 0 };
        for (auto const v : pm.value)
        {
            os << v << (++n != N ? ", " : " ");
        }
        os << '}';
#if USE_UNIT_SYSTEM
        os << '[' << decltype(pm)::s_units << ']';
#endif
    }

    return os;
}

} // namespace pm
