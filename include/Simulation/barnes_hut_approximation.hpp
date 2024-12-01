#pragma once

#include "concepts.hpp"
#include "ndtree.hpp"
#include "particle_concepts.hpp"
#include <chrono>

namespace simulation::bh_appox
{

template <
    pm::particle_concepts::Particle Particle_Type,
    typename Solver_Type,
    std::size_t Tree_Fanout = 2>
class barnes_hut_approximation
{
public:
    using particle_t                           = Particle_Type;
    inline static constexpr auto s_tree_fanout = Tree_Fanout;
    using tree_t                               = ndt::ndtree<s_tree_fanout, particle_t>;
    using solver_t                             = Solver_Type;
    using interaction_t                        = typename solver_t::interaction_t;
    using depth_t                              = typename tree_t::depth_t;
    using size_type                            = typename tree_t::size_type;
    using boundary_t                           = typename tree_t::boundary_t;
    using duration_t                           = std::chrono::seconds;
    using value_type                           = typename particle_t::value_type;
    using acceleration_t                       = typename particle_t::acceleration_t;
    using position_t                           = typename particle_t::position_t;
    using mass_t                               = typename particle_t::mass_t;

    // TODO: Improve interface, too many parameters, implement propper move ctor and move
    // them in.
    barnes_hut_approximation(
        std::span<particle_t>                  particles,
        utility::concepts::Duration auto const sim_duration,
        utility::concepts::Duration auto const sim_dt,
        depth_t const                          tree_max_depth,
        size_type const                        tree_box_capacity,
        std::optional<boundary_t>              tree_bounds = std::nullopt
    ) :
        m_dt{ std::chrono::duration_cast<duration_t>(sim_dt) },
        m_simulation_duration{ std::chrono::duration_cast<duration_t>(sim_duration) },
        m_particles{ particles },
        m_ndtree(m_particles, tree_max_depth, tree_box_capacity, tree_bounds),
        m_solver(m_particles, m_dt)
    {
        while (m_current_time < m_simulation_duration)
        {
            m_solver.run();
        }
    }

    auto get_acceleration(
        std::size_t           idx,
        std::span<mass_t>     mass,
        std::span<position_t> pos
    ) noexcept -> acceleration_t
    {
        assert(std::ranges::size(mass) == std::ranges::size(pos));
        acceleration_t acc{};
        for (std::size_t i = 0; i != std::ranges::size(mass); ++i)
        {
            if (i != idx) [[likely]]
            {
                acc = std::move(acc) +
                      interaction_t::acceleration_contribution(pos[idx], pos[i], mass[i]);
            }
        }
#if DEBUG_PRINT_INTERACTION
        std::cout << "Acc " << idx << ": " << acc << '\n';
#endif
        return acc;
    }

private:
    duration_t            m_current_time{};
    duration_t            m_simulation_duration;
    duration_t            m_dt;
    std::span<particle_t> m_particles;
    tree_t                m_ndtree;
    solver_t              m_solver;
};

} // namespace simulation::bh_appox
