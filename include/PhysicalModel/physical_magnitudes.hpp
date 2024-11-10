#pragma once

#include "casts.hpp"
#include "particle_concepts.hpp"
#include "unit_system.hpp"
#include <algorithm>
#include <concepts>
#include <functional>
#include <iostream>
#include <ranges>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace pm::magnitudes
{

template <std::size_t N, std::floating_point F>
struct physical_vector
{
    using value_type                                             = F;
    using size_type                                              = decltype(N);
    inline static constexpr auto s_dimension                     = N;
    inline static constexpr auto _disambiguator_physical_vector_ = 0;
    using container_t    = std::array<value_type, s_dimension>;
    using const_iterator = typename container_t::const_iterator;
    using iterator       = typename container_t::iterator;

    inline auto assert_in_bounds(std::integral auto const idx) const -> void
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(s_dimension));
    }

#if __GNUC__ >= 14
    [[nodiscard]]
    auto value(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).value_;
    }

    [[nodiscard]]
    auto operator[](this auto&& self, std::integral auto idx) -> decltype(auto)
    {
        std::forward<decltype(self)>(self).assert_in_bounds(idx);
        return std::forward<decltype(self)>(self).value_[idx];
    }
#else
    [[nodiscard]]
    auto value() noexcept -> container_t&
    {
        return value_;
    }

    [[nodiscard]]
    auto value() const noexcept -> const container_t&
    {
        return value_;
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

public:
    container_t value_;
};

template <std::size_t N, std::floating_point F>
[[nodiscard]]
auto operator-(physical_vector<N, F> const& v1, physical_vector<N, F> const& v2) noexcept
    -> physical_vector<N, F>
{
    using vector_t = physical_vector<N, F>;
    vector_t    ret{};
    const auto& view = std::views::zip(v1, v2);
    std::transform(std::cbegin(view), std::cend(view), std::begin(ret), [](auto v) {
        return std::get<0>(v) - std::get<1>(v);
    });
    return ret;
}

auto operator/(concepts::Vector auto const& v, std::floating_point auto value) noexcept
    -> std::remove_cvref_t<decltype(v)>
{
    using vector_t = std::remove_cvref_t<decltype(v)>;
    vector_t ret{};
    std::transform(std::cbegin(v), std::cend(v), std::begin(ret), [value](auto e) {
        return e / value;
    });
    return ret;
}

auto operator*(concepts::Vector auto const& v, std::floating_point auto value) noexcept
    -> std::remove_cvref_t<decltype(v)>
{
    using vector_t = std::remove_cvref_t<decltype(v)>;
    vector_t ret{};
    std::transform(std::cbegin(v), std::cend(v), std::begin(ret), [value](auto e) {
        return e * value;
    });
    return ret;
}

template <
    std::size_t         N,
    std::floating_point F
#if USE_UNIT_SYSTEM
    ,
    auto U
#endif
    >
struct physical_magnitude
{
public:
    inline static constexpr auto _disambiguator_physical_magnitude_ = 0;
    using container_t                        = physical_vector<N, F>;
    using value_type                         = typename container_t::value_type;
    inline static constexpr auto s_dimension = container_t::s_dimension;
#if USE_UNIT_SYSTEM
    inline static constexpr auto s_units = U;
#endif

#if __GNUC__ >= 14
    [[nodiscard]]
    auto value() const noexcept -> container_t const&
    {
        return value_;
    }

    [[nodiscard]]
    auto value() noexcept -> container_t&
    {
        return value_;
    }

    [[nodiscard]]
    auto value() const noexcept -> value_type
        requires(N == 1)
    {
        return value_[0];
    }

    [[nodiscard]]
    auto value() noexcept -> value_type&
        requires(N == 1)
    {
        return value_[0];
    }

    [[nodiscard]]
    auto operator[](this auto&& self, std::integral auto idx) -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).value_[idx];
    }
#else
    [[nodiscard]]
    auto value() const noexcept -> container_t const&
    {
        return value_;
    }

    [[nodiscard]]
    auto value() noexcept -> container_t&
    {
        return value_;
    }

    [[nodiscard]]
    auto value() const noexcept -> value_type
        requires(N == 1)
    {
        return value_[0];
    }

    [[nodiscard]]
    auto value() noexcept -> value_type&
        requires(N == 1)
    {
        return value_[0];
    }

    [[nodiscard]]
    auto operator[](std::integral auto idx) const -> value_type
    {
        return value_[idx];
    }

    [[nodiscard]]
    auto operator[](std::integral auto idx) -> value_type&
    {
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

public:
    container_t value_;
};

