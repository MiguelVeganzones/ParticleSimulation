#pragma once

#include "particle_concepts.hpp"
#include "physical_vector.hpp"
#include "unit_system.hpp"
#include <concepts>
#include <functional>
#include <iostream>
#include <ranges>
#include <type_traits>

#ifdef USE_UNIT_SYSTEM
#include <initializer_list>
#include <string_view>
#endif

namespace pm::magnitudes
{

template <
    std::size_t         N,
    std::floating_point F
#ifdef USE_UNIT_SYSTEM
    ,
    units::Units U
#endif
    >
struct physical_magnitude
{
public:
    inline static constexpr auto _disambiguator_physical_magnitude_ = 0;
    using container_t                        = physical_vector<N, F>;
    using value_type                         = typename container_t::value_type;
    inline static constexpr auto s_dimension = container_t::s_dimension;
#ifdef USE_UNIT_SYSTEM
    inline static constexpr auto s_units = U;
#endif

#ifdef USE_UNIT_SYSTEM
    constexpr physical_magnitude() noexcept                          = default;
    constexpr physical_magnitude(physical_magnitude const&) noexcept = default;
    constexpr physical_magnitude(physical_magnitude&&) noexcept      = default;
    constexpr auto operator=(physical_magnitude const&) noexcept -> physical_magnitude& =
                                                                        default;
    constexpr auto operator=(physical_magnitude&&) noexcept -> physical_magnitude& =
                                                                   default;
    ~physical_magnitude() noexcept = default;

    physical_magnitude(std::initializer_list<value_type> init) noexcept :
        value_{ init }
    {
    }

    physical_magnitude(container_t const& v) noexcept :
        value_{ v }
    {
    }

    physical_magnitude(container_t&& v) noexcept :
        value_{ std::move(v) }
    {
    }

    template <particle_concepts::Magnitude Other>
        requires(std::remove_reference_t<Other>::s_units == units::Units::_runtime_unit_)
    constexpr physical_magnitude(Other&& other) noexcept :
        value_{ std::forward<Other>(other).value_ }
    {
    }

    template <particle_concepts::Magnitude Other>
        requires(std::remove_reference_t<Other>::s_units == units::Units::_runtime_unit_)
    constexpr auto operator=(Other&& other) noexcept -> physical_magnitude&
    {
        value_ = std::forward<Other>(other).value_;
        return *this;
    }
#endif

#if __GNUC__ >= 14
    template <typename Self>
    [[nodiscard]]
    auto value(this Self&& self) noexcept -> auto&&
    {
        return std::forward<Self>(self).value_;
    }

    template <typename Self>
    [[nodiscard]]
    auto magnitude(this Self&& self) noexcept -> auto&&
        requires(N == 1)
    {
        return std::forward<Self>(self).value_[0];
    }

    [[nodiscard]]
    auto operator[](this auto&& self, std::integral auto idx) -> auto&&
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
    auto magnitude() const noexcept -> value_type
        requires(N == 1)
    {
        return value_[0];
    }

    [[nodiscard]]
    auto magnitude() noexcept -> value_type&
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

    auto operator+=(auto&& other) noexcept -> decltype(auto)
    {
        value_ = operator_impl(
            value_, std::forward<decltype(other)>(other).value(), std::plus{}
        );
        return *this;
    }

    auto operator-=(auto&& other) noexcept -> decltype(auto)
    {
        value_ = operator_impl(
            value_, std::forward<decltype(other)>(other).value(), std::minus{}
        );
        return *this;
    }

    auto operator*=(auto&& other) noexcept -> decltype(auto)
    {
        value_ = operator_impl(
            value_, std::forward<decltype(other)>(other).value(), std::multiplies{}
        );
        return *this;
    }

