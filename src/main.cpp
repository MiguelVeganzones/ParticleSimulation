#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
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

constexpr auto universe_diameter = 50.f;

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

int ndtree_test()
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 3;
    using particle_t        = particle::ndparticle<N, F>;
    using high_frequency_tick_t =
        synchronization::tick_period<std::chrono::milliseconds, 400>;

    const auto size      = 10;
    auto       particles = generate_particle_set<N, F>(size);

    const auto  depth        = 2;
    const auto  box_capacity = 3;
    ndt::ndtree tree(std::span{ particles }, depth, box_capacity);

    solvers::yoshida4_solver<particle_t> solver(
        particles, high_frequency_tick_t::period_duration
    );

    std::cout << tree << '\n';
    for (auto i = 0; i != 300000; ++i)
    {
        solver.run();
        tree.reorganize();
        tree.cache_summary();
    }
    std::cout << tree << '\n';
    return EXIT_SUCCESS;
}

int particle_movement_visualization_debug()
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 3;
    static constexpr auto K = 3000000000000; // Iterations
    using particle_t        = particle::ndparticle<N, F>;

    using high_frequency_tick_t =
        synchronization::tick_period<std::chrono::milliseconds, 100>;

    const auto size      = 4;
    auto       particles = generate_particle_set<N, F>(size);

    std::cout << "<-------------- Simulation -------------->\n";

    const auto initial_limtis = ndt::detail::compute_limits(particles);
    std::cout << initial_limtis << '\n';

    std::cout << "Particle Limits:\n";
    utility::timing::stopwatch s{ "Simulation" };

    std::cout << "Here0:\n";
    TApplication app = TApplication("Root app", 0, nullptr);

    std::cout << "Here2:\n";
    root_plotting::time_plotter4 plotter;

    std::cout << "Here3:\n";

    /*
    solvers::runge_kutta_solver<4, particle_t> solver(
        particles, high_frequency_tick_t::period_duration
    );
    */
    solvers::yoshida4_solver<particle_t> solver(
        particles, high_frequency_tick_t::period_duration
    );
    /*
    solvers::leapfrog_solver<2, particle_t> solver(
        particles,
        high_frequency_tick_t::period_duration,
        high_frequency_tick_t::period_duration
    );
    */
    std::cout << "Here4:\n";

    for (auto i = 0uz; i != K; ++i)
    {
        solver.run();
        if (i % 10000 == 0)
        {
            std::cout << i << '\n';
            const auto current_limtis = ndt::detail::compute_limits(particles);
            std::cout << current_limtis << '\n';
            plotter.append(
                static_cast<float>(particles[0].position().value()[0]),
                static_cast<float>(particles[1].position().value()[0]),
                static_cast<float>(particles[2].position().value()[0]),
                static_cast<float>(particles[3].position().value()[0])
            );
        }
    }

    for (auto const& p : particles | std::views::take(10))
    {
        std::cout << p << '\n';
    }

    const auto final_limtis = ndt::detail::compute_limits(particles);
    std::cout << final_limtis << '\n';
    std::cout << "<\\-------------- Simulation -------------->\n";

    app.Run();

    return EXIT_SUCCESS;
}

int particle_movement_visualization_test()
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 3;
    static constexpr auto K = 3000000000000; // Iterations
    using particle_t        = particle::ndparticle<N, F>;

    using low_frequency_tick_t = synchronization::tick_period<std::chrono::seconds, 1>;
    using high_frequency_tick_t =
        synchronization::tick_period<std::chrono::microseconds, 20>;

    const auto size      = 50;
    auto       particles = generate_particle_set<N, F>(size);

    std::cout << "<-------------- Simulation -------------->\n";

    std::cout << "Particle Limits:\n";
    const auto initial_limtis = ndt::detail::compute_limits(particles);
    std::cout << initial_limtis << '\n';

    utility::timing::stopwatch s{ "Simulation" };

    TApplication app = TApplication("Root app", 0, nullptr);

    root_plotting::scatter_plot_3D scatter_plot;

    solvers::leapfrog_solver<13, particle_t> solver(
        particles,
        high_frequency_tick_t::period_duration,
        low_frequency_tick_t::period_duration

    );

    std::array<float, size> x;
    std::array<float, size> y;
    std::array<float, size> z;

    for (auto i = 0uz; i != K; ++i)
    {
        solver.run();
        if (i % 100 == 0)
        {
            std::cout << i << '\n';
            const auto current_limtis = ndt::detail::compute_limits(particles);
            std::cout << current_limtis << '\n';

            for (int j = 0; j != size; ++j)
            {
                x[j] = static_cast<float>(particles[j].position()[0]);
                y[j] = static_cast<float>(particles[j].position()[1]);
                z[j] = static_cast<float>(particles[j].position()[2]);
            }
            scatter_plot.plot(size, &x[0], &y[0], &z[0]);
        }
    }

    for (auto const& p : particles | std::views::take(10))
    {
        std::cout << p << '\n';
    }

    const auto final_limtis = ndt::detail::compute_limits(particles);
    std::cout << final_limtis << '\n';
    std::cout << "<\\-------------- Simulation -------------->\n";

    app.Run();

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

    // particle_movement_visualization_debug();
    // particle_movement_visualization_test();
    ndtree_test();
    return EXIT_SUCCESS;
}
