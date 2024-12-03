#pragma once

#include "concepts.hpp"
#include "csv_logger.hpp"
#include "ndtree.hpp"
#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include "stopwatch.hpp"
#include "utils.hpp"
#include "yoshida.hpp"
#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "scatter_plot.hpp"
#endif
#include "random.hpp" // ToDo Remove

namespace simulation::bh_appox
{

using namespace pm::interaction;

template <
    pm::particle_concepts::Particle Particle_Type,
    // typename Solver_Type,
    std::size_t Tree_Fanout = 2>
class barnes_hut_approximation
{
public:
    using particle_t                           = Particle_Type;
    inline static constexpr auto s_tree_fanout = Tree_Fanout;
    using tree_t                               = ndt::ndtree<s_tree_fanout, particle_t>;
    using box_t                                = typename tree_t::box_t;
    using solver_t       = solvers::yoshida4_solver<barnes_hut_approximation, particle_t>;
    using interaction_t  = gravitational_interaction_calculator<particle_t>;
    using depth_t        = typename tree_t::depth_t;
    using size_type      = typename tree_t::size_type;
    using boundary_t     = typename tree_t::boundary_t;
    using value_type     = typename particle_t::value_type;
    using acceleration_t = typename particle_t::acceleration_t;
    using position_t     = typename particle_t::position_t;
    using velocity_t     = typename particle_t::velocity_t;
    using mass_t         = typename particle_t::mass_t;
    using duration_t     = std::chrono::duration<value_type>;
    using owning_container_t                      = std::vector<particle_t>;
    inline static constexpr auto s_working_copies = solver_t::s_working_copies;
    inline static constexpr auto s_theta          = value_type{ 0.4 };

    // TODO: Improve interface, too many parameters, implement proper move ctor and move
    // them in.
    barnes_hut_approximation(
        std::vector<particle_t>                particles,
        utility::concepts::Duration auto const sim_duration,
        utility::concepts::Duration auto const sim_dt,
        depth_t const                          tree_max_depth,
        size_type const                        tree_box_capacity,
        std::optional<boundary_t>              tree_bounds = std::nullopt
    ) :
        m_simulation_duration{ std::chrono::duration_cast<duration_t>(sim_duration) },
        m_dt{ std::chrono::duration_cast<duration_t>(sim_dt) },
        // TODO: Hardcoded solver running_copies = 4
        m_particles{ particles, particles, particles, particles, particles }, // TODO Fix
        m_ndtrees{
            tree_t(m_particles[0], tree_max_depth, tree_box_capacity, tree_bounds),
            tree_t(m_particles[1], tree_max_depth, tree_box_capacity, tree_bounds),
            tree_t(m_particles[2], tree_max_depth, tree_box_capacity, tree_bounds),
            tree_t(m_particles[3], tree_max_depth, tree_box_capacity, tree_bounds)
        },
        m_simulation_size{ std::ranges::size(m_particles[s_working_copies]) },
        m_solver(this, m_simulation_size, m_dt)
    {
    }

    auto run() noexcept -> void
    {
// Plotting is this ugly as on now unfortunately
#ifdef USE_ROOT_PLOTTING
        root_plotting::scatter_plot_3D scatter_plot;

        std::vector<float> x(m_simulation_size);
        std::vector<float> y(m_simulation_size);
        std::vector<float> z(m_simulation_size);
        std::size_t        iteration{};
#endif
        m_ndtrees[0].cache_summary();
        std::cout << m_ndtrees[0] << '\n';
        while (m_current_time < m_simulation_duration)
        {
            utility::timing::stopwatch s{ "Iteration" };
            m_solver.run();
            m_current_time += m_dt;
            std::cout << m_current_time << '\n';
            std::ostringstream filename;
            filename << "execution_data_" << m_current_time;
            if (utility::random::srandom::randfloat<float>() < 0.01f)
            {
                logger::csv::write_to_csv(m_particles[s_working_copies], filename.str());
            }
#ifdef USE_ROOT_PLOTTING
            if (iteration++ % 2 == 0)
            {
                for (auto j = decltype(m_simulation_size){}; j != m_simulation_size; ++j)
                {
                    x[j] = static_cast<float>(m_particles[s_working_copies][j].position(
                    )[0]);
                    y[j] = static_cast<float>(m_particles[s_working_copies][j].position(
                    )[1]);
                    z[j] = static_cast<float>(m_particles[s_working_copies][j].position(
                    )[2]);
                }
                scatter_plot.plot(
                    static_cast<int>(m_simulation_size), &x[0], &y[0], &z[0]
                );
            }
#endif
        }
        std::cout << count << '\n';
    }

