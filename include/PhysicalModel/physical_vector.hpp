#pragma once

#include "casts.hpp"
#include "particle_concepts.hpp"
#include <algorithm>
#include <array>
#include <concepts>
#include <ranges>

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

    inline auto assert_in_bounds(std::integral auto const idx) const -> void
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(s_dimension));
    }

#if USE_UNIT_SYSTEM
    constexpr physical_vector() noexcept                       = default;
    constexpr physical_vector(physical_vector const&) noexcept = default;
    constexpr physical_vector(physical_vector&&) noexcept      = default;
    constexpr auto operator=(physical_vector const&) noexcept
        -> physical_vector&                                                  = default;
    constexpr auto operator=(physical_vector&&) noexcept -> physical_vector& = default;
    ~physical_vector() noexcept                                              = default;

    physical_vector(std::initializer_list<value_type> init) noexcept
    {
        std::copy(init.begin(), init.end(), value_.begin());
    }
#endif

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

auto operator+(auto&& pva, auto&& pvb) noexcept -> decltype(auto)
    requires particle_concepts::Vector<std::remove_reference_t<std::remove_reference_t<
                 std::remove_reference_t<std::remove_reference_t<decltype(pva)>>>>> ||
             particle_concepts::Vector<std::remove_reference_t<decltype(pvb)>>
{
    return operator_impl(
        std::forward<std::remove_reference_t<std::remove_reference_t<
            std::remove_reference_t<std::remove_reference_t<decltype(pva)>>>>>(pva),
        std::forward<std::remove_reference_t<decltype(pvb)>>(pvb),
        std::plus{}
    );
}

auto operator-(auto&& pva, auto&& pvb) noexcept -> decltype(auto)
    requires particle_concepts::Vector<std::remove_reference_t<std::remove_reference_t<
                 std::remove_reference_t<std::remove_reference_t<decltype(pva)>>>>> ||
             particle_concepts::Vector<std::remove_reference_t<decltype(pvb)>>
{
    return operator_impl(
        std::forward<std::remove_reference_t<std::remove_reference_t<
            std::remove_reference_t<std::remove_reference_t<decltype(pva)>>>>>(pva),
        std::forward<std::remove_reference_t<decltype(pvb)>>(pvb),
        std::minus{}
    );
}

auto operator*(auto&& pva, auto&& pvb) noexcept -> decltype(auto)
    requires particle_concepts::Vector<std::remove_reference_t<std::remove_reference_t<
                 std::remove_reference_t<std::remove_reference_t<decltype(pva)>>>>> ||
             particle_concepts::Vector<std::remove_reference_t<decltype(pvb)>>
{
    return operator_impl(
        std::forward<std::remove_reference_t<std::remove_reference_t<
            std::remove_reference_t<std::remove_reference_t<decltype(pva)>>>>>(pva),
        std::forward<std::remove_reference_t<decltype(pvb)>>(pvb),
        std::multiplies{}
    );
}

auto operator/(auto&& pva, auto&& pvb) noexcept -> decltype(auto)
    requires particle_concepts::Vector<std::remove_reference_t<std::remove_reference_t<
                 std::remove_reference_t<std::remove_reference_t<decltype(pva)>>>>> ||
             particle_concepts::Vector<std::remove_reference_t<decltype(pvb)>>
{
    return operator_impl(
        std::forward<std::remove_reference_t<std::remove_reference_t<
            std::remove_reference_t<std::remove_reference_t<decltype(pva)>>>>>(pva),
        std::forward<std::remove_reference_t<decltype(pvb)>>(pvb),
        std::divides{}
    );
}

template <typename T1, typename T2>
    requires(particle_concepts::Vector<std::remove_reference_t<T1>> || particle_concepts::Vector<std::remove_reference_t<T2>>)
[[nodiscard]]
auto operator_impl(T1&& pva, T2&& pvb, auto&& binary_op) noexcept -> decltype(auto)
{
    using T1_t            = std::remove_reference_t<T1>;
    using T2_t            = std::remove_reference_t<T2>;
    constexpr auto at_idx = [](auto&& v, std::integral auto idx) noexcept
        requires(particle_concepts::Vector<std::remove_reference_t<decltype(v)>> || std::is_floating_point_v<std::remove_reference_t<decltype(v)>>)
    {
        if constexpr (particle_concepts::Vector<std::remove_reference_t<decltype(v)>>)
        {
            return v[idx];
        }
        if constexpr (std::ranges::range<std::remove_reference_t<decltype(v)>>)
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
    if constexpr (particle_concepts::Vector<T1_t>)
    {
        physical_vector<T1_t::s_dimension, typename T1_t::value_type> ret{};
        for (auto i = 0uz; i != std::ranges::size(pva); ++i)
        {
            ret[i] = binary_op(pva[i], at_idx(pvb, i));
        }
        return ret;
    }
    else if constexpr (particle_concepts::Vector<T2_t>)
    {
        physical_vector<T2_t::s_dimension, typename T2_t::value_type> ret{};
        for (auto i = 0uz; i != std::ranges::size(pvb); ++i)
        {
            ret[i] = binary_op(at_idx(pva, i), pvb[i]);
        }
        return ret;
    }
    else
    {
        utility::error_handling::assert_unreachable();
    }
}

template <std::size_t N, std::floating_point F>
auto operator<<(std::ostream& os, physical_vector<N, F> const pv) noexcept
    -> std::ostream&
{
    if constexpr (N == 1)
    {
        os << pv.value();
    }
    else
    {
        os << "{ ";
        std::size_t n{ 0 };
        for (auto const v : pv)
        {
            os << v << (++n != N ? ", " : " ");
        }
        os << '}';
    }

    return os;
}

} // namespace pm::magnitudes
