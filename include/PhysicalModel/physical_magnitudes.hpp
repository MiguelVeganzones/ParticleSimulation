#pragma once

#include "casts.hpp"
#include "particle_concepts.hpp"
#include <concepts>
#include <functional>
#include <iostream>
#include <string_view>
#include <type_traits>

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
    _runtime_unit_,
};

} // namespace pm::units

namespace pm
{

using namespace units;

template <std::floating_point F>
struct physical_constants
{
    static_assert(std::is_same_v<F, double>);
    inline static constexpr auto G = static_cast<F>(0.000000000066743);
};

} // namespace pm

namespace pm::magnitudes
{

template <typename T>
concept physical_magnitude_concept = requires {
    std::remove_reference_t<T>::s_dimension;
    std::remove_reference_t<T>::s_units;
    typename std::remove_reference_t<T>::value_type;
};

template <std::size_t N, std::floating_point F, auto U>
struct physical_magnitude
{
    using value_type                         = F;
    inline static constexpr auto s_dimension = N;
    inline static constexpr auto s_units     = U;
    using container_t                        = std::array<value_type, s_dimension>;
    container_t value_;

    inline auto assert_in_bounds(std::integral auto const idx) const -> void
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(s_dimension));
    }

#if __GNUC__ >= 14
    [[nodiscard]]
    auto value(this auto&& self) noexcept -> decltype(auto)
        requires(s_dimension == 1)
    {
        if constexpr (N == 1)
        {
            return std::forward<decltype(self)>(self)[0];
        }
        else
        {
            return std::forward<decltype(self)>(self).value_;
        }
    }

    [[nodiscard]]
    auto operator[](this auto&& self, std::integral auto idx) -> decltype(auto)
    {
        std::forward<decltype(self)>(self).assert_in_bounds(idx);
        return std::forward<decltype(self)>(self).value_[idx];
    }
#else
    [[nodiscard]]
    auto value() noexcept -> delctype(auto)
    {
        if constexpr (N == 1)
        {
            return value_[0];
        }
        else
        {
            return value_;
        }
    }

    [[nodiscard]]
    auto value() const noexcept -> value_type const&
        requires(s_dimension == 1)
    {
        if constexpr (N == 1)
        {
            return value_[0];
        }
        else
        {
            return value_;
        }
    }

    [[nodiscard]]
    auto operator[](std::integral auto idx) const -> value_type
    {
        assert_in_bounds(idx);
        return value_[idx];
    }

    [[nodiscard]]
    auto operator[](std::integral auto idx) -> value_type&
    {
        assert_in_bounds(idx);
        return value_[idx];
    }
#endif

    [[nodiscard]]
    auto cbegin() const -> container_t::const_iterator
    {
        return std::cbegin(value_);
    }

    [[nodiscard]]
    auto cend() const -> container_t::const_iterator
    {
        return std::cend(value_);
    }

#if __GNUC__ >= 14
    [[nodiscard]]
    auto begin(this auto&& self) noexcept -> decltype(auto)
    {
        return std::begin(std::forward<decltype(self)>(self).value_);
    }

    [[nodiscard]]
    auto end(this auto&& self) noexcept -> decltype(auto)
    {
        return std::end(std::forward<decltype(self)>(self).value_);
    }
#else
    [[nodiscard]]
    auto begin() const noexcept -> container_t::const_iterator
    {
        return std::begin(value_);
    }

    [[nodiscard]]
    auto end() const noexcept -> container_t::const_iterator
    {
        return std::end(value_);
    }

    [[nodiscard]]
    auto begin() -> container_t::iterator
    {
        return std::begin(value_);
    }

    [[nodiscard]]
    auto end() -> container_t::iterator
    {
        return std::end(value_);
    }
#endif

    [[nodiscard]]
    constexpr auto operator<=>(physical_magnitude const&) const = default;
};

template <std::size_t N, std::floating_point F>
using position = physical_magnitude<N, F, units::Units::m>;
template <std::size_t N, std::floating_point F>
using distance = physical_magnitude<N, F, units::Units::m>;
template <std::size_t N, std::floating_point F>
using linear_velocity = physical_magnitude<N, F, units::Units::m_s>;
template <std::size_t N, std::floating_point F>
using linear_acceleration = physical_magnitude<N, F, units::Units::m_s2>;
template <std::size_t N, std::floating_point F>
using angular_position = physical_magnitude<N, F, units::Units::rad>;
template <std::size_t N, std::floating_point F>
using angular_velocity = physical_magnitude<N, F, units::Units::rad_s>;
template <std::size_t N, std::floating_point F>
using angular_acceleration = physical_magnitude<N, F, units::Units::rad_s2>;
template <std::floating_point F>
using mass = physical_magnitude<1, F, units::Units::kg>;
template <std::floating_point F>
using energy = physical_magnitude<1, F, units::Units::newton>;

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
    requires(concepts::Magnitude<T1> || concepts::Magnitude<T2>)
[[nodiscard]]
auto operator_impl(T1&& pma, T2&& pmb, auto&& binary_op) noexcept -> decltype(auto)
{
    constexpr auto at_idx = [](auto&& v, std::integral auto idx) noexcept
        requires(concepts::Magnitude<std::remove_reference_t<decltype(v)>> || std::is_floating_point_v<std::remove_reference_t<decltype(v)>>)
    {
        if constexpr (std::ranges::range<decltype(v)>)
        {
            return v[idx];
        }
        else if constexpr (std::is_floating_point_v<std::remove_reference_t<decltype(v)>>)
        {
            return v;
        }
        else
        {
            utility::error_handling::assert_unreachable();
        }
    };
    if constexpr (concepts::Magnitude<T1> && concepts::Magnitude<T2>)
    {
        physical_magnitude<
            T1::s_dimension,
            typename T1::value_type,
            units::Units::_runtime_unit_>
            ret{};
        for (auto i = 0uz; i != std::ranges::size(pma); ++i)
        {
            ret[i] = binary_op(pma[i], pmb[i]);
        }
        return ret;
    }
    else if constexpr (concepts::Magnitude<T1>)
    {
        physical_magnitude<T1::s_dimension, typename T1::value_type, T1::s_units> ret{};
        for (auto i = 0uz; i != std::ranges::size(pma); ++i)
        {
            ret[i] = binary_op(pma[i], at_idx(pmb, i));
        }
        return ret;
    }
    else if constexpr (concepts::Magnitude<T2>)
    {
        physical_magnitude<T2::s_dimension, typename T2::value_type, T2::s_units> ret{};
        for (auto i = 0uz; i != std::ranges::size(pmb); ++i)
        {
            ret[i] = binary_op(at_idx(pma, i), pmb[i]);
        }
        return ret;
    }
    else
    {
        return binary_op(at_idx(pma, 0), at_idx(pmb, 0));
    }
}

template <std::size_t N, std::floating_point F, auto U>
auto operator<<(std::ostream& os, physical_magnitude<N, F, U> const pm) noexcept
    -> std::ostream&
{
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
        case units::Units::_runtime_unit_: return "?";
        default: return "UNKNOWN";
        }
    };
    constexpr auto unit = unit_name(U);
    if constexpr (N == 1)
    {
        os << pm.value();
        os << '[' << unit << ']';
    }
    else
    {
        os << "{ ";
        std::size_t n{ 0 };
        for (auto const v : pm)
        {
            os << v << (++n != N ? ", " : " ");
        }
        os << '}';
        os << '[' << unit << ']';
    }

    return os;
}

} // namespace pm::magnitudes
