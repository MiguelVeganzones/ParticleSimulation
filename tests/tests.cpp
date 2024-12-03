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

    auto particles_a = std::vector{ particle::ndparticle<N, F>{
                                        pm::magnitudes::mass<F>{ 10 },
                                        pm::magnitudes::position<N, F>{ -100 },
                                        pm::magnitudes::linear_velocity<N, F>{ 0 } },
                                    particle::ndparticle<N, F>{
                                        pm::magnitudes::mass<F>{ 10 },
                                        pm::magnitudes::position<N, F>{ 100 },
                                        pm::magnitudes::linear_velocity<N, F>{ 0 },
                                    } };
    auto particles_b = std::vector{ particle::ndparticle<N, F>{
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
    const auto size_        = std::ranges::size(particles_a);
    using mass_t            = typename particle_t::mass_t;
    using position_t        = typename particle_t::position_t;

    simulation::bh_approx::barnes_hut_approximation<particle_t> simulation_a(
        particles_a, duration, duration, max_depth, box_capacity
    );
    simulation_a.run();
    simulation::bf::brute_force_computation<particle_t> simulation_b(
        particles_b, duration, duration
    );

    simulation_b.run();
    for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
    {
        EXPECT_EQ(particles_a[p_idx].velocity(), particles_b[p_idx].velocity());
    }
}
