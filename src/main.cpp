#include "particle.hpp"
#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include "random.hpp"
#include "random_distributions.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

template <pm::concepts::Particle P>
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
        pm::magnitudes::mass<F>{ utility::random::srandom::randfloat<F>() * F{ 20 } },
        pm::magnitudes::linear_velocity<N, F>{
            F{ 100.f } * -utility::random::srandom::randfloat<F>(),
            -utility::random::srandom::randfloat<F>(),
            -utility::random::srandom::randfloat<F>() },

        pm::magnitudes::linear_acceleration<N, F>{
            utility::random::srandom::randfloat<F>(),
            utility::random::srandom::randfloat<F>() / F{ 10 },
            utility::random::srandom::randfloat<F>() }
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
    ndtree<sample_t> tree(std::span{ samples }, 5, 4);
    std::cout << tree;

    return EXIT_SUCCESS;
}

int gravitational_interaction_test()
{
    using namespace pm;

    using F                 = double;
    static constexpr auto N = 3;
    using sample_t          = particle::ndparticle<N, F>;

    const auto            size = 3;
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

    for (auto const& p1 : samples)
    {
        for (auto const& p2 : samples)
        {
            std::cout << pm::interaction::gravitational_interaction(p1, p2) << '\n';
        }
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
}
