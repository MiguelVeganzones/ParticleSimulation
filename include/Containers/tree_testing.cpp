#include "../PhysicalModel/particle.hpp"
#include "../Utility/random.hpp"
#include "../Utility/random_distributions.hpp"
#include "ndtree.hpp"
#include <iostream>
#include <span>
#include <tuple>
#include <vector>

int test()
{
    using namespace ndt;
    using F                 = float;
    static constexpr auto N = 3;
    using sample_t          = particle::ndparticle<N, float>;

    const auto            size = 500;
    std::vector<sample_t> samples;

    for (auto i : std::views::iota(0, size))
    {
        samples.emplace_back(
            pm::position<N, F>{ utility::random::srandom::randfloat<F>(),
                                utility::random::srandom::randfloat<F>(),
                                utility::random::srandom::randfloat<F>() },
            pm::mass<F>{ F{ 10 } * utility::random::srandom::randfloat<F>() },
            pm::linear_velocity<F>{ -utility::random::srandom::randfloat<F>() },
            pm::linear_acceleration<F>{ -utility::random::srandom::randfloat<F>() }
        );
    }

    ndtree<sample_t> tree(std::span{ samples }, 3, 1);
    std::cout << tree;

    return EXIT_SUCCESS;
}

int main()
{
    test();
}
