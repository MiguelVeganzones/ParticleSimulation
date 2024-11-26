#pragma once

#include <type_traits>
#define DEBUG_NDTREE 1

#include "constexpr_functions.hpp"
#include "error_handling.hpp"
#include "logging.hpp"
#include <algorithm>
#include <array>
#include <cassert>
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

namespace concepts
{
template <typename T>
concept Point = requires(T t) {
    typename T::value_type;
    T::s_dimension;
    t[0];
    std::begin(t);
    std::end(t);
};

template <typename T>
concept boundary_concept = requires(T t) {
    t.min();
    t.max();
    t.mid(0);
};

template <typename T>
concept sample_concept = requires(T t) {
    typename T::value_type;
    T::s_dimension;
    { t.position() } -> std::convertible_to<typename T::position_t>;
    t.properties();
    { merge(std::array{ t, t }) } -> std::same_as<T>;
} && std::is_destructible_v<T>;

} // namespace concepts

template <concepts::Point Point_Type>
class ndboundary
{
public:
    using point_t                            = Point_Type;
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
    auto min(index_t const idx) const -> value_type
    {
        return m_min[idx];
    }

    [[nodiscard]]
    auto max(index_t const idx) const -> value_type
    {
        return m_max[idx];
    }

    auto mid(index_t const idx) const -> value_type
    {
        return std::midpoint(m_max[idx], m_min[idx]);
    }

    [[nodiscard]]
    constexpr auto operator<=>(ndboundary const&) const = default;

private:
    point_t m_min;
    point_t m_max;
};

template <concepts::Point Point_Type>
auto operator<<(std::ostream& os, ndboundary<Point_Type> const& b) -> std::ostream&
{
    os << "{ " << b.min() << " }, { " << b.max() << " }";
    return os;
}

namespace detail
{

template <concepts::Point Point_Type>
[[nodiscard]]
auto in(Point_Type const& p, ndboundary<Point_Type> const& b) noexcept -> bool
{
    for (auto i = decltype(Point_Type::s_dimension){ 0 }; i != Point_Type::s_dimension;
         ++i)
    {
        if (p[i] < b.min(i) || p[i] > b.max(i))
        {
            return false;
        }
    }
    return true;
}

template <concepts::Point Point_Type>
[[nodiscard]]
auto count_in(
    std::ranges::range auto const& collection,
    ndboundary<Point_Type> const&  b
) noexcept -> std::size_t
{
    return std::ranges::count_if(collection, [b](auto const& p) { return in(p, b); });
}

[[nodiscard]]
auto compute_limits(std::ranges::range auto const& data) noexcept
    requires concepts::sample_concept<std::ranges::range_value_t<decltype(data)>>
{
    using sample_t   = std::ranges::range_value_t<decltype(data)>;
    using point_t    = typename sample_t::position_t;
    constexpr auto N = point_t::s_dimension;
    point_t        min;
    point_t        max;
    for (auto i = decltype(N){ 0 }; i != N; ++i)
    {
        const auto bounds =
            std::ranges::minmax(data | std::views::transform([i](auto const& p) -> auto {
                                    return p.position()[i];
                                }));
        min[i] = bounds.min;
        max[i] = bounds.max;
    }
    return ndboundary<point_t>{ min, max };
}

} // namespace detail

template <concepts::sample_concept Sample_Type>
class ndbox
{
public:
    using sample_t                           = Sample_Type;
    using point_t                            = typename sample_t::position_t;
    using box_t                              = ndbox<sample_t>;
    inline static constexpr auto s_dimension = point_t::s_dimension;
    using value_type                         = typename sample_t::value_type;
    inline static constexpr auto s_simension = sample_t::s_dimension;
    using boundary_t                         = ndboundary<point_t>;
    using depth_t                            = unsigned int;
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
        m_summary{},
        m_capacity{ max_elements },
        m_max_depth{ max_depth },
        m_depth{ depth }
    {
    }

