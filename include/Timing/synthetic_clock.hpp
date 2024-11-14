#pragma once

#include <chrono>

#ifndef PRINT_ELAPSED
#define PRINT_ELAPSED (false)
#if PRINT_ELAPSED
#include <iostream>
#endif
#endif

namespace synchronization
{

template <typename Duration_Type, auto Duration_Value>
struct tick_period
{
    using duration_t                             = Duration_Type;
    inline static constexpr auto period_duration = duration_t(Duration_Value);
};

namespace impl
{
template <typename T, auto V>
auto tick_period_concept_dummy(tick_period<T, V>) -> void
{
}

template <typename T>
concept tick_period_concept =
    requires { tick_period_concept_dummy(std::declval<std::remove_cvref_t<T>&>()); };
} // namespace impl

template <impl::tick_period_concept Tick_Period, std::size_t Clock_ID = 0>
class synthetic_clock
{
public:
    using clock_t                          = std::chrono::steady_clock;
    using rep                              = typename clock_t::rep;
    using period                           = typename clock_t::period;
    using duration                         = typename clock_t::duration;
    using time_point                       = typename clock_t::time_point;
    inline static constexpr auto is_steady = true;
    inline static constexpr auto clock_ID  = Clock_ID;

private:
    using tick_period_t = Tick_Period;
    using elapsed_t     = std::chrono::nanoseconds;

public:
    [[nodiscard]]
    inline static auto now() noexcept -> time_point
    {
        return time_point_;
    }

    inline static auto tick() noexcept -> void
    {
#if PRINT_ELAPSED
        std::cout
            << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_).count()
            << '\n';
#endif
        time_point_ += elapsed_;
    }

private:
    inline static time_point time_point_ = time_point{};
    inline static elapsed_t  elapsed_    = tick_period_t::period_duration;
};

} // namespace synchronization
