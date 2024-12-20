#include "barnes_hut_approximation.hpp"
#include "brute_force.hpp"
#include "factory.hpp"
#include "logging.hpp"
#include "particle.hpp"
#include "random_distributions.hpp"
#include "synthetic_clock.hpp"
#include <array>
#include <concepts>
#include <cstdlib>
#include <iostream>
#include <vector>

constexpr auto universe_radius = 10.0;

template <std::floating_point F>
auto generate_particle_pair()
{
    const F m = 1e8;
    return std::array{ pm::particle::ndparticle<1, F>(
                           pm::magnitudes::mass<F>{ m },
                           pm::magnitudes::position<1, F>{ -universe_radius },
                           pm::magnitudes::linear_velocity<1, F>{ 0 },
                           pm::magnitudes::linear_acceleration<1, F>{ 0 }
                       ),
                       pm::particle::ndparticle<1, F>(
                           pm::magnitudes::mass<F>{ m },
                           pm::magnitudes::position<1, F>{ universe_radius },
                           pm::magnitudes::linear_velocity<1, F>{ 0 },
                           pm::magnitudes::linear_acceleration<1, F>{ 0 }
                       ) };
}

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

int barnes_hut_test()
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 3;
    using particle_t        = particle::ndparticle<N, F>;
    using tick_t = synchronization::tick_period<std::chrono::milliseconds, 100>;

    const auto size         = 20;
    auto       particles    = generate_particle_set<N, F>(size);
    const auto duration     = std::chrono::seconds(100000);
    const auto max_depth    = 7;
    const auto box_capacity = 3;
    const auto theta        = F{ 0.4 };

    simulation::bh_approx::barnes_hut_approximation<particle_t> simulation_a(
        particles, duration, tick_t::period_duration, theta, max_depth, box_capacity
    );

    std::cout << "Simulation A\n";
    simulation_a.run();

    simulation::bf::brute_force_computation<particle_t> simulation_b(
        particles, duration, tick_t::period_duration
    );

    std::cout << "Simulation B\n";
    simulation_b.run();

    return EXIT_SUCCESS;
}

int main()
{
#ifdef USE_ROOT_PLOTTING
    TApplication app = TApplication("Root app", 0, nullptr);
#endif
    utility::logging::default_source::log(
        utility::logging::severity_level::info, "Inside main function."
    );
    utility::logging::default_source::log(
        utility::logging::severity_level::error, "Huge error or sth..."
    );

    barnes_hut_test();

#ifdef USE_ROOT_PLOTTING
    app.Run();
#endif

    return EXIT_SUCCESS;
}
