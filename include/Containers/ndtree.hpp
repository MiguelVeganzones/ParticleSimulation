#ifndef INLCUDED_NDTREE
#define INLCUDED_NDTREE

#define DEBUG_NDTREE 1

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

#ifdef DEBUG_NDTREE
#include <iostream>
#endif

#define NDTREE_MAX_DIMENSIONS 64 // Because of how boundary subdivisions are computed

namespace ndt
{

template <std::size_t N, std::floating_point F>
struct ndpoint
{
    using value_type                         = F;
    inline static constexpr auto s_dimension = N;
    using container_t                        = std::array<value_type, s_dimension>;

    [[nodiscard]]
    auto operator[](std::integral auto idx) -> value_type&
    {
        assert(idx < N);
        return value[idx];
    }

    [[nodiscard]]
    auto operator[](std::integral auto idx) const -> value_type
    {
        assert(idx < N);
        return value[idx];
    }

    [[nodiscard]]
    auto begin() const -> container_t::const_iterator
    {
        return std::begin(value);
    }

    [[nodiscard]]
    auto end() -> container_t::iterator
    {
        return std::end(value);
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

    container_t value;
};

template <std::size_t N, std::floating_point F>
class ndboundary
{
public:
    using point_t                            = ndpoint<N, F>;
    using value_type                         = typename point_t::value_type;
    inline static constexpr auto s_dimension = point_t::s_dimension;
    using index_t                            = std::remove_const_t<decltype(s_dimension)>;

public:
    constexpr ndboundary() noexcept = default;

    constexpr ndboundary(point_t const& p1, point_t const& p2) noexcept
    {
        for (index_t i = 0; i != s_dimension; ++i)
        {

            // Explicit copy to avoid dangling reference to a temporary
            std::pair<value_type, value_type> r = std::minmax(p1[i], p2[i]);
            m_min[i]                            = r.first;
            m_max[i]                            = r.second;
        }
    }

    [[nodiscard]]
    auto min() const -> point_t const&
    {
        return m_min;
    }

    [[nodiscard]]
    auto max() const -> point_t const&
    {
        return m_max;
    }

    [[nodiscard]]
    auto min(index_t const idx) const -> F
    {
        return m_min[idx];
    }

    [[nodiscard]]
    auto max(index_t const idx) const -> F
    {
        return m_max[idx];
    }

