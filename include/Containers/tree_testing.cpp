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
    using F          = float;
    constexpr auto N = 3;

    using namespace utility::random;

    const ndpoint<N, F> p1 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<N, F> p2 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<N, F> p3 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<N, F> p4 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };

    ndsample<N, F, F> s1{ p1, srandom::randfloat<F>() };
    ndsample<N, F, F> s2{ p2, srandom::randfloat<F>() };
    ndsample<N, F, F> s3{ p3, srandom::randfloat<F>() };
    ndsample<N, F, F> s4{ p4, srandom::randfloat<F>() };
    ndsample<N, F, F> s5{ p3, srandom::randfloat<F>() };
    ndsample<N, F, F> s6{ p2, srandom::randfloat<F>() };
    auto              samples = std::vector{ s1, s2, s3, s4, s5, s6 };

    ndtree<N, F, F> tree(std::span{ samples }, 3, 1);
    std::cout << tree;

    return EXIT_SUCCESS;
}

int debug()
{
    using namespace ndt;
    using F          = float;
    constexpr auto N = 4;
    constexpr auto K = 1000;

    using namespace utility::random;
    using namespace utility::random_distributions;
    using distribution_t = random_distribution<float, DistributionCategory::Exponential>;
    using param_type     = typename distribution_t::param_type;
    param_type     params(0.25f);
    distribution_t d(params);

    const ndpoint<N, F> d1 = { 10, 10, 10, 10 };
    const ndpoint<N, F> d2 = { -10, -10, -10, -10 };

    std::vector<ndsample<N, F, F>> samples;
    samples.reserve(K);
    for (auto i = 0; i != K; ++i)
    {
        samples.push_back(ndsample<N, F, F>(ndpoint<N, F>{ d(), d(), d(), d() }, d()));
    }

    const ndpoint<N, F> p1 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };

    const ndpoint<N, F> p2 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<N, F> p3 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<N, F> p4 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };


    std::cout << p1 << "\n";
    std::cout << p2 << "\n";

    std::cout << detail::compute_limits(std::array{ p1, p2, p3, p4 }).min() << "\n";
    std::cout << detail::compute_limits(std::array{ p1, p2, p3, p4 }).max() << "\n";
    std::cout << detail::in(p1, { p2, p3 });
    std::cout << detail::count_in(std::array{ p1, p1, p4, p3 }, ndboundary{ d1, d2 })
              << '\n';

    ndtree<N, F, F> tree(std::span{ samples }, 10, 3, ndboundary<N, F>{ d2, d1 });
    std::cout << tree;

    return EXIT_SUCCESS;
}

int main()
{
    debug();
    test();
}
