#include "barnes_hut_approximation.hpp"
#include "brute_force.hpp"
#ifdef USE_ROOT_PLOTTING
#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#endif
#include "factory.hpp"
#include "leapfrog.hpp"
#include "logging.hpp"
#include "ndtree.hpp"
#include "particle.hpp"
#include "physical_magnitudes.hpp"
#include "plotting.hpp"
#include "random_distributions.hpp"
#include "runge_kutta.hpp"
#include "scatter_plot.hpp"
#include "stopwatch.hpp"
#include "synthetic_clock.hpp"
#include "time_plotter.hpp"
#include "utils.hpp"
#include "yoshida.hpp"
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

constexpr auto universe_diameter = 200.f;

template <std::floating_point F>
auto generate_particle_pair()
{
    const F m = 1e8;
    return std::array{ pm::particle::ndparticle<1, F>(
                           pm::magnitudes::mass<F>{ m },
                           pm::magnitudes::position<1, F>{ -universe_diameter },
                           pm::magnitudes::linear_velocity<1, F>{ 0 },
                           pm::magnitudes::linear_acceleration<1, F>{ 0 }
                       ),
                       pm::particle::ndparticle<1, F>(
                           pm::magnitudes::mass<F>{ m },
                           pm::magnitudes::position<1, F>{ universe_diameter },
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
        param_type            params(F{ 0.001f });
        static distribution_t d(params);
        return d() * F{ 100 };
    };

    auto position_generator = []() mutable -> F {
        using distribution_a_t = random_distribution<F, DistributionCategory::Uniform>;
        using param_type_a     = typename distribution_a_t::param_type;
        param_type_a            params_a(F{ -universe_diameter }, F{ universe_diameter });
        static distribution_a_t d_a(params_a);
        using distribution_b_t = random_distribution<F, DistributionCategory::Gamma>;
        using param_type_b     = typename distribution_b_t::param_type;
        param_type_b            params_b(F{ 1 }, F{ 1 });
        static distribution_b_t d_b(params_b);
        return d_a(); // + universe_diameter;
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
    using tick_t            = synchronization::tick_period<std::chrono::seconds, 1>;

    const auto size         = 10000;
    auto       particles    = generate_particle_set<N, F>(size);
    const auto duration     = std::chrono::seconds(100);
    const auto max_depth    = 7;
    const auto box_capacity = 3;

    simulation::bh_appox::barnes_hut_approximation<particle_t> simulation_a(
        particles, duration, tick_t::period_duration, max_depth, box_capacity
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
    utility::logging::default_source::log(
        utility::logging::severity_level::info, "Inside main function."
    );
    utility::logging::default_source::log(
        utility::logging::severity_level::error, "Huge error or sth..."
    );

    barnes_hut_test();
    return EXIT_SUCCESS;
}
