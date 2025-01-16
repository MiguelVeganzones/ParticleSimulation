#pragma once

#include "casts.hpp"
#include "expression_templates.hpp"
#include "particle_concepts.hpp"
#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <ranges>
#include <type_traits>

// physical_vector cannot be in its own namespace unfortunately to enable ADL
// of physical_vector construction in direct list initialization of physical_magnitudes
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

    inline auto assert_in_bounds([[maybe_unused]] std::integral auto const idx) const
        -> void
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(s_dimension));
    }

    physical_vector(const auto& src) noexcept
    {
        for (size_type i = 0; i != s_dimension; ++i)
        {
            value_[i] = src[i];
        }
    }

    auto operator=(const auto& src) noexcept -> physical_vector&
    {
        for (size_type i = 0; i != s_dimension; ++i)
        {
            value_[i] = src[i];
        }
        return *this;
    }

    constexpr physical_vector() noexcept                       = default;
    constexpr physical_vector(physical_vector const&) noexcept = default;
    constexpr physical_vector(physical_vector&&) noexcept      = default;
    constexpr auto operator=(physical_vector const&) noexcept
        -> physical_vector&                                                  = default;
    constexpr auto operator=(physical_vector&&) noexcept -> physical_vector& = default;
    ~physical_vector() noexcept                                              = default;

#if __GNUC__ >= 14
    [[nodiscard]]
    auto value(this auto&& self) noexcept -> auto&&
    {
        return std::forward<decltype(self)>(self).value_;
    }

    [[nodiscard]]
    auto operator[](this auto&& self, std::integral auto idx) -> auto&&
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
    auto operator[](std::size_t idx) const -> value_type
    {
        assert_in_bounds(idx);
        return value_[idx];
    }

    [[nodiscard]]
    auto operator[](std::size_t idx) -> value_type&
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

    constexpr auto operator<=>(physical_vector const&) const = default;

public:
    container_t value_;
};

template <particle_concepts::Vector Vector_Type>
auto max(Vector_Type const& pva, Vector_Type const& pvb) noexcept -> decltype(auto)
{
    Vector_Type ret{};
    std::ranges::transform(std::views::zip(pva, pvb), std::begin(ret), [](const auto& e) {
        return std::max(std::get<0>(e), std::get<1>(e));
    });
    return ret;
}

template <particle_concepts::Vector Vector_Type>
auto min(Vector_Type const& pva, Vector_Type const& pvb) noexcept -> decltype(auto)
{
    Vector_Type ret{};
    std::ranges::transform(std::views::zip(pva, pvb), std::begin(ret), [](const auto& e) {
        return std::min(std::get<0>(e), std::get<1>(e));
    });
    return ret;
}

template <std::size_t N, std::floating_point F>
auto operator<<(std::ostream& os, physical_vector<N, F> const pv) noexcept
    -> std::ostream&
{
    os << "{ ";
    std::size_t n{ 0 };
    for (auto const v : pv)
    {
        os << v << (++n != N ? ", " : " ");
    }
    os << '}';

    return os;
}

} // namespace pm::magnitudes
