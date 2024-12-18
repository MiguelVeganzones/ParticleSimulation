#include <limits>
#undef USE_BOOST_LOGGING
#undef USE_ROOT_PLOTTING
#include "physical_vector.hpp"
#include "random.hpp"
#include <array>
#include <gtest/gtest.h>

TEST(VectorBasicsTest, VectorFundamentals)
{
    using F                 = float;
    static constexpr auto N = 150;
    using vector_t          = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 250; ++j)
    {
        std::array<F, N> data{};

        vector_t v;
        for (std::size_t i = 0; i != N; ++i)
        {
            data[i] = utility::random::srandom::randfloat<F>();
            v[i]    = data[i];
        }

        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_EQ(v[i], data[i]);
        }
    }
}

TEST(VectorOperationsTest, VectorScalarPlus)
{
    using F                 = float;
    static constexpr auto N = 250;

    using vector_t = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 250; ++j)
    {
        std::array<F, N> data{};

        vector_t   tmp;
        const auto r = utility::random::srandom::randfloat<F>();
        for (std::size_t i = 0; i != N; ++i)
        {
            data[i] = utility::random::srandom::randfloat<F>();
            tmp[i]  = data[i];
        }

        vector_t v = tmp + r;
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(v[i], data[i] + r, std::numeric_limits<F>::epsilon());
        }
    }
}

TEST(VectorOperationsTest, VectorVectorPlus)
{
    using F                 = float;
    static constexpr auto N = 250;

    using vector_t = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 250; ++j)
    {
        std::array<F, N> data1{};
        std::array<F, N> data2{};
        vector_t         tmp1;
        vector_t         tmp2;

        for (std::size_t i = 0; i != N; ++i)
        {
            data1[i] = utility::random::srandom::randfloat<F>();
            data2[i] = utility::random::srandom::randfloat<F>();
            tmp1[i]  = data1[i];
            tmp2[i]  = data2[i];
        }

        const auto v1 = tmp1 + tmp2;
        const auto v2 = tmp2 + tmp1;
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(v1[i], data1[i] + data2[i], std::numeric_limits<F>::epsilon());
            EXPECT_NEAR(v2[i], data1[i] + data2[i], std::numeric_limits<F>::epsilon());
        }
    }
}

TEST(VectorOperationsTest, VectorScalarMinus)
{
    using F                 = float;
    static constexpr auto N = 250;

    using vector_t = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 250; ++j)
    {
        std::array<F, N> data{};

        vector_t   tmp;
        const auto r = utility::random::srandom::randfloat<F>();
        for (std::size_t i = 0; i != N; ++i)
        {
            data[i] = utility::random::srandom::randfloat<F>();
            tmp[i]  = data[i];
        }

        vector_t v = tmp - r;
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(v[i], data[i] - r, std::numeric_limits<F>::epsilon());
        }
    }
}

TEST(VectorOperationsTest, VectorVectorMinus)
{
    using F                 = float;
    static constexpr auto N = 250;

    using vector_t = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 250; ++j)
    {
        std::array<F, N> data1{};
        std::array<F, N> data2{};
        vector_t         tmp1;
        vector_t         tmp2;

        for (std::size_t i = 0; i != N; ++i)
        {
            data1[i] = utility::random::srandom::randfloat<F>();
            data2[i] = utility::random::srandom::randfloat<F>();
            tmp1[i]  = data1[i];
            tmp2[i]  = data2[i];
        }

        const auto v = tmp1 - tmp2;
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(v[i], data1[i] - data2[i], std::numeric_limits<F>::epsilon());
        }
    }
}

TEST(VectorOperationsTest, VectorScalarMultiplies)
{
    using F                 = float;
    static constexpr auto N = 250;

    using vector_t = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 200; ++j)
    {
        std::array<F, N> data{};
        vector_t         tmp;

        for (std::size_t i = 0; i != N; ++i)
        {
            data[i] = utility::random::srandom::randfloat<F>();
            tmp[i]  = data[i];
        }
        const auto r  = utility::random::srandom::randfloat<F>();
        const auto v1 = tmp * r;
        const auto v2 = r * tmp;
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(v1[i], data[i] * r, std::numeric_limits<F>::epsilon());
            EXPECT_NEAR(v2[i], data[i] * r, std::numeric_limits<F>::epsilon());
        }
    }
}

TEST(VectorOperationsTest, VectorVectorMultiplies)
{
    using F                 = float;
    static constexpr auto N = 250;

    using vector_t = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 200; ++j)
    {
        std::array<F, N> data1{};
        std::array<F, N> data2{};
        vector_t         tmp1;
        vector_t         tmp2;

        for (std::size_t i = 0; i != N; ++i)
        {
            data1[i] = utility::random::srandom::randfloat<F>();
            data2[i] = utility::random::srandom::randfloat<F>();
            tmp1[i]  = data1[i];
            tmp2[i]  = data2[i];
        }
        const auto v1 = tmp1 * tmp2;
        const auto v2 = tmp2 * tmp1;
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(v1[i], data1[i] * data2[i], std::numeric_limits<F>::epsilon());
            EXPECT_NEAR(v2[i], data1[i] * data2[i], std::numeric_limits<F>::epsilon());
        }
    }
}

TEST(VectorOperationsTest, VectorScalarDivides)
{
    using F                 = float;
    static constexpr auto N = 250;

    using vector_t = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 200; ++j)
    {
        std::array<F, N> data{};
        vector_t         tmp;

        for (std::size_t i = 0; i != N; ++i)
        {
            data[i] = utility::random::srandom::randfloat<F>();
            tmp[i]  = data[i];
        }
        const auto r  = utility::random::srandom::randfloat<F>();
        const auto v1 = tmp / r;
        const auto v2 = r / tmp;
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(v1[i], data[i] / r, std::numeric_limits<F>::epsilon());
            EXPECT_NEAR(v2[i], r / data[i], std::numeric_limits<F>::epsilon());
        }
    }
}

TEST(VectorOperationsTest, VectorVectorDivides)
{
    using F                 = float;
    static constexpr auto N = 250;

    using vector_t = pm::magnitudes::physical_vector<N, F>;

    for (std::size_t j = 0; j != 200; ++j)
    {
        std::array<F, N> data1{};
        std::array<F, N> data2{};
        vector_t         tmp1;
        vector_t         tmp2;

        for (std::size_t i = 0; i != N; ++i)
        {
            data1[i] = utility::random::srandom::randfloat<F>();
            data2[i] = utility::random::srandom::randfloat<F>();
            tmp1[i]  = data1[i];
            tmp2[i]  = data2[i];
        }
        const auto v1 = tmp1 / tmp2;
        const auto v2 = tmp2 / tmp1;
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(v1[i], data1[i] / data2[i], std::numeric_limits<F>::epsilon());
            EXPECT_NEAR(v2[i], data2[i] / data1[i], std::numeric_limits<F>::epsilon());
        }
    }
}
