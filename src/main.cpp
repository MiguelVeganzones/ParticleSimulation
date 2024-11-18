#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "factory.hpp"
#include "logging.hpp"
#include "ndtree.hpp"
#include "particle.hpp"
#include "particle_interaction.hpp"
#include "particle_systems.hpp"
#include "plotting.hpp"
#include "random_distributions.hpp"
#include "stopwatch.hpp"
#include "synthetic_clock.hpp"
#include "time_plotter.hpp"
#include "utils.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>

constexpr auto universe_diameter = 0.01f;

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

    auto acceleration_generator = []() -> F { return F{ 0 }; };

    return particle_set_factory<N, F>(
        size,
        mass_generator,
        position_generator,
        velocity_generator,
        acceleration_generator
    );
}

int ndtree_test()
{
    using namespace ndt;
    using namespace pm;
    using F                 = float;
    static constexpr auto N = 3;
    using sample_t          = particle::ndparticle<N, F>;

    const auto size    = 30;
    auto       samples = generate_particle_set<N, F>(size);

    for (auto i = 0uz; auto const& s : samples)
    {
        std::cout << "Sample: " << i++ << '\n' << s << '\n';
    }
    ndtree<sample_t> tree(std::span<sample_t, size>{ samples }, 5, 4uz);
    std::cout << tree;

    return EXIT_SUCCESS;
}

int gravitational_interaction_test()
{
    using namespace pm;

    using F                 = double;
    static constexpr auto N = 3;

    const auto size    = 30;
    const auto samples = generate_particle_set<N, F>(size);

    for (auto i = 0uz; auto const& s : samples)
    {
        std::cout << "Sample: " << i++ << '\n' << s << '\n';
    }

    for (auto const& p1 : samples | std::views::take(10))
    {
        for (auto const& p2 : samples | std::views::take(10))
        {
            std::cout << pm::interaction::gravitational_interaction(p1, p2) << '\n';
        }
    }

    return EXIT_SUCCESS;
}

int particle_movement_test()
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 3;
    using sample_t          = particle::ndparticle<N, F>;

    using tick_t             = synchronization::tick_period<std::chrono::seconds, 200>;
    using simulation_clock_t = synchronization::synthetic_clock<tick_t>;

    const auto size    = 5;
    auto       samples = generate_particle_set<N, F>(size);

    std::cout << "Start of particle 0: " << samples[0] << '\n';

    for (auto i = 0uz; i != 10; ++i)
    {
        pm::interaction::update_acceleration(
            samples[0], std::span<sample_t, size>{ samples }
        );
        samples[0].update_position(tick_t::period_duration);

        std::cout << i << "\tPosition of particle 0: " << samples[0].position() << '\n';

        if constexpr (utility::concepts::is_manual_tick_clock_v<simulation_clock_t>)
        {
            simulation_clock_t::tick();
        }
    }

    std::cout << "End of particle 0: " << samples[0] << '\n';

    return EXIT_SUCCESS;
}

int solar_system_test()
{
    using namespace pm;
    using F = double;

    auto particles = pm::particle_systems::create_solar_system<F>();

    for (auto const& p : particles)
    {
        std::cout << p << '\n';
    }

    return EXIT_SUCCESS;
}

int particle_movement_simulation()
{
    using namespace pm;

    using F                 = double;
    static constexpr auto N = 2;
    static constexpr auto K = 3000000; // Iterations
    using particle_t        = particle::ndparticle<N, F>;

    using tick_t = synchronization::tick_period<std::chrono::milliseconds, 10000>;
    using simulation_clock_t = synchronization::synthetic_clock<tick_t>;

    const auto size      = 3;
    auto       particles = generate_particle_set<N, F>(size);


    std::cout << "<-------------- Simulation -------------->\n";

    const auto initial_limtis = ndt::detail::compute_limits(particles);
    std::cout << initial_limtis << '\n';

    std::cout << "Particle Limits:\n";
    utility::timing::stopwatch s{ "Simulation" };
    for (auto i = 0uz; i != K; ++i)
    {
        if (i % 10000 == 0)
        {
            std::cout << "Iteration: " << i << '\n';
            for (auto const& p : particles | std::views::take(10))
            {
                std::cout << p << '\n';
            }
            const auto current_limtis = ndt::detail::compute_limits(particles);
            std::cout << current_limtis << '\n';
            if (current_limtis.min()[0] < initial_limtis.min()[0] - 0.001)
            {
                std::cout << "END" << std::endl;
                std::terminate();
            }
        }
        for (auto& p : particles)
        {
            pm::interaction::update_acceleration(
                p, std::span<particle_t, size>{ particles }
            );
        }
        for (auto& p : particles)
        {
            p.update_position(tick_t::period_duration);
        }

        if constexpr (utility::concepts::is_manual_tick_clock_v<simulation_clock_t>)
        {
            simulation_clock_t::tick();
        }
    }

    for (auto const& p : particles | std::views::take(10))
    {
        std::cout << p << '\n';
    }

    const auto final_limtis = ndt::detail::compute_limits(particles);
    std::cout << final_limtis << '\n';
    std::cout << "<\\-------------- Simulation -------------->\n";

    return EXIT_SUCCESS;
}