template <std::size_t N, std::floating_point F, auto U>
struct physical_magnitude_type_factory
{
#if USE_UNIT_SYSTEM
    using type = physical_magnitude<N, F, U>;
#else
    using type = physical_magnitude<N, F>;
#endif
};

template <std::size_t N, std::floating_point F, auto U>
using physical_magnitude_t = typename physical_magnitude_type_factory<N, F, U>::type;

template <std::size_t N, std::floating_point F>
using position = physical_magnitude_t<N, F, units::Units::m>;
template <std::size_t N, std::floating_point F>
using distance = physical_magnitude_t<N, F, units::Units::m>;
template <std::size_t N, std::floating_point F>
using linear_velocity = physical_magnitude_t<N, F, units::Units::m_s>;
template <std::size_t N, std::floating_point F>
using linear_acceleration = physical_magnitude_t<N, F, units::Units::m_s2>;
template <std::size_t N, std::floating_point F>
using angular_position = physical_magnitude_t<N, F, units::Units::rad>;
template <std::size_t N, std::floating_point F>
using angular_velocity = physical_magnitude_t<N, F, units::Units::rad_s>;
template <std::size_t N, std::floating_point F>
using angular_acceleration = physical_magnitude_t<N, F, units::Units::rad_s2>;
template <std::floating_point F>
using mass = physical_magnitude_t<1, F, units::Units::kg>;
template <std::size_t N, std::floating_point F>
using force = physical_magnitude_t<N, F, units::Units::newton>;

auto operator+(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
{
    return operator_impl(
        std::forward<decltype(pma)>(pma), std::forward<decltype(pmb)>(pmb), std::plus{}
    );
}

auto operator-(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
{
    return operator_impl(
        std::forward<decltype(pma)>(pma), std::forward<decltype(pmb)>(pmb), std::minus{}
    );
}

auto operator*(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
{
    return operator_impl(
        std::forward<decltype(pma)>(pma),
        std::forward<decltype(pmb)>(pmb),
        std::multiplies{}
    );
}

auto operator/(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
{
    return operator_impl(
        std::forward<decltype(pma)>(pma), std::forward<decltype(pmb)>(pmb), std::divides{}
    );
}

template <typename T1, typename T2>
    requires(concepts::Magnitude<T1> || concepts::Magnitude<T2>)
[[nodiscard]]
auto operator_impl(T1&& pma, T2&& pmb, auto&& binary_op) noexcept -> decltype(auto)
{
    using T1_t            = std::remove_reference_t<T1>;
    using T2_t            = std::remove_reference_t<T2>;
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
    if constexpr (concepts::Magnitude<T1_t>)
    {
        physical_magnitude_t<
            T1_t::s_dimension,
            typename T1_t::value_type,
            units::Units::_runtime_unit_>
            ret{};
        for (auto i = 0uz; i != std::ranges::size(pma); ++i)
        {
            ret[i] = binary_op(pma[i], at_idx(pmb, i));
        }
        return ret;
    }
    else if constexpr (concepts::Magnitude<T2_t>)
    {
        physical_magnitude_t<
            T2_t::s_dimension,
            typename T2_t::value_type,
            units::Units::_runtime_unit_>
            ret{};
        for (auto i = 0uz; i != std::ranges::size(pmb); ++i)
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

template <
    std::size_t         N,
    std::floating_point F
#if USE_UNIT_SYSTEM
    ,
    auto U
#endif
    >
auto operator<<(
    std::ostream& os,
    physical_magnitude<
        N,
        F
#if USE_UNIT_SYSTEM
        ,
        U
#endif
        > const pm
) noexcept -> std::ostream&
{
#if USE_UNIT_SYSTEM
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
#endif
    if constexpr (N == 1)
    {
        os << pm.value();
#if USE_UNIT_SYSTEM
        os << '[' << unit << ']';
#endif
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
#if USE_UNIT_SYSTEM
        os << '[' << unit << ']';
#endif
    }

    return os;
}

} // namespace pm::magnitudes
