#include "ndtree.hpp"
#include "particle.hpp"
#include "particle_factory.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(TreeTests, TreePerformsCorrectSplit)
{
    // GTEST_SKIP() << "skipping tree tests";
    static constexpr auto N       = 3;
    using F                       = double;
    using particle_t              = pm::particle::ndparticle<N, F>;
    const std::size_t Tree_Fanout = 2;
    // inline static constexpr auto s_tree_fanout = Tree_Fanout;
    const int         depth_t         = 4;
    const int         box_capacity_t  = 4;
    const float       universe_radius = 100.0;
    const std::size_t size            = 10;
    auto particles = particle_factory::generate_particle_set<N, F>(size, universe_radius);
    auto tree_t =
        ndt::ndtree<Tree_Fanout, particle_t>(particles, depth_t, box_capacity_t);
    std::cout << tree_t.box().boxes();
    const auto n_elements = tree_t.box().elements();
    ASSERT_EQ(n_elements, 10);
}