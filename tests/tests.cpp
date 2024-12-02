#include "barnes_hut_approximation.hpp"
#include "brute_force.hpp"
#include "concepts.hpp"
#include "particle.hpp"
#include "physical_vector.hpp"
#include "synthetic_clock.hpp"
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(ParticleTest, ParticleSizeTest)
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 3;
    using particle_t        = particle::ndparticle<N, F>;
    using tick_t            = synchronization::tick_period<std::chrono::seconds, 1>;

    auto particles = std::array{ particle::ndparticle<N, F>{
                                     pm::magnitudes::mass<F>{ 10 },
                                     pm::magnitudes::position<N, F>{ -100 },
                                     pm::magnitudes::linear_velocity<N, F>{ 0 } },
                                 particle::ndparticle<N, F>{
                                     pm::magnitudes::mass<F>{ 10 },
                                     pm::magnitudes::position<N, F>{ 100 },
                                     pm::magnitudes::linear_velocity<N, F>{ 0 },
                                 } };

    const auto duration     = std::chrono::seconds(100);
    const auto max_depth    = 7;
    const auto box_capacity = 3;

    simulation::bh_appox::barnes_hut_approximation<particle_t> simulation_a(
        particles, duration, tick_t::period_duration, max_depth, box_capacity
    );
    simulation_a.run();
    simulation::bf::brute_force_computation<particle_t> simulation_b(
        particles, duration, tick_t::period_duration
    );

    simulation_b.run();
    EXPECT_EQ(1, 1);
}