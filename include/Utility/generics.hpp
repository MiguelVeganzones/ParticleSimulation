#ifndef INCLUDED_UTILITY_GENERICS
#define INCLUDED_UTILITY_GENERICS

#include <algorithm>
#include <type_traits>

namespace utility::generics
{

template <typename T>
struct interval
{
    T min;
    T max;
};

template <std::floating_point F>
class unsigned_normalized
{
public:
    inline static constexpr auto s_interval = interval<F>{ 0, 1 };

    constexpr explicit unsigned_normalized(F value) noexcept :
        value_(std::clamp(value, s_interval.min, s_interval.max))
    {
    }

    [[nodiscard]]
    auto value() const noexcept -> F
    {
        return value_;
    }

private:
    F value_;
};

template <typename T>
class ranged_value
{
public:
    using value_type = T;
    using range_type = interval<T>;

public:
    constexpr ranged_value(value_type value, range_type range) :
        range_{ range },
        value_{ clamp_impl(value, range) }
    {
    }

    [[nodiscard]]
    auto get() const noexcept -> value_type
    {
        return value_;
    }

    auto set(value_type value) noexcept -> void
    {
        value_ = clamp_impl(value, range_);
    }

    [[nodiscard]]
    auto get_range() const noexcept -> range_type
    {
        return range_;
    }

private:
    [[nodiscard]]
    inline static constexpr auto clamp_impl(value_type value, range_type range) noexcept
        -> value_type
    {
        return clamp(value, range);
    }

private:
    range_type range_;
    value_type value_;
};

template <typename T>
[[nodiscard]]
auto clamp(T value, interval<T> limits) noexcept -> T
{
    return std::clamp(value, limits.min, limits.max);
}

template <typename T>
[[nodiscard]]
auto in(T value, interval<T> limits) noexcept -> bool
{
    return (value >= limits.min) && (value <= limits.max);
}

template <typename T, typename F>
[[nodiscard]]
auto index_interval(interval<T> const& interval, unsigned_normalized<F> index)
    -> std::common_type_t<T, F>
{
    const auto idx = static_cast<std::common_type_t<T, F>>(index.value());
    const auto min = static_cast<std::common_type_t<T, F>>(interval.min);
    const auto max = static_cast<std::common_type_t<T, F>>(interval.max);
    return min + (max - min) * idx;
}

} // namespace utility::generics

#endif // INCLUDED_UTILITY_GENERICS
