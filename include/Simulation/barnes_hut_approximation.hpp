#pragma once

#include "compile_time_utility.hpp"
#include "concepts.hpp"
#include "generics.hpp"
#include "ndtree.hpp"
#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include "physical_magnitudes.hpp"
#include "simulation_config.hpp"
#include "utils.hpp"
#include "yoshida.hpp"
#include <atomic>
#include <bits/ranges_algo.h>
#include <chrono>
#include <iostream>
#include <ranges>
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include "scatter_plot_3D.hpp"
#endif
#define MEASURE_ENERGY 0
#if MEASURE_ENERGY
#include "energy.hpp"
#include "random.hpp"
#endif

namespace simulation::bh_approx
{

using namespace pm::interaction;

template <
    pm::particle_concepts::Particle  Particle_Type,
    pm::interaction::InteractionType Interaction_Type,
    // typename Solver_Type,
    std::size_t Tree_Fanout = 2>
class barnes_hut_approximation
{
public:
    using particle_t                           = Particle_Type;
    inline static constexpr auto s_tree_fanout = Tree_Fanout;
    using tree_t                               = ndt::ndtree<s_tree_fanout, particle_t>;
    using box_t                                = typename tree_t::box_t;
    using solver_t      = solvers::yoshida4_solver<barnes_hut_approximation, particle_t>;
    using interaction_t = particle_interaction_t<particle_t, Interaction_Type>;
    static_assert(pm::particle_concepts::Interaction<interaction_t>);
    using depth_t                                 = typename tree_t::depth_t;
    using size_type                               = typename tree_t::size_type;
    using boundary_t                              = typename tree_t::boundary_t;
    using value_type                              = typename particle_t::value_type;
    using acceleration_t                          = typename particle_t::acceleration_t;
    using position_t                              = typename particle_t::position_t;
    using velocity_t                              = typename particle_t::velocity_t;
    using mass_t                                  = typename particle_t::mass_t;
    using duration_t                              = std::chrono::duration<value_type>;
    using owning_container_t                      = std::vector<particle_t>;
    inline static constexpr auto s_working_copies = solver_t::s_working_copies;
    inline static constexpr auto s_theta_range =
        utility::generics::interval{ value_type{ 0 }, value_type{ 1 } };

    barnes_hut_approximation(
        std::vector<particle_t> const& particles,
        /*
        utility::concepts::Duration auto const sim_duration,
        utility::concepts::Duration auto const sim_dt,
        value_type                             theta,
        depth_t const                          tree_max_depth,
        size_type const                        tree_box_capacity,
    */
        simulation::config::simulation_common_config<particle_t> const&   base_config,
        simulation::config::barnes_hut_specific_config<particle_t> const& specific_config,
        std::optional<boundary_t> tree_bounds = std::nullopt
    ) :
        m_simulation_duration{
            std::chrono::duration_cast<duration_t>(base_config.duration_)
        },
        m_dt{ std::chrono::duration_cast<duration_t>(base_config.dt_) },
        m_particles{
            utility::compile_time_utility::array_factory<s_working_copies + 1>(particles)
        },
        m_ndtrees{ utility::compile_time_utility::array_factory<s_working_copies>(
            [this, specific_config, tree_bounds](std::size_t I) -> tree_t {
                return tree_t(
                    m_particles[I],
                    specific_config.tree_max_depth_,
                    specific_config.tree_box_capacity_,
                    tree_bounds
                );
            }
        ) },
        m_simulation_size{ std::ranges::size(current_system_state()) },
        m_solver(this, m_simulation_size, m_dt),
        m_theta_sq{ std::pow(specific_config.theta_, value_type{ 2 }), s_theta_range }
    {
        assert(m_dt > duration_t{ 0 });
        assert(m_simulation_duration > duration_t{ 0 });
    }

