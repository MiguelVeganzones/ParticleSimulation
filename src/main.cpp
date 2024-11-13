#include "factory.hpp"
#include "particle.hpp"
#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include "random.hpp"
#include "random_distributions.hpp"
#include "stopwatch.hpp"
#include "synthetic_clock.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

int particle_test()
{
    using namespace ndt;
    using namespace pm;
    using F                 = float;
    static constexpr auto N = 3;
    using sample_t          = particle::ndparticle<N, F>;

    const auto            size = 30;
    std::vector<sample_t> samples;

    for ([[maybe_unused]]
         auto _ : std::views::iota(0, size))
    {
        samples.push_back(factory::particle_factory<N, F>([]() {
            return utility::random::srandom::randfloat<F>();
        }));
    }

    for (auto i = 0uz; auto const& s : samples)
    {
        std::cout << "Sample: " << i++ << '\n' << s << '\n';
    }
    ndtree<sample_t> tree(std::span{ samples }, 5, 4uz);
    std::cout << tree;

    return EXIT_SUCCESS;
}

int gravitational_interaction_test()
{
    using namespace pm;

    using F                 = double;
    static constexpr auto N = 3;
    using sample_t          = particle::ndparticle<N, F>;

    const auto            size = 30;
    std::vector<sample_t> samples;

    for ([[maybe_unused]]
         auto _ : std::views::iota(0, size))
    {
        samples.push_back(factory::particle_factory<N, F>([]() {
            return utility::random::srandom::randfloat<F>();
        }));
    }

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

    const auto            size = 5;
    std::vector<sample_t> samples;

    for ([[maybe_unused]]
         auto _ : std::views::iota(0, size))
    {
        samples.push_back(factory::particle_factory<N, F>([]() {
            return utility::random::srandom::randfloat<F>();
        }));
    }

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
    static constexpr auto N = 3;
    static constexpr auto K = 300; // Iterations
    using particle_t        = particle::ndparticle<N, F>;

    using tick_t             = synchronization::tick_period<std::chrono::seconds, 200>;
    using simulation_clock_t = synchronization::synthetic_clock<tick_t>;

    const auto              size = 50000;
    std::vector<particle_t> particles;
    particles.reserve(size);

    for ([[maybe_unused]]
         auto _ : std::views::iota(0, size))
    {
        particles.push_back(factory::particle_factory<N, F>([]() {
            return utility::random::srandom::randfloat<F>();
        }));
    }

    for (auto const& p : particles | std::views::take(10))
    {
        std::cout << p << '\n';
    }

    utility::timing::stopwatch s{ "Simulation" };
    for (auto i = 0uz; i != K; ++i)
    {
        if (i % 10 == 0)
        {
            std::cout << "Iteration: " << i << '\n';
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
    particle_test();
    gravitational_interaction_test();
    particle_movement_test();
    particle_movement_simulation();
}
