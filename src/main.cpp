#include "factory.hpp"
#include "logging.hpp"
#include "ndtree.hpp"
#include "particle.hpp"
#include "particle_interaction.hpp"
#include "random_distributions.hpp"
#include "solar_system.hpp"
#include "stopwatch.hpp"
#include "synthetic_clock.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

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
        return d();
    };

    auto position_generator = []() mutable -> F {
        using distribution_a_t = random_distribution<F, DistributionCategory::Uniform>;
        using param_type_a     = typename distribution_a_t::param_type;
        param_type_a            params_a(F{ -100 }, F{ 100 });
        static distribution_a_t d_a(params_a);
        using distribution_b_t = random_distribution<F, DistributionCategory::Gamma>;
        using param_type_b     = typename distribution_b_t::param_type;
        param_type_b            params_b(F{ 8 }, F{ 1 });
        static distribution_b_t d_b(params_b);
        return d_a() + d_b();
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
    auto       particles = create_solar_system();
    auto       particles = generate_particle_set<N, F>(size);


    std::cout << "<-------------- Simulation -------------->\n";

    const auto initial_limtis = ndt::detail::compute_limits(particles);
    std::cout << initial_limtis << '\n';

    std::cout << "Particle Limits:\n";

    for (auto const& p : particles | std::views::take(10))
    {
        std::cout << p << '\n';
    }

    utility::timing::stopwatch s{ "Simulation" };
    for (auto i = 0uz; i != K; ++i)
    {
        if (i % 1 == 0)
        {
            for (auto const& p : particles | std::views::take(10))
            {
                std::cout << p << '\n';
            }
            std::cout << "Iteration: " << i << '\n';
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
    particle_movement_simulation();
    return EXIT_SUCCESS;
}
