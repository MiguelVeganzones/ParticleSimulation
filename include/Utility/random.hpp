#ifndef INCLUDED_RANDOM_NUMBER_GENERATOR
#define INCLUDED_RANDOM_NUMBER_GENERATOR

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>

#ifdef max
#undef max
#endif

namespace utility::random
{

template <std::floating_point F>
class random
{
public:
    using value_type = F;

private:
    [[nodiscard]]
    static auto static_instance() noexcept -> random&
    {
        static random s_Random_engine(0);
        return s_Random_engine;
    }

public:
    static auto s_seed(unsigned int seed_ = std::random_device{}()) noexcept -> void
    {
        static_instance().seed_engine(seed_);
    }

    [[nodiscard]]
    static auto s_randfloat() noexcept -> value_type
    {
        return static_instance().randfloat();
    }

    /// @brief Generates a uniform integral number in the range [min, max]
    /// @tparam T Integral type
    /// @param min Inclusive lower bound
    /// @param max Inclusive upper bound
    /// @return integral number of type T uniformly distributed in the range
    /// [min, max]
    template <std::integral T>
    [[nodiscard]]
    inline static auto s_randintegral(T min, T max) noexcept -> T
    {
        return static_instance().randintegral(min, max);
    }

    [[nodiscard]]
    inline static auto s_randnormal(value_type avg, value_type stddev) noexcept
        -> value_type
    {
        return static_instance().randnormal(avg, stddev);
    }

    [[nodiscard]]
    inline static auto s_randnormal() noexcept -> value_type
    {
        return static_instance().randnormal();
    }

public:
    random(unsigned int seed = std::random_device{}()) noexcept
    {
        seed_engine(seed);
    }

    random(random const&) noexcept            = default;
    random(random&&) noexcept                 = default;
    random& operator=(random const&) noexcept = default;
    random& operator=(random&&) noexcept      = default;
    ~random() noexcept                        = default;

    [[nodiscard]]
    auto randfloat() noexcept -> value_type
    {
        return m_Uniform_real_(m_Random_engine_);
    }

    /// @brief Generates a uniform integral number in the range [min, max]
    /// @tparam T Integral type
    /// @param min Inclusive lower bound
    /// @param max Inclusive upper bound
    /// @return integral number of type T uniformly distributed in the range
    /// [min, max]
    template <std::integral T>
    [[nodiscard]]
    auto randintegral(T min, T max) noexcept -> T
    {
        assert(min <= max);
        std::uniform_int_distribution<T> uniform_dist(min, max);
        return uniform_dist(m_Random_engine_);
    }

    [[nodiscard]]
    auto randnormal(float avg, float stddev) noexcept -> float
    {
        std::normal_distribution<float> n(avg, stddev);
        return n(m_Random_engine_);
    }

    [[nodiscard]]
    auto randnormal() noexcept -> float
    {
        return m_Default_normal_(m_Random_engine_);
    }

private:
    inline auto seed_engine(unsigned int seed) noexcept -> void
    {
        m_Random_engine_.seed(seed);
    }

private:
    std::mt19937_64                            m_Random_engine_;
    std::uniform_real_distribution<value_type> m_Uniform_real_ =
        std::uniform_real_distribution<value_type>(0.f, 1.f);
    std::normal_distribution<value_type> m_Default_normal_ =
        std::normal_distribution<value_type>(0.f, 1.f);
};

} // namespace utility::random

#endif // INCLUDED_RANDOM_NUMBER_GENERATOR
