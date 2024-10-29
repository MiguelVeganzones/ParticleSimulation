#include "../Utility/random.hpp"
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

    const ndpoint<F, N> p1 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<F, N> p2 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<F, N> p3 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<F, N> p4 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };

    ndsample<F, F, N> s1{ p1, srandom::randfloat<F>() };
    ndsample<F, F, N> s2{ p2, srandom::randfloat<F>() };
    ndsample<F, F, N> s3{ p3, srandom::randfloat<F>() };
    ndsample<F, F, N> s4{ p4, srandom::randfloat<F>() };
    ndsample<F, F, N> s5{ p3, srandom::randfloat<F>() };
    ndsample<F, F, N> s6{ p2, srandom::randfloat<F>() };
    auto              samples = std::vector{ s1, s2, s3, s4, s5, s6 };

    ndtree<F, F, N> tree(std::span{ samples }, 3, 1);
    std::cout << tree;

    return EXIT_SUCCESS;
}

int debug()
{
    using namespace ndt;
    using F          = float;
    constexpr auto N = 3;

    using namespace utility::random;

    const ndpoint<F, N> d1 = { 1, 1, 1 };
    const ndpoint<F, N> d2 = { -1, -1, -1 };
    const ndpoint<F, N> p1 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<F, N> p2 = { srandom::randfloat<F>(),
                               srandom::randfloat<F>(),
                               srandom::randfloat<F>() };
    const ndpoint<F, N> p3 = { srandom::randfloat<F>(),
                               -srandom::randfloat<F>(),
                               -srandom::randfloat<F>() };
    const ndpoint<F, N> p4 = { -srandom::randfloat<F>(),
                               -srandom::randfloat<F>(),
                               srandom::randfloat<F>() };

    ndsample<F, F, N> sd1{ d1, srandom::randfloat<F>() };
    ndsample<F, F, N> sd2{ d2, srandom::randfloat<F>() };
    ndsample<F, F, N> s1{ p1, srandom::randfloat<F>() };
    ndsample<F, F, N> s2{ p2, srandom::randfloat<F>() };
    ndsample<F, F, N> s3{ p3, srandom::randfloat<F>() };
    ndsample<F, F, N> s4{ p4, srandom::randfloat<F>() };
    ndsample<F, F, N> s5{ p3, srandom::randfloat<F>() };
    ndsample<F, F, N> s6{ p2, srandom::randfloat<F>() };
    auto              samples = std::vector{ s1, s2, s3, s4, s5, s6 };

    std::cout << p1 << "\n";
    std::cout << p2 << "\n";

    std::cout << detail::compute_limits(std::array{ p1, p2, p3, p4 }).min() << "\n";
    std::cout << detail::compute_limits(std::array{ p1, p2, p3, p4 }).max() << "\n";
    std::cout << detail::in(p1, { p2, p3 });
    std::cout << detail::count_in(std::array{ p1, p1, p4, p3 }, ndboundary{ d1, d2 })
              << '\n';

    ndtree<F, F, N> tree(std::span{ samples }, 3, 1, ndboundary<F, N>{ d2, d1 });
    std::cout << tree;

    return EXIT_SUCCESS;
}

int main()
{
    debug();
    test();
}
