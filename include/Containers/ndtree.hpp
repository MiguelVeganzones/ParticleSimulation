#ifndef INLCUDED_NDTREE
#define INLCUDED_NDTREE

#define DEBUG_NDTREE

#ifdef DEBUG_NDTREE
#include <iostream>
#endif
#include "../Utility/constexpr_functions.hpp"
#include "../Utility/error_handling.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <iomanip>
#include <numeric>
#include <optional>
#include <ostream>
#include <ranges>
#include <string>
#include <vector>

#define NDTREE_MAX_DIMENSIONS 8 // Por poner algo vaya

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

    auto mid(std::size_t idx) const -> F
    {
        return std::midpoint(max[idx], min[idx]);
    }
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

template <std::floating_point F, std::size_t N>
auto operator<<(std::ostream& os, ndboundary<F, N> b) -> std::ostream&
{
    os << "{ " << b.min << " }, { " << b.max << " }";
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
        if (p[i] < b.min[i] || p[i] > b.max[i])
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
    using box_t      = ndbox<T, F, N>;
    using depth_t    = int;

public:
    ndbox(
        boundary_t  boundary,
        std::size_t max_elements,
        depth_t     depth,
        depth_t     max_depth
    ) :
        m_boundary{ boundary },
        m_elements{},
        m_capacity{ max_elements },
        m_max_depth{ max_depth },
        m_depth{ depth }
    {
    }

    auto insert(sample_t& s) -> bool
    {
        if (!detail::in(s.position, m_boundary))
        {
            return false;
        }
        if (!m_fragmented)
        {
            if (std::get<0>(m_elements).size() < m_capacity || m_depth == m_max_depth)
            {
                std::cout << "Value at " << s.position << " stored in Box at depth "
                          << m_depth << " with bounds " << m_boundary << '\n';
                std::get<0>(m_elements).push_back(&s);
                return true;
            }
            else
            {
                std::cout << "########################\n";
                fragment();
                std::cout << "------------------------\n";
            }
        }
        if (m_fragmented)
        {
            for (auto& b : std::get<1>(m_elements))
            {
                if (const auto success = b.insert(s); success)
                {
                    return true;
                }
            }
            return false;
        }
        utility::error_handling::assert_unreachable();
    }

    auto print_info(std::ostream& os) const -> void
    {
        static auto header = [](auto depth) { return std::string(depth, '\t'); };
        os << header(m_depth - 1) << "<ndbox<T,F," << N << ">>\n";
        os << header(m_depth) << "Boundary: " << m_boundary << '\n';
        os << header(m_depth) << "Capacity " << m_capacity << '\n';
        os << header(m_depth) << "Depth " << m_depth << '\n';
        os << header(m_depth) << "Fragmented: " << m_fragmented << '\n';
        os << header(m_depth) << "Boxes: " << boxes() << '\n';
        if (!m_fragmented)
        {
            os << header(m_depth)
               << "Elements: " << std::ranges::size(std::get<0>(m_elements)) << '\n';
            for (auto const& e : std::get<0>(m_elements))
            {
                os << header(m_depth) << e->position << '\n';
            }
        }
        else
        {
            for (auto const& b : std::get<1>(m_elements))
            {
                b.print_info(os);
            }
        }
        os << header(m_depth - 1) << "<\\ndbox<T,F" << N << ">>\n";
    }

    [[nodiscard]]
    auto boxes() const -> std::size_t
    {
        return m_fragmented
                   ? std::ranges::fold_left(
                         std::get<1>(m_elements),
                         0,
                         [](auto nboxes, const auto& b) { return 1 + nboxes + b.boxes(); }
                     )
                   : 0;
    }

private:
    auto fragment() -> void
    {
        if (m_fragmented)
        {
            return;
        }
        auto samples = std::move(std::get<0>(m_elements));
        m_elements   = std::vector<ndbox>();
        std::get<1>(m_elements).reserve(s_nd_fanout);
        for (auto binary_div = decltype(s_nd_fanout){ 0 }; binary_div != s_nd_fanout;
             ++binary_div)
        {
            boundary_t bounds;
            for (auto i = decltype(N){ 0 }; i != N; ++i)
            {
                const auto top_half = (binary_div & (1 << i)) > 0;
                bounds.min[i]       = top_half ? m_boundary.mid(i) : m_boundary.min[i];
                bounds.max[i]       = top_half ? m_boundary.max[i] : m_boundary.mid(i);
            }
            std::get<1>(m_elements)
                .push_back(box_t{ bounds, m_capacity, m_depth + 1, m_max_depth });
        }
        for (auto* s : samples)
        {
            if (!s)
            {
                continue;
            }
            for (auto& b : std::get<1>(m_elements))
            {
                if (const auto success = b.insert(*s); success)
                {
                    break;
                }
            }
        }
        m_fragmented = true;
    }

private:
    boundary_t                                               m_boundary;
    std::variant<std::vector<sample_t*>, std::vector<ndbox>> m_elements;
    std::size_t                                              m_capacity;
    bool                                                     m_fragmented = false;
    depth_t                                                  m_max_depth;
    depth_t                                                  m_depth;
};

template <typename PType, typename F, std::size_t N>
    requires(N > 0 && N < NDTREE_MAX_DIMENSIONS)
class ndtree
{
public:
    using value_type = F;
    using size_type  = std::size_t;
    using depth_t    = int;
    using box_t      = ndbox<PType, value_type, N>;

public:
    inline static constexpr auto s_dimension = box_t::s_dimension;
    inline static constexpr auto s_1d_fanout = box_t::s_1d_fanout;
    inline static constexpr auto s_nd_fanout = box_t::s_nd_fanout;

public:
    using point_t      = ndpoint<value_type, s_dimension>;
    using boundary_t   = ndboundary<value_type, s_dimension>;
    using sample_t     = ndsample<PType, F, s_dimension>;
    using parent_index = std::size_t;
    template <typename T>
    using container = std::vector<T>;

public:
    ndtree(
        std::span<sample_t>       collection,
        depth_t const             max_depth,
        size_type const           box_capacity,
        std::optional<boundary_t> limits = std::nullopt
    ) :
        m_data_view{ collection },
        m_box(
            limits.has_value() ? limits.value() : detail::compute_limits(collection),
            box_capacity,
            1,
            max_depth
        ),
        m_max_depth{ max_depth },
        m_capacity{ box_capacity }
    {
        for (auto& e : collection)
        {
            insert(e, m_box);
        }
    }

    auto insert(sample_t& p, box_t& b) -> bool
    {
        return m_box.insert(p);
    }

    auto print_info(std::ostream& os = std::cout) const -> void
    {
        os << "<ndtree<T,F," << N << ">>\n";
        os << "Capacity: " << m_capacity << '\n';
        os << "Max depth: " << m_max_depth << '\n';
        os << "Elements: " << std::ranges::size(m_data_view) << '\n';
        os << "<\\ndtree<T,F," << N << ">>\n";
    }

    auto box() const -> box_t const&
    {
        return m_box;
    }

private:
    std::span<sample_t> m_data_view;
    box_t               m_box;
    depth_t             m_max_depth;
    size_type           m_capacity;
};

template <typename PType, typename F, std::size_t N>
auto operator<<(std::ostream& os, ndtree<PType, F, N> const& tree) -> std::ostream&
{
    tree.print_info(os);
    tree.box().print_info(os);
    return os;
}

} // namespace ndt

#endif // INLCUDED_NDTREE
