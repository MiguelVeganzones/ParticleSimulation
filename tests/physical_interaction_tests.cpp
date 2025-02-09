#include <iostream>
#include <limits>
#undef USE_BOOST_LOGGING
#undef USE_ROOT_PLOTTING
#include "particle.hpp"
#include "particle_factory.hpp"
#include "particle_interaction.hpp"
#include "physical_constants.hpp"
#include "random_distributions.hpp"
#include "utils.hpp"
#include <gtest/gtest.h>

constexpr auto universe_radius = 100;

TEST(PhysicalInteraction, GravitationalInteraction)
{
    using namespace pm;
    using F                    = double;
    static constexpr auto N    = 10;
    using particle_t           = particle::ndparticle<N, F>;
    using acceleration_t       = typename particle_t::acceleration_t;
    constexpr auto size        = 50uz;
    constexpr auto interaction = pm::interaction::InteractionType::Gravitational;

    auto rnd = [](auto scale) {
        using distribution_t = utility::random_distributions::random_distribution<
            F,
            utility::random_distributions::DistributionCategory::Exponential>;
        using param_type = typename distribution_t::param_type;
        static const param_type params(0.01);
        static distribution_t   d(params);
        return d() * F{ scale } + std::numeric_limits<F>::epsilon();
    };

    for (std::size_t k = 0; k != 100; ++k)
    {
        physical_parameters<F>::set_gravitational_constant(rnd(static_cast<F>(k)));
        const auto particles =
            particle_factory::generate_particle_set<N, F>(size, universe_radius);
        for (std::size_t i = 0; i != size; ++i)
        {
            for (std::size_t j = 0; j != size; ++j)
            {
                auto&& p1 = particles[i];
                auto&& p2 = particles[j];

                const auto distance = utils::distance(p1.position(), p2.position());
                const auto d        = utils::l2_norm(distance.value());
                const auto a        = i == j
                                          ? acceleration_t{}
                                          : physical_parameters<F>::G * p2.mass().magnitude() *
                                         distance / (d * d * d);
                const auto a_hat =
                    interaction::particle_interaction_t<particle_t, interaction>::
                        acceleration_contribution(particles[i], particles[j])

                    ;
                for (std::size_t dim = 0; dim != N; ++dim)
                {
                    if (i == j)
                    {
                        EXPECT_EQ(a_hat[dim], a[dim]);
                        EXPECT_EQ(a_hat[dim], F{ 0 });
                    }
                    else
                    {
                        EXPECT_NEAR(
                            std::abs((a_hat[dim] - a[dim]) / a[dim]), F{ 0 }, F{ 1e-3 }
                        );
                    }
                }
            }
        }
        physical_parameters<F>::reset();
    }
}
