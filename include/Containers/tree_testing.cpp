#include "ndtree.hpp"
#include <iostream>
#include <span>
#include <tuple>
#include <vector>

int main()
{
    using namespace ndt;
    using F          = float;
    constexpr auto N = 3;

    const ndpoint<F, N> p1 = { 1, 4, 1 };
    const ndpoint<F, N> p2 = { 0, 0, 0 };
    const ndpoint<F, N> p3 = { 2, 5, 5 };
    const ndpoint<F, N> p4 = { 4, 2, 1 };

    ndsample<F, F, N> s1{ p1, 0 };
    ndsample<F, F, N> s2{ p2, -4 };
    ndsample<F, F, N> s3{ p3, 6 };
    ndsample<F, F, N> s4{ p4, 1 };
    ndsample<F, F, N> s5{ p2, 2 };
    ndsample<F, F, N> s6{ p3, 3 };
    auto              samples = std::vector{ s1, s2, s3, s4, s5, s6 };

    std::cout << p1 << "\n";
    std::cout << p2 << "\n";

    std::cout << detail::compute_limits(std::array{ p1, p2, p3, p4 }).min << "\n";
    std::cout << detail::compute_limits(std::array{ p1, p2, p3, p4 }).max << "\n";
    std::cout << detail::in(p1, { p2, p3 });
    std::cout << detail::count_in(std::array{ p1, p1, p4, p3 }, ndboundary{ p2, p3 })
              << '\n';

    ndtree<F, F, N> tree(std::span{ samples }, 2, 1);
    std::cout << tree;
}
