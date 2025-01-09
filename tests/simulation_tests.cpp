#include <chrono>
#undef USE_BOOST_LOGGING
#undef USE_ROOT_PLOTTING
#undef DEBUG_NDTREE

#include "barnes_hut_approximation.hpp"
#include "brute_force.hpp"
#include "factory.hpp"
#include "particle.hpp"
#include "particle_factory.hpp"
#include "simulation_config.hpp"
#include "synthetic_clock.hpp"
#include <chrono>
#include <gtest/gtest.h>

constexpr auto universe_radius = 100;

TEST(SimulationTest, TreeAndBruteForceComparisonReturnsSimilarResults)
{
    using namespace pm;
    using F                    = double;
    static constexpr auto N    = 3;
    using particle_t           = particle::ndparticle<N, F>;
    constexpr auto interaction = pm::interaction::InteractionType::Gravitational;

#ifndef NDEBUG
    simulation::config::simulation_common_config<particle_t> base_config{
        .dt_             = std::chrono::seconds(1),
        .duration_       = std::chrono::seconds(150),
        .particle_count_ = 150,
        .sim_type_       = simulation::config::SimulationType::_none_
    };
    constexpr auto f = 0.95;
#else
    simulation::config::simulation_common_config<particle_t> base_config{
        .dt_             = std::chrono::milliseconds(200),
        .duration_       = std::chrono::seconds(1000),
        .particle_count_ = 300,
        .sim_type_       = simulation::config::SimulationType::_none_
    };
    constexpr auto f = 0.85;
#endif
    const auto size = base_config.particle_count_;
    auto particles = particle_factory::generate_particle_set<N, F>(size, universe_radius);

    simulation::config::barnes_hut_specific_config<particle_t> bh_config{
        .tree_max_depth_ = 7, .tree_box_capacity_ = 3, .theta_ = F{ 0.4 }
    };

    simulation::bh_approx::barnes_hut_approximation<particle_t, interaction>
        barnes_simulation_engine(particles, base_config, bh_config);
    simulation::bf::brute_force_computation<particle_t, interaction>
        brute_force_simulation_engine(particles, base_config);

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

TEST(SimulationTest, TreeAndBruteForceComparisonReturnsTheSameResult)
{
    GTEST_SKIP();
    using namespace pm;
    using F                    = double;
    static constexpr auto N    = 3;
    using particle_t           = particle::ndparticle<N, F>;
    constexpr auto interaction = pm::interaction::InteractionType::Gravitational;

#ifndef NDEBUG
    simulation::config::simulation_common_config<particle_t> base_config{
        .dt_             = std::chrono::seconds(1),
        .duration_       = std::chrono::seconds(150),
        .particle_count_ = 150,
        .sim_type_       = simulation::config::SimulationType::_none_
    };
    constexpr auto f = 0.95;
#else
    simulation::config::simulation_common_config<particle_t> base_config{
        .dt_             = std::chrono::milliseconds(200),
        .duration_       = std::chrono::seconds(1000),
        .particle_count_ = 300,
        .sim_type_       = simulation::config::SimulationType::_none_
    };
    constexpr auto f = 0.85;
#endif
    const auto size = base_config.particle_count_;
    auto       particles =
        particle_factory::generate_particle_set<N, F>(size, universe_radius / 2);

    simulation::config::barnes_hut_specific_config<particle_t> bh_config{
        .tree_max_depth_ = 7, .tree_box_capacity_ = 3, .theta_ = F{ 0.0 }
    };

    simulation::bh_approx::barnes_hut_approximation<particle_t, interaction>
        barnes_simulation_engine(particles, base_config, bh_config);
    simulation::bf::brute_force_computation<particle_t, interaction>
        brute_force_simulation_engine(particles, base_config);

    barnes_simulation_engine.run();
    brute_force_simulation_engine.run();

    for (std::size_t p_idx = 0; p_idx != size; ++p_idx)
    {
        for (std::size_t i = 0; i != N; ++i)
        {
            EXPECT_FLOAT_EQ(
                barnes_simulation_engine.velocity_read(p_idx)[i],
                brute_force_simulation_engine.velocity_read(p_idx)[i]
            );
        }
    }
    EXPECT_GE(
        barnes_simulation_engine.f_eval_count(),
        static_cast<std::size_t>(
            static_cast<double>(brute_force_simulation_engine.f_eval_count()) * f
        )
    );
}