    auto get_acceleration(size_type copy_idx, std::size_t p_idx) noexcept
        -> acceleration_t
    {
        auto const acc =
            get_box_contribution(m_particles[copy_idx][p_idx], m_ndtrees[copy_idx].box());
        return acc;
    }

    [[nodiscard]]
    auto get_box_contribution(particle_t const& p, box_t const& b) -> acceleration_t
    {
        if (!b.summary().has_value() || b.summary().value().id() == p.id())
        {
            return acceleration_t{};
        }
        auto const summary = b.summary().value();
        const auto s       = pm::utils::l2_norm(b.space_diagonal().value());
        const auto d       = pm::utils::l2_norm(
            pm::utils::distance(p.position(), summary.position()).value()
        );
        if ((s / d) < s_theta)
        {
            ++count;
            return interaction_t::acceleration_contribution(p, summary);
        }
        else
        {
            acceleration_t acc{};
            if (b.fragmented())
            {
                for (auto const& subbox : b.subboxes())
                {
                    acc = std::move(acc) + get_box_contribution(p, subbox);
                }
            }
            else
            {
                for (auto const* const other : b.contained_elements())
                {
                    if (other->id() != p.id()) [[likely]]
                    {
                        ++count;
                        acc = std::move(acc) +
                              interaction_t::acceleration_contribution(p, *other);
                    }
                }
            }
            return acc;
        }
    }

    inline auto commit_buffer(std::size_t working_copy_idx) noexcept -> void
    {
        m_ndtrees[working_copy_idx].reorganize();
        m_ndtrees[working_copy_idx].cache_summary();
    }

    [[nodiscard]]
    inline auto position_read(std::size_t p_idx) const noexcept -> position_t const&

    {
        return m_particles[s_working_copies][p_idx].position();
    }

    inline auto position_write(std::size_t p_idx, position_t value) noexcept -> void
    {
        m_particles[s_working_copies][p_idx].position() = value;
    }

    [[nodiscard]]
    inline auto position_buffer_read(std::size_t buffer_id, std::size_t p_idx)
        const noexcept -> position_t const&
    {
        return m_particles[buffer_id][p_idx].position();
    }

    inline auto position_buffer_write(
        std::size_t       buffer_id,
        std::size_t       p_idx,
        position_t const& value
    ) noexcept -> void
    {
        m_particles[buffer_id][p_idx].position() = value;
    }

    [[nodiscard]]
    inline auto velocity_read(std::size_t p_idx) const noexcept -> velocity_t const&
    {
        return m_particles[s_working_copies][p_idx].velocity();
    }

    inline auto velocity_write(std::size_t p_idx, velocity_t value) noexcept -> void
    {
        m_particles[s_working_copies][p_idx].velocity() = value;
    }

    [[nodiscard]]
    inline auto velocity_buffer_read(std::size_t buffer_id, std::size_t p_idx)
        const noexcept -> velocity_t const&
    {
        return m_particles[buffer_id][p_idx].velocity();
    }

    inline auto velocity_buffer_write(
        std::size_t       buffer_id,
        std::size_t       p_idx,
        velocity_t const& value
    ) noexcept -> void
    {
        m_particles[buffer_id][p_idx].velocity() = value;
    }

private:
    // TODO Reorder
    duration_t                                           m_current_time{};
    duration_t                                           m_simulation_duration;
    duration_t                                           m_dt;
    std::array<owning_container_t, s_working_copies + 1> m_particles;
    std::array<tree_t, s_working_copies>                 m_ndtrees;
    size_type                                            m_simulation_size;
    solver_t                                             m_solver;
    std::size_t                                          count = 0;
};

} // namespace simulation::bh_appox