int particle_movement_visualization_debug()
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 1;
    static constexpr auto K = 3000000000000; // Iterations
    using particle_t        = particle::ndparticle<N, F>;

    using high_freq_tick_t = synchronization::tick_period<std::chrono::nanoseconds, 100>;
    using low_freq_tick_t  = synchronization::tick_period<std::chrono::microseconds, 100>;

    const auto size      = 2;
    auto       particles = generate_particle_set<N, F>(size);

    std::cout << "<-------------- Simulation -------------->\n";

    const auto initial_limtis = ndt::detail::compute_limits(particles);
    std::cout << initial_limtis << '\n';

    std::cout << "Particle Limits:\n";
    utility::timing::stopwatch s{ "Simulation" };

    std::vector<float> x{};
    std::vector<float> y1{};
    std::vector<float> y2{};

    TApplication app = TApplication("Root app", 0, nullptr);

    root_plotting::time_plotter plotter;

    bool    flag  = false;
    const F delta = 0.00001;
    for (auto i = 0uz; i != K; ++i)
    {
        if (flag)
        {
            const auto current_limtis = ndt::detail::compute_limits(particles);
            std::cout << current_limtis << '\n';
            const auto [_, d] = utils::normalize(
                current_limtis.min().value() - current_limtis.max().value()
            );
            x.push_back(static_cast<float>(i));
            y1.push_back(static_cast<float>(particles[0].position().value()[0]));
            y2.push_back(static_cast<float>(particles[1].position().value()[0]));
            plotter.plot((int)y1.size(), &y1[0], &y2[0]);
            std::cout << "D: " << d << '\n';
            if (d >= delta)
            {
                std::cout << "END" << std::endl;
                break;
            }
        }
        else if (i % 1000000)
        {
            const auto current_limtis = ndt::detail::compute_limits(particles);
            std::cout << current_limtis << '\n';
            const auto [_, d] = utils::normalize(
                current_limtis.min().value() - current_limtis.max().value()
            );
            if (d < delta)
            {
                flag = true;
            }
        }

        for (auto& p : particles)
        {
            pm::interaction::update_acceleration(
                p, std::span<particle_t, size>{ particles }
            );
        }
        for (auto& p : particles)
        {
            p.update_position(
                flag ? high_freq_tick_t::period_duration
                     : low_freq_tick_t::period_duration
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

int particle_movement_visualization()
{
    using namespace pm;
    using F                 = double;
    static constexpr auto N = 1;
    static constexpr auto K = 3000000000000; // Iterations
    using particle_t        = particle::ndparticle<N, F>;

    using tick_t = synchronization::tick_period<std::chrono::milliseconds, 1>;

    const auto size      = 2;
    auto       particles = generate_particle_set<N, F>(size);

    std::cout << "<-------------- Simulation -------------->\n";

    const auto initial_limtis = ndt::detail::compute_limits(particles);
    std::cout << initial_limtis << '\n';

    std::cout << "Particle Limits:\n";
    utility::timing::stopwatch s{ "Simulation" };

    std::vector<float> x{};
    std::vector<float> y1{};
    std::vector<float> y2{};

    TApplication app = TApplication("Root app", 0, nullptr);

    root_plotting::time_plotter plotter;

    for (auto i = 0uz; i != K; ++i)
    {
        if (i % 1000000)
        {
            const auto current_limtis = ndt::detail::compute_limits(particles);
            std::cout << current_limtis << '\n';
            const auto [_, d] = utils::normalize(
                current_limtis.min().value() - current_limtis.max().value()
            );
            x.push_back(static_cast<float>(i));
            y1.push_back(static_cast<float>(particles[0].position().value()[0]));
            y2.push_back(static_cast<float>(particles[1].position().value()[0]));
            plotter.plot((int)y1.size(), &y1[0], &y2[0]);
            std::cout << "D: " << d << '\n';
        }
        for (auto& p : particles)
        {
            pm::interaction::update_acceleration(
                p, std::span<particle_t, size>{ particles }
            );
        }
        for (auto& p : particles)
        {
            p.update_position(tick_t::period_duration);
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
    utility::logging::init();
    utility::logging::default_source::log(
        utility::logging::severity_level::info, "Inside main function."
    );
    utility::logging::default_source::log(
        utility::logging::severity_level::error, "Huge error or sth..."
    );

    // ndtree_test();
    // gravitational_interaction_test();
    // particle_movement_test();
    // particle_movement_simulation();
    particle_movement_visualization();
    solar_system_test();
    return EXIT_SUCCESS;
}
