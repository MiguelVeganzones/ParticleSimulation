#ifndef INLCUDED_NTREE
#define INLCUDED_NTREE

#include "../Utility/constexpr_functions.hpp"
#include "../Utility/error_handling.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <optional>
#include <ostream>
#include <ranges>
#include <vector>

#define NTREE_MAX_DIMENSIONS 8 // Por poner algo vaya

namespace ndt
{

template <std::floating_point F, std::size_t N>
struct ndpoint
{
    using value_type                                  = F;
    inline static constexpr auto         s_dimensions = N;
    std::array<value_type, s_dimensions> value;

    auto operator[](std::integral auto idx) -> value_type&
    {
        assert(idx < N);
        return value[idx];
    }

    auto operator[](std::integral auto idx) const -> value_type
    {
        assert(idx < N);
        return value[idx];
    }

    auto begin() const
    {
        return std::begin(value);
    }

    auto end() const
    {
        return std::end(value);
    }
};

template <std::floating_point F, std::size_t N>
struct ndboundary
{
    using point_t = ndpoint<F, N>;
    point_t min;
    point_t max;
};

template <typename T, std::floating_point F, std::size_t N>
struct ndsample
{
    ndpoint<F, N> position;
    T             properties;
};

template <std::floating_point F, std::size_t N>
auto operator<<(std::ostream& os, ndpoint<F, N> p) -> std::ostream&
{
    os << "{ ";
    for (auto e : p)
        os << e << ", ";
    os << "}";
    return os;
}

namespace concepts
{
template <typename T>
concept point_concept = requires(T t) { t.value; };

template <typename T>
concept boundary_concept = requires(T t) {
    t.min;
    t.max;
};

template <typename T>
concept sample_concept = requires(T t) {
    t.position;
    t.properties;
};

} // namespace concepts

namespace detail
{

template <std::floating_point F, std::size_t N>
auto in(ndpoint<F, N> const& p, ndboundary<F, N> const& b) noexcept -> bool
{
    for (auto i = decltype(N){ 0 }; i != N; ++i)
    {
        if (p[i] < b.min[i] || p[i] >= b.max[i])
        {
            return false;
        }
    }
    return true;
}

template <std::floating_point F, std::size_t N>
auto count_in(std::ranges::range auto const& collection, ndboundary<F, N> const& b)
{
    return std::ranges::count_if(collection, [b](auto const& p) { return in(p, b); });
}

auto compute_limits(std::ranges::range auto const& data) noexcept
    requires concepts::point_concept<std::ranges::range_value_t<decltype(data)>>
{
    using point_t = std::ranges::range_value_t<decltype(data)>;
    constexpr auto                              N = point_t::s_dimensions;
    ndboundary<typename point_t::value_type, N> limits;
    for (auto i = decltype(N){ 0 }; i != N; ++i)
    {
        const auto bounds = std::ranges::minmax(
            data | std::views::transform([i](auto const& p) -> auto const& {
                return p.value[i];
            })
        );
        limits.min[i] = bounds.min;
        limits.max[i] = bounds.max;
    }
    return limits;
}

auto compute_limits(std::ranges::range auto const& data) noexcept
    requires concepts::sample_concept<std::ranges::range_value_t<decltype(data)>>
{
    return compute_limits(data | std::views::transform([](auto const& s) -> auto const& {
                              return s.position;
                          }));
}
} // namespace detail

template <typename T, std::floating_point F, std::size_t N>
class ndbox
{
public:
    inline static constexpr auto s_dimension = std::size_t{ N };
    inline static constexpr auto s_1d_fanout = std::size_t{ 2 };
    inline static constexpr auto s_nd_fanout =
        utility::cx_functions::pow(s_1d_fanout, s_dimension);

public:
    using point_t    = ndpoint<F, N>;
    using boundary_t = ndboundary<F, N>;
    using sample_t   = ndsample<T, F, N>;

public:
    ndbox(boundary_t boundary, std::size_t max_elements) :
        m_boundary{ boundary },
        m_elements{},
        m_capacity{ max_elements }
    {
    }

    auto insert(sample_t const& s) -> bool
    {
        if (!detail::in(s.position, m_boundary))
        {
            return false;
        }
        if (!m_fragmented)
        {
            if (std::get<0>(m_elements).size() < m_capacity)
            {
                std::get<0>(m_elements).push_back(&s);
                return true;
            }
            else
            {
                fragment();
            }
        }
        if (m_fragmented)
        {
            for (auto& c : std::get<1>(m_elements))
            {
                const auto success = c.insert(s);
                if (success)
                {
                    return true;
                }
            }
            return false;
        }
        utility::error_handling::assert_unreachable();
    }

private:
    auto fragment() -> void
    {
        if (m_fragmented)
        {
            return;
        }
        auto&& samples = std::move(std::get<0>(m_elements));
        m_elements     = std::vector<ndbox>(s_nd_fanout);
        for (auto s : samples)
        {
            for (auto& c : std::get<1>(m_elements))
            {
                if (const auto success = c.insert(s); success)
                {
                    break;
                }
            }
        }
    }

private:
    boundary_t                                               m_boundary;
    std::variant<std::vector<sample_t*>, std::vector<ndbox>> m_elements;
    std::size_t                                              m_capacity;
    bool                                                     m_fragmented = false;
};

template <typename PType, typename F, std::uint8_t N>
    requires(N > 0 && N < NTREE_MAX_DIMENSIONS)
class ntree
{
public:
    using value_type = F;
    using size_type  = std::size_t;
    using depth_t    = std::uint8_t;
    using box_t      = ndbox<PType, value_type, N>;

public:
    inline static constexpr auto s_dimension = box_t::s_dimension;
    inline static constexpr auto s_1d_fanout = box_t::s_1d_fanout;
    inline static constexpr auto s_nd_fanout = box_t::s_nd_fanout;

public:
    using point_t      = ndpoint<value_type, s_dimension>;
    using boundary_t   = ndboundary<value_type, s_dimension>;
    using parent_index = std::size_t;
    template <typename T>
    using container = std::vector<T>;

public:
    ntree(
        std::span<value_type>     collection,
        depth_t const             max_depth,
        size_type const           min_bin_capacity,
        std::optional<boundary_t> limits
    )
    {
        if (!limits.has_value())
        {
            limits = detail::compute_limits(collection);
        }
        for (auto& e : collection)
        {
            insert(e, m_box);
        }
    }

    auto insert(point_t const& p, boundary_t const& b) -> bool
    {
        return false;
    }

private:
    std::span<value_type> m_data_view;
    box_t                 m_box;
    depth_t               m_max_depth;
    size_type             min_bin_capacity;
};

} // namespace ndt

#endif // INLCUDED_NTREE
