#include <chrono>
#undef USE_BOOST_LOGGING
#undef USE_ROOT_PLOTTING
#undef DEBUG_NDTREE

#include "barnes_hut_approximation.hpp"
#include "brute_force.hpp"
#include "factory.hpp"
#include "particle.hpp"
#include "random_distributions.hpp"
#include "synthetic_clock.hpp"
#include <gtest/gtest.h>

constexpr auto universe_radius = 100;

template <std::size_t N, std::floating_point F>
auto generate_particle_set(std::size_t size)
{
    using namespace pm::factory;
    using namespace utility::random_distributions;

    auto mass_generator = []() mutable -> F {
        using distribution_t = random_distribution<F, DistributionCategory::Exponential>;
        using param_type     = typename distribution_t::param_type;
        const param_type      params(0.001);
        static distribution_t d(params);
        return d() * F{ 100 };
    };

    auto position_generator = []() mutable -> F {
        using distribution_a_t = random_distribution<F, DistributionCategory::Uniform>;
        using param_type_a     = typename distribution_a_t::param_type;
        const param_type_a      params_a(-universe_radius, universe_radius);
        static distribution_a_t d_a(params_a);
        using distribution_b_t = random_distribution<F, DistributionCategory::Gamma>;
        using param_type_b     = typename distribution_b_t::param_type;
        const param_type_b      params_b(F{ 1 }, F{ 1 });
        static distribution_b_t d_b(params_b);
        return d_a(); // + universe_radius;
    };

    auto velocity_generator = []() -> F { return F{ 0 }; };

    return particle_set_factory<N, F>(
        size, mass_generator, position_generator, velocity_generator
    );
}

TEST(SimulationTest, TreeAndBruteForceComparison)
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 3;
    using particle_t        = particle::ndparticle<N, F>;

    const auto size      = 150;
    auto       particles = generate_particle_set<N, F>(size);

    using tick_t            = synchronization::tick_period<std::chrono::seconds, 1>;
    const auto duration     = std::chrono::seconds(200);
    const auto max_depth    = 7;
    const auto box_capacity = 3;
    const auto size_        = std::ranges::size(particles);

    simulation::bh_approx::barnes_hut_approximation<particle_t> barnes_simulation_engine(
        particles, duration, tick_t::period_duration, max_depth, box_capacity
    );
    simulation::bf::brute_force_computation<particle_t> brute_force_simulation_engine(
        particles, duration, tick_t::period_duration
    );

    barnes_simulation_engine.run();
    brute_force_simulation_engine.run();

    for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
    {
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(
                barnes_simulation_engine.velocity_read(p_idx)[i],
                brute_force_simulation_engine.velocity_read(p_idx)[i],
                F{ 0.0001 * universe_radius }
            );
        }
    }
    EXPECT_LE(
        barnes_simulation_engine.f_eval_count(),
        brute_force_simulation_engine.f_eval_count()
    );
}