    auto mid(index_t const idx) const -> F
    {
        return std::midpoint(m_max[idx], m_min[idx]);
    }

private:
    point_t m_min;
    point_t m_max;
};

template <std::size_t N, std::floating_point F, typename T>
struct ndsample
{
    using position_t                         = ndpoint<N, F>;
    using value_type                         = T;
    inline static constexpr auto s_dimension = position_t::s_dimension;
    position_t                   position;
    value_type                   properties;
};

template <std::size_t N, std::floating_point F>
auto operator<<(std::ostream& os, ndpoint<N, F> p) -> std::ostream&
{
    os << "{ ";
    for (auto e : p)
        os << e << ", ";
    os << "}";
    return os;
}

template <std::size_t N, std::floating_point F>
auto operator<<(std::ostream& os, ndboundary<N, F> b) -> std::ostream&
{
    os << "{ " << b.min() << " }, { " << b.max() << " }";
    return os;
}

namespace concepts
{
template <typename T>
concept point_concept = requires(T t) {
    typename T::value_type;
    t.value;
    T::s_dimension;
    t[0];
};

template <typename T>
concept boundary_concept = requires(T t) {
    t.min();
    t.max();
    t.mid(0);
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
[[nodiscard]]
auto in(ndpoint<N, F> const& p, ndboundary<N, F> const& b) noexcept -> bool
{
    for (auto i = decltype(N){ 0 }; i != N; ++i)
    {
        if (p[i] < b.min(i) || p[i] > b.max(i))
        {
            return false;
        }
    }
    return true;
}

template <std::floating_point F, std::size_t N>
[[nodiscard]]
auto count_in(
    std::ranges::range auto const& collection,
    ndboundary<N, F> const&        b
) noexcept -> std::size_t
{
    return std::ranges::count_if(collection, [b](auto const& p) { return in(p, b); });
}

[[nodiscard]]
auto compute_limits(std::ranges::range auto const& data) noexcept
    requires concepts::point_concept<std::ranges::range_value_t<decltype(data)>>
{
    using point_t    = std::ranges::range_value_t<decltype(data)>;
    constexpr auto N = point_t::s_dimension;
    point_t        min;
    point_t        max;
    for (auto i = decltype(N){ 0 }; i != N; ++i)
    {
        const auto bounds = std::ranges::minmax(
            data | std::views::transform([i](auto const& p) -> auto const& {
                return p.value[i];
            })
        );
        min[i] = bounds.min;
        max[i] = bounds.max;
    }
    return ndboundary<point_t::s_dimension, typename point_t::value_type>{ min, max };
}

[[nodiscard]]
auto compute_limits(std::ranges::range auto const& data) noexcept
    requires concepts::sample_concept<std::ranges::range_value_t<decltype(data)>>
{
    return compute_limits(data | std::views::transform([](auto const& s) -> auto const& {
                              return s.position;
                          }));
}
} // namespace detail

template <std::size_t N, std::floating_point F, typename T>
class ndbox
{
public:
    using point_t                            = ndpoint<N, F>;
    inline static constexpr auto s_dimension = point_t::s_dimension;

public:
    using boundary_t = ndboundary<s_dimension, F>;
    using sample_t   = ndsample<s_dimension, F, T>;
    using box_t      = ndbox<s_dimension, F, T>;
    using depth_t    = int;

public:
    inline static constexpr auto s_1d_fanout = std::size_t{ 2 };
    inline static constexpr auto s_nd_fanout =
        utility::cx_functions::pow(s_1d_fanout, s_dimension);

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

    [[nodiscard]]
    auto insert(sample_t const& s) noexcept -> bool
    {
        if (!detail::in(s.position, m_boundary))
        {
            return false;
        }
        if (!m_fragmented)
        {
            auto&& elements = contained_elements();
            if (elements.size() < m_capacity || m_depth == m_max_depth)
            {
#if DEBUG_NDTREE
                std::cout << "Value at " << s.position << " stored in Box at depth "
                          << m_depth << " with bounds " << m_boundary << '\n';
#endif
                elements.push_back(const_cast<sample_t*>(&s));
                return true;
            }
            else
            {
#if DEBUG_NDTREE
                std::cout << "########################\n";
#endif
                fragment();
#if DEBUG_NDTREE
                std::cout << "------------------------\n";
#endif
            }
        }
        if (m_fragmented)
        {
            for (auto&& b : subboxes())
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
        os << header(m_depth) << "<ndbox<T,F," << N << ">>\n";
        os << header(m_depth + 1) << "Boundary: " << m_boundary << '\n';
        os << header(m_depth + 1) << "Capacity " << m_capacity << '\n';
        os << header(m_depth + 1) << "Depth " << m_depth << '\n';
        os << header(m_depth + 1) << "Fragmented: " << m_fragmented << '\n';
        os << header(m_depth + 1) << "Boxes: " << boxes() << '\n';
        os << header(m_depth + 1) << "Elements: " << elements() << '\n';
        if (!m_fragmented)
        {
            auto&& elements = contained_elements();
            for (auto const& e : elements)
            {
                os << header(m_depth + 1) << e->position << '\n';
            }
        }
        else
        {
            for (auto const& b : subboxes())
            {
                b.print_info(os);
            }
        }
        os << header(m_depth) << "<\\ndbox<T,F" << N << ">>\n";
    }

    [[nodiscard]]
    auto boxes() const -> std::size_t
    {
        return m_fragmented
                   ? std::ranges::fold_left(
                         subboxes(),
                         0,
                         [](auto nboxes, const auto& b) { return 1 + nboxes + b.boxes(); }
                     )
                   : 0;
    }

    [[nodiscard]]
    auto elements() const -> std::size_t
    {
        return m_fragmented ? std::ranges::fold_left(
                                  subboxes(),
                                  0,
                                  [](auto nelements, const auto& e) {
                                      return nelements + e.elements();
                                  }
                              )
                            : std::ranges::size(contained_elements());
    }

private:
    [[nodiscard]]
    auto subboxes(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self.m_fragmented);
        return std::get<1>(self.m_elements);
    }

