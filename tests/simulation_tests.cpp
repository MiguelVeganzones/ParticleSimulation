#include <chrono>
#undef USE_BOOST_LOGGING
#undef USE_ROOT_PLOTTING
#undef DEBUG_NDTREE

#include "barnes_hut_approximation.hpp"
#include "brute_force.hpp"
#include "factory.hpp"
#include "particle.hpp"
#include "particle_factory.hpp"
#include "synthetic_clock.hpp"
#include <gtest/gtest.h>

constexpr auto universe_radius = 100;

TEST(SimulationTest, TreeAndBruteForceComparison)
{
    using namespace pm;
    using F                    = double;
    static constexpr auto N    = 3;
    using particle_t           = particle::ndparticle<N, F>;
    constexpr auto interaction = pm::interaction::InteractionType::Gravitational;

#ifndef NDEBUG
    const auto size     = 150;
    const auto duration = std::chrono::seconds(150);
    using tick_t        = synchronization::tick_period<std::chrono::seconds, 1>;
    constexpr auto f    = 0.95;
#else
    const auto size     = 300;
    const auto duration = std::chrono::seconds(1000);
    using tick_t        = synchronization::tick_period<std::chrono::milliseconds, 200>;
    constexpr auto f    = 0.85;
#endif
    auto particles = particle_factory::generate_particle_set<N, F>(size, universe_radius);

    const auto max_depth    = 7;
    const auto box_capacity = 3;
    const auto theta        = F{ 0.4 };

    simulation::bh_approx::barnes_hut_approximation<particle_t, interaction>
        barnes_simulation_engine(
            particles, duration, tick_t::period_duration, theta, max_depth, box_capacity
        );
    simulation::bf::brute_force_computation<particle_t, interaction>
        brute_force_simulation_engine(particles, duration, tick_t::period_duration);

    barnes_simulation_engine.run();
    brute_force_simulation_engine.run();

    for (std::size_t p_idx = 0; p_idx != size; ++p_idx)
    {
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_NEAR(
                barnes_simulation_engine.velocity_read(p_idx)[i],
                brute_force_simulation_engine.velocity_read(p_idx)[i],
                F{ 1e-7 * universe_radius }
            );
        }
    }
    EXPECT_LE(
        barnes_simulation_engine.f_eval_count(),
        static_cast<std::size_t>(
            static_cast<double>(brute_force_simulation_engine.f_eval_count()) * f
        )
    );
}