    auto run() noexcept -> void
    {
// Plotting is this ugly as on now unfortunately
#ifdef USE_ROOT_PLOTTING
        using data_point_t = typename plotting::plots_3D::scatter_plot_3D::data_point;
        std::vector<std::vector<data_point_t>> data(1);
        data[0].resize(m_simulation_size);
        plotting::plots_3D::scatter_plot_3D scatter_plot(data);
#endif
        m_ndtrees[0].cache_summary();
        while (m_current_time < m_simulation_duration)
        {
            m_solver.run();
            m_current_time += m_dt;
#if MEASURE_ENERGY
            if (utility::random::srandom::randfloat<float>() < 0.02f)
            {
                std::cout << "Current time: " << m_current_time << '\n';
                std::cout << "System total energy: "
                          << pm::magnitudes::energy<
                                 value_type>{ pm::energy::compute_kinetic_energy(
                                                  current_system_state()
                                              ) +
                                              pm::energy::
                                                  compute_gravitational_potential_energy(
                                                      current_system_state()
                                                  ) }
                          << std::endl;
            }
#endif
#ifdef LOG_TO_CSV
            std::ostringstream filename;
            filename << "execution_data_" << m_current_time;
            if (utility::random::srandom::randfloat<float>() < 0.01f)
            {
                logger::csv::write_to_csv(m_particles[s_working_copies], filename.str());
            }
#endif
#ifdef USE_ROOT_PLOTTING
            if (m_current_time - m_prev_plot_time >= m_plot_interval)
            {
                for (auto j = decltype(m_simulation_size){}; j != m_simulation_size; ++j)
                {
                    data[0][j].x =
                        static_cast<float>(m_particles[s_working_copies][j].position()[0]
                        );
                    data[0][j].y =
                        static_cast<float>(m_particles[s_working_copies][j].position()[1]
                        );
                    data[0][j].z =
                        static_cast<float>(m_particles[s_working_copies][j].position()[2]
                        );
                }
                scatter_plot.render();
                m_prev_plot_time = m_current_time;
            }
#endif
        }
    }

    auto get_acceleration(size_type copy_idx, std::size_t p_idx) noexcept
        -> acceleration_t
    {
        return get_box_contribution(
            m_particles[copy_idx][p_idx], m_ndtrees[copy_idx].box()
        );
    }

    [[nodiscard]]
    auto get_box_contribution(particle_t const& p, box_t const& b) const -> acceleration_t
    {
        if (!b.summary().has_value() || b.summary().value().id() == p.id())
        {
            return acceleration_t{};
        }
        auto const summary = b.summary().value();
        const auto s       = pm::utils::l2_norm_sq(b.diagonal_length().value());
        const auto d       = pm::utils::l2_norm_sq(
            pm::utils::distance(p.position(), summary.position()).value()
        );
        if ((s / d) < m_theta_sq.get())
        {
            ++m_f_eval_count;
            return interaction_t::acceleration_contribution(p, summary);
        }
        else
        {
            if (b.fragmented())
            {
                return std::ranges::fold_left(
                    b.subboxes(),
                    acceleration_t{},
                    [this, p](auto acc, auto const& subbox) {
                        return acceleration_t{ std::move(acc) +
                                               get_box_contribution(p, subbox) };
                    }
                );
            }
            else
            {
                return std::ranges::fold_left(
                    b.contained_elements(),
                    acceleration_t{},
                    [this, p](auto acc, auto const* const other) {
                        if (other->id() != p.id()) [[likely]]
                        {
                            ++m_f_eval_count;
                            return acceleration_t{
                                std::move(acc) +
                                interaction_t::acceleration_contribution(p, *other)
                            };
                        }
                        else
                        {
                            return acc;
                        }
                    }
                );
            }
        }
    }

    [[nodiscard]]
    inline auto current_system_state() const noexcept -> auto const&
    {
        return m_particles[s_working_copies];
    }

    [[nodiscard]]
    inline auto current_system_state() noexcept -> auto&
    {
        return m_particles[s_working_copies];
    }

    inline auto commit_buffer(std::size_t working_copy_idx) noexcept -> void
    {
        m_ndtrees[working_copy_idx].reorganize();
        m_ndtrees[working_copy_idx].cache_summary();
    }

    [[nodiscard]]
    inline auto position_read(std::size_t p_idx) const noexcept -> position_t const&

    {
        return current_system_state()[p_idx].position();
    }

    inline auto position_write(std::size_t p_idx, position_t value) noexcept -> void
    {
        current_system_state()[p_idx].position() = value;
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
        return current_system_state()[p_idx].velocity();
    }

    inline auto velocity_write(std::size_t p_idx, velocity_t value) noexcept -> void
    {
        current_system_state()[p_idx].velocity() = value;
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

    [[nodiscard]]
    inline auto f_eval_count() const noexcept -> std::size_t
    {
        return m_f_eval_count;
    }

private:
    duration_t                                           m_current_time{};
    duration_t                                           m_simulation_duration;
    duration_t                                           m_dt;
    std::array<owning_container_t, s_working_copies + 1> m_particles;
    std::array<tree_t, s_working_copies>                 m_ndtrees;
    size_type                                            m_simulation_size;
    solver_t                                             m_solver;
    mutable std::atomic<std::size_t>                     m_f_eval_count = 0;
    utility::generics::ranged_value<value_type>          m_theta_sq;
#ifdef USE_ROOT_PLOTTING
    duration_t m_plot_interval  = duration_t{ 3.0 };
    duration_t m_prev_plot_time = -m_plot_interval;
#endif
};

} // namespace simulation::bh_approx