    [[nodiscard]]
    auto insert(sample_t const& s) noexcept -> bool
    {
        if (!detail::in(s.position(), m_boundary))
        {
            return false;
        }
        if (!m_fragmented)
        {
            auto&& elements = contained_elements();
            if (elements.size() < m_capacity || m_depth == m_max_depth)
            {
#if DEBUG_NDTREE
                std::cout << "Value at " << s.position() << " stored in Box at depth "
                          << m_depth << " with bounds " << m_boundary << '\n';
#endif
                elements.push_back(const_cast<sample_t*>(&s));
                return true;
            }
            else
            {
#if DEBUG_NDTREE
                utility::logging::default_source::log(
                    utility::logging::severity_level::info, "Fragentation started"
                );
#endif
                fragment();
#if DEBUG_NDTREE
                utility::logging::default_source::log(
                    utility::logging::severity_level::info, "Fragentation finished"
                );
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

    auto cache_summary() noexcept -> void
    {
        if (m_fragmented)
        {
            for (auto&& b : subboxes())
            {
                b.cache_summary();
            }
            m_summary = merge(std::views::transform(subboxes(), [](auto const& b) {
                return b.summary();
            }));
        }
        else
        {
            m_summary =
                merge(std::views::transform(contained_elements(), [](auto const& b) {
                    return *b;
                }));
        }
    }

    [[nodiscard]]
    auto summary() const noexcept -> sample_t const&
    {
        return m_summary;
    }

    auto print_info(std::ostream& os) const -> void
    {
        static auto header = [](auto depth) { return std::string(depth, '\t'); };
        os << header(m_depth) << "<ndbox<" << s_dimension << ">>\n ";
        os << header(m_depth + 1) << "Boundary: " << m_boundary << '\n';
        os << header(m_depth + 1) << "Capacity " << m_capacity << '\n';
        os << header(m_depth + 1) << "Depth " << m_depth << '\n';
        os << header(m_depth + 1) << "Fragmented: " << m_fragmented << '\n';
        os << header(m_depth + 1) << "Boxes: " << boxes() << '\n';
        os << header(m_depth + 1) << "Summary: " << summary() << '\n';
        os << header(m_depth + 1) << "Elements: " << elements() << '\n';
        if (!m_fragmented)
        {
            auto&& elements = contained_elements();
            for (auto const& e : elements)
            {
                os << header(m_depth + 1) << e->position() << '\n';
            }
        }
        else
        {
            for (auto const& b : subboxes())
            {
                b.print_info(os);
            }
        }
        os << header(m_depth) << "<\\ndbox<" << s_dimension << ">>\n";
    }

    [[nodiscard]]
    auto boxes() const -> std::size_t
    {
        return m_fragmented ? std::ranges::fold_left(
                                  subboxes(),
                                  std::ranges::size(subboxes()),
                                  [](auto acc, const auto& b) { return acc + b.boxes(); }
                              )
                            : 0;
    }

    [[nodiscard]]
    auto elements() const -> std::size_t
    {
        return m_fragmented
                   ? std::ranges::fold_left(
                         subboxes(),
                         0uz,
                         [](auto acc, const auto& e) { return acc + e.elements(); }
                     )
                   : std::ranges::size(contained_elements());
    }

private:
    // because you cannot portably have a macro expansion (assert) inside #if #endif
    inline auto assert_fragmented() const noexcept -> void
    {
        assert(m_fragmented);
    }

    inline auto assert_not_fragmented() const noexcept -> void
    {
        assert(!m_fragmented);
    }

#if __GNUC__ >= 14
    [[nodiscard]]
    auto contained_elements(this auto&& self) noexcept -> auto&&
    {
        std::forward<decltype(self)>(self).assert_not_fragmented();
        return std::get<0>(std::forward<decltype(self)>(self).m_elements);
    }

    [[nodiscard]]
    auto subboxes(this auto&& self) noexcept -> auto&&
    {
        std::forward<decltype(self)>(self).assert_fragmented();
        return std::get<1>(std::forward<decltype(self)>(self).m_elements);
    }
#else
    [[nodiscard]]
    auto contained_elements() noexcept -> auto&
    {
        assert_not_fragmented();
        return std::get<0>(m_elements);
    }

    [[nodiscard]]
    auto contained_elements() const noexcept -> auto const&
    {
        assert_not_fragmented();
        return std::get<0>(m_elements);
    }

    [[nodiscard]]
    auto subboxes() noexcept -> auto&
    {
        assert_fragmented();
        return std::get<1>(m_elements);
    }

    [[nodiscard]]
    auto subboxes() const noexcept -> auto const&
    {
        assert_fragmented();
        return std::get<1>(m_elements);
    }
#endif

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
            for (auto i = decltype(s_dimension){ 0 }; i != s_dimension; ++i)
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
    sample_t                                                 m_summary;
    std::size_t                                              m_capacity;
    bool                                                     m_fragmented = false;
    depth_t                                                  m_max_depth;
    depth_t                                                  m_depth;
};

template <concepts::sample_concept Sample_Type>
    requires(
        Sample_Type::position_t::s_dimension > 0 &&
        Sample_Type::position_t::s_dimension < NDTREE_MAX_DIMENSIONS
    )
class ndtree
{
public:
    using sample_t                           = Sample_Type;
    using position_t                         = typename sample_t::position_t;
    using value_type                         = typename sample_t::value_type;
    using size_type                          = std::size_t;
    inline static constexpr auto s_dimension = sample_t::s_dimension;
    using box_t                              = ndbox<sample_t>;
    using depth_t                            = typename box_t::depth_t;
    using point_t                            = typename sample_t::position_t;
    using boundary_t                         = ndboundary<point_t>;
    template <typename T>
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
            0uz,
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

    auto cache_summary() noexcept -> void
    {
        m_box.cache_summary();
    }

    auto print_info(std::ostream& os = std::cout) const -> void
    {
        os << "<ndtree <" << s_dimension << ">>\n";
        os << "Capacity: " << m_capacity << '\n';
        os << "Max depth: " << m_max_depth << '\n';
        os << "Elements: " << m_box.elements() << " out of "
           << std::ranges::size(m_data_view) << '\n';
        os << "Summary: " << m_box.summary() << '\n';
        os << "<\\ndtree<" << s_dimension << ">>\n";
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

template <concepts::sample_concept Sample_Type>
auto operator<<(std::ostream& os, ndtree<Sample_Type> const& tree) -> std::ostream&
{
    tree.print_info(os);
    tree.box().print_info(os);
    return os;
}

} // namespace ndt
