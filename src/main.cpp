#include "particle.hpp"
#include "random.hpp"
#include "random_distributions.hpp"
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
    using F                 = float;
    static constexpr auto N = 3;
    using sample_t          = particle::ndparticle<N, float>;

    const auto            size = 30;
    std::vector<sample_t> samples;

    for ([[maybe_unused]]
         auto _ : std::views::iota(0, size))
    {
        samples.emplace_back(
            pm::position<N, F>{ utility::random::srandom::randfloat<F>(),
                                utility::random::srandom::randfloat<F>(),
                                utility::random::srandom::randfloat<F>() } *
                F{ 3 },
            // operator*(mass, F)
            pm::mass<F>{ utility::random::srandom::randfloat<F>() * F{ 20 } },
            pm::linear_velocity<N, F>{ F{ 100.f } *
                                           -utility::random::srandom::randfloat<F>(),
                                       -utility::random::srandom::randfloat<F>(),
                                       -utility::random::srandom::randfloat<F>() },

            pm::linear_acceleration<N, F>{ utility::random::srandom::randfloat<F>(),
                                           utility::random::srandom::randfloat<F>() /
                                               F{ 10 },
                                           utility::random::srandom::randfloat<F>() }
        );
    }

    for (auto i = 0uz; auto const& s : samples)
    {
        std::cout << "Sample: " << i++ << '\n' << s << '\n';
    }
    ndtree<sample_t> tree(std::span{ samples }, 5, 4);
    std::cout << tree;

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
}