    auto operator/=(auto&& other) noexcept -> decltype(auto)
    {
        value_ = operator_impl(
            value_, std::forward<decltype(other)>(other).value(), std::divides{}
        );
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator<=>(physical_magnitude const&) const = default;

public:
    container_t value_;
};

template <std::size_t N, std::floating_point F, auto U>
struct physical_magnitude_type_factory
{
#ifdef USE_UNIT_SYSTEM
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
template <std::floating_point F>
using charge = physical_magnitude_t<1, F, units::Units::coulomb>;
template <std::size_t N, std::floating_point F>
using force = physical_magnitude_t<N, F, units::Units::newton>;
template <std::floating_point F>
using energy = physical_magnitude_t<1, F, units::Units::joule>;
template <std::size_t N, std::floating_point F>
using runtime_unit = physical_magnitude_t<N, F, units::Units::_runtime_unit_>;

auto operator+(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
    requires particle_concepts::Magnitude<std::remove_reference_t<decltype(pma)>> ||
             particle_concepts::Magnitude<std::remove_reference_t<decltype(pmb)>>
{
    return operator_impl(
        std::forward<std::remove_reference_t<decltype(pma)>>(pma),
        std::forward<std::remove_reference_t<decltype(pmb)>>(pmb),
        std::plus{}
    );
}

auto operator-(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
    requires particle_concepts::Magnitude<std::remove_reference_t<decltype(pma)>> ||
             particle_concepts::Magnitude<std::remove_reference_t<decltype(pmb)>>
{
    return operator_impl(
        std::forward<std::remove_reference_t<decltype(pma)>>(pma),
        std::forward<std::remove_reference_t<decltype(pmb)>>(pmb),
        std::minus{}
    );
}

auto operator*(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
    requires particle_concepts::Magnitude<std::remove_reference_t<decltype(pma)>> ||
             particle_concepts::Magnitude<std::remove_reference_t<decltype(pmb)>>
{
    return operator_impl(
        std::forward<std::remove_reference_t<decltype(pma)>>(pma),
        std::forward<std::remove_reference_t<decltype(pmb)>>(pmb),
        std::multiplies{}
    );
}

auto operator/(auto&& pma, auto&& pmb) noexcept -> decltype(auto)
    requires particle_concepts::Magnitude<std::remove_reference_t<decltype(pma)>> ||
             particle_concepts::Magnitude<std::remove_reference_t<decltype(pmb)>>
{
    return operator_impl(
        std::forward<std::remove_reference_t<decltype(pma)>>(pma),
        std::forward<std::remove_reference_t<decltype(pmb)>>(pmb),
        std::divides{}
    );
}

template <particle_concepts::Magnitude Magnitude_Type>
auto max(Magnitude_Type const& pma, Magnitude_Type const& pmb) noexcept -> decltype(auto)
{
    return Magnitude_Type{ max(pma.value(), pmb.value()) };
}

template <particle_concepts::Magnitude Magnitude_Type>
auto min(Magnitude_Type const& pma, Magnitude_Type const& pmb) noexcept -> decltype(auto)
{
    return Magnitude_Type{ min(pma.value(), pmb.value()) };
}

template <typename T1, typename T2>
    requires(particle_concepts::Magnitude<std::remove_reference_t<T1>> ||
             particle_concepts::Magnitude<std::remove_reference_t<T2>>)
[[nodiscard]]
auto operator_impl(T1&& pma, T2&& pmb, auto&& binary_op) noexcept -> decltype(auto)
{
    using T1_t = std::remove_reference_t<T1>;
    using T2_t = std::remove_reference_t<T2>;
    if constexpr (particle_concepts::Magnitude<T1_t> &&
                  particle_concepts::Magnitude<T2_t>)
    {
        return physical_magnitude_t<
            T1_t::s_dimension,
            typename T1_t::value_type,
            units::Units::_runtime_unit_>{ operator_impl(
            std::forward<T1>(pma).value(),
            std::forward<T2>(pmb).value(),
            std::forward<decltype(binary_op)>(binary_op)
        ) };
    }
    else if constexpr (particle_concepts::Magnitude<T1_t>)
    {
        return physical_magnitude_t<
            T1_t::s_dimension,
            typename T1_t::value_type,
            units::Units::_runtime_unit_>{ operator_impl(
            std::forward<T1>(pma).value(),
            std::forward<T2>(pmb),
            std::forward<decltype(binary_op)>(binary_op)
        ) };
    }
    else if constexpr (particle_concepts::Magnitude<T2_t>)
    {
        return physical_magnitude_t<
            T2_t::s_dimension,
            typename T2_t::value_type,
            units::Units::_runtime_unit_>{ operator_impl(
            std::forward<T1>(pma),
            std::forward<T2>(pmb).value(),
            std::forward<decltype(binary_op)>(binary_op)
        ) };
    }
    else
    {
        utility::error_handling::assert_unreachable();
    }
}

template <
    std::size_t         N,
    std::floating_point F
#ifdef USE_UNIT_SYSTEM
    ,
    units::Units U
#endif
    >
auto operator<<(
    std::ostream& os,
    physical_magnitude<
        N,
        F
#ifdef USE_UNIT_SYSTEM
        ,
        U
#endif
        > const pm
) noexcept -> std::ostream&
{
    os << pm.value();
#ifdef USE_UNIT_SYSTEM
    os << '[' << pm::units::repr<U>() << ']';
#endif

    return os;
}

} // namespace pm::magnitudes
