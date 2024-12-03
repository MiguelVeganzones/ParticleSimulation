#include "barnes_hut_approximation.hpp"
#include "brute_force.hpp"
#include "concepts.hpp"
#include "particle.hpp"
#include "physical_vector.hpp"
#include "synthetic_clock.hpp"
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(SimulationTest, TreeAndBruteForceComparison)
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 3;
    using particle_t        = particle::ndparticle<N, F>;

    auto particles = std::vector{ particle::ndparticle<N, F>{
                                      pm::magnitudes::mass<F>{ 10 },
                                      pm::magnitudes::position<N, F>{ -100 },
                                      pm::magnitudes::linear_velocity<N, F>{ 0 } },
                                  particle::ndparticle<N, F>{
                                      pm::magnitudes::mass<F>{ 10 },
                                      pm::magnitudes::position<N, F>{ 100 },
                                      pm::magnitudes::linear_velocity<N, F>{ 0 },
                                  } };

    const auto duration     = std::chrono::seconds(1);
    const auto max_depth    = 7;
    const auto box_capacity = 3;
    const auto size_        = std::ranges::size(particles);

    simulation::bh_approx::barnes_hut_approximation<particle_t> simulation_a(
        particles, duration, duration, max_depth, box_capacity
    );
    simulation_a.run();
    simulation::bf::brute_force_computation<particle_t> simulation_b(
        particles, duration, duration
    );

    simulation_b.run();
    for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
    {
        EXPECT_EQ(simulation_a.velocity_read(p_idx), simulation_b.velocity_read(p_idx));
    }
}
