#include "particle.hpp"
#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include "random.hpp"
#include "random_distributions.hpp"
#include "synthetic_clock.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

template <pm::particle_concepts::Particle P>
auto generate_particle() noexcept -> P
{
    using namespace pm::magnitudes;
    constexpr auto N = P::s_dimension;
    using F          = typename P::value_type;
    return P(
        pm::magnitudes::position<N, F>{ utility::random::srandom::randfloat<F>() * F{ 3 },
                                        utility::random::srandom::randfloat<F>() * F{ 3 },
                                        utility::random::srandom::randfloat<F>() *
                                            F{ 3 } },
        // operator*(mass, F)
        pm::magnitudes::mass<F>{ utility::random::srandom::randfloat<F>() * F{ 200 } },
        pm::magnitudes::linear_velocity<N, F>{
            F{ 100 } * -utility::random::srandom::randfloat<F>(),
            -utility::random::srandom::randfloat<F>(),
            -utility::random::srandom::randfloat<F>() } *
            F{ 0 },

        pm::magnitudes::linear_acceleration<N, F>{
            utility::random::srandom::randfloat<F>(),
            utility::random::srandom::randfloat<F>() / F{ 10 },
            utility::random::srandom::randfloat<F>() } *
            F{ 0 }
    );
}

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
        samples.push_back(generate_particle<sample_t>());
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

    const auto            size = 3000;
    std::vector<sample_t> samples;

    for ([[maybe_unused]]
         auto _ : std::views::iota(0, size))
    {
        samples.push_back(generate_particle<sample_t>());
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

    using tick_t = synchronization::tick_period<std::chrono::milliseconds, 200>;
    using simulation_clock_t = synchronization::synthetic_clock<tick_t>;

    const auto            size = 30000;
    std::vector<sample_t> samples;

    for ([[maybe_unused]]
         auto _ : std::views::iota(0, size))
    {
        samples.push_back(generate_particle<sample_t>());
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
}