    [[nodiscard]]
    auto contained_elements(this auto&& self) noexcept -> decltype(auto)
    {
        assert(!self.m_fragmented);
        return std::get<0>(self.m_elements);
    }

    auto fragment() noexcept -> void
    {
        if (m_fragmented)
        {
            return;
        }
        auto samples = std::move(contained_elements());
        m_elements   = std::vector<ndbox>();
        m_fragmented = true;
        subboxes().reserve(s_nd_fanout);
        static_assert(
            s_1d_fanout == 2,
            "The divisions are only binary with implementation! It could be generalzed "
            "if needed tho..."
        );
        for (auto binary_div = decltype(s_nd_fanout){ 0 }; binary_div != s_nd_fanout;
             ++binary_div)
        {
            point_t min;
            point_t max;
            for (auto i = decltype(N){ 0 }; i != N; ++i)
            {
                const auto top_half = (binary_div & (1 << i)) > 0;
                min[i]              = top_half ? m_boundary.mid(i) : m_boundary.min(i);
                max[i]              = top_half ? m_boundary.max(i) : m_boundary.mid(i);
            }
            subboxes().push_back(box_t{
                boundary_t{ min, max }, m_capacity, m_depth + 1, m_max_depth });
        }
        for (auto const* const s : samples)
        {
            if (!s)
            {
                continue;
            }
            for (auto&& b : subboxes())
            {
                if (const auto success = b.insert(*s); success)
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
    depth_t                                                  m_max_depth;
    depth_t                                                  m_depth;
};

template <std::size_t N, std::floating_point Position_Impl_Type, typename Value_Type>
    requires(N > 0 && N < NDTREE_MAX_DIMENSIONS)
class ndtree
{
public:
    using sample_t   = ndsample<N, Position_Impl_Type, Value_Type>;
    using position_t = typename sample_t::position_t;
    using value_type = typename sample_t::value_type;
    using size_type  = std::size_t;
    using depth_t    = int;

public:
    inline static constexpr auto s_dimension = sample_t::s_dimension;

public:
    using box_t =
        ndbox<position_t::s_dimension, typename position_t::value_type, value_type>;
    using point_t    = ndpoint<s_dimension, typename position_t::value_type>;
    using boundary_t = ndboundary<s_dimension, typename position_t::value_type>;
    template <typename T>
    using container = std::vector<T>;

public:
    inline static constexpr auto s_1d_fanout = box_t::s_1d_fanout;
    inline static constexpr auto s_nd_fanout = box_t::s_nd_fanout;

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
            0,
            max_depth
        ),
        m_max_depth{ max_depth },
        m_capacity{ box_capacity }
    {
        for (auto& e : collection)
        {
            [[maybe_unused]]
            auto _ = insert(e);
        }
    }

    [[nodiscard]]
    auto insert(sample_t const& p) noexcept -> bool
    {
        return m_box.insert(p);
    }

    auto print_info(std::ostream& os = std::cout) const -> void
    {
        os << "<ndtree<T,F," << s_dimension << ">>\n";
        os << "Capacity: " << m_capacity << '\n';
        os << "Max depth: " << m_max_depth << '\n';
        os << "Elements: " << m_box.elements() << " out of "
           << std::ranges::size(m_data_view) << '\n';
        os << "<\\ndtree<T,F," << s_dimension << ">>\n";
    }

    [[nodiscard]]
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

template <std::size_t N, std::floating_point F, typename T>
auto operator<<(std::ostream& os, ndtree<N, F, T> const& tree) -> std::ostream&
{
    tree.print_info(os);
    tree.box().print_info(os);
    return os;
}

} // namespace ndt

#endif // INLCUDED_NDTREE
