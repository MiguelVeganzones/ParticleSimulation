#pragma once

#include "compile_time_utility.hpp"
#include "energy.hpp"
#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include "random.hpp"
#include "simulation_config.hpp"
#include "yoshida.hpp"
#include <chrono>
#include <iostream>

namespace simulation::bf
{

using namespace pm::interaction;

template <
    pm::particle_concepts::Particle  Particle_Type,
    pm::interaction::InteractionType Interaction_Type
    // typename Solver_Type
    >
class brute_force_computation
{
public:
    using particle_t    = Particle_Type;
    using solver_t      = solvers::yoshida4_solver<brute_force_computation, particle_t>;
    using interaction_t = particle_interaction_t<particle_t, Interaction_Type>;
    static_assert(pm::particle_concepts::Interaction<interaction_t>);
    using value_type                              = typename particle_t::value_type;
    using duration_t                              = std::chrono::duration<value_type>;
    using acceleration_t                          = typename particle_t::acceleration_t;
    using position_t                              = typename particle_t::position_t;
    using velocity_t                              = typename particle_t::velocity_t;
    using mass_t                                  = typename particle_t::mass_t;
    using owning_container_t                      = std::vector<particle_t>;
    inline static constexpr auto s_working_copies = solver_t::s_working_copies;

    brute_force_computation(
        std::vector<particle_t> const& particles,
        /*
        utility::concepts::Duration auto const sim_duration,
        utility::concepts::Duration auto const sim_dt
        */
        simulation::config::simulation_common_config<particle_t> const& base_config
    ) :
        m_simulation_duration{
            std::chrono::duration_cast<duration_t>(base_config.duration_)
        },
        m_dt{ std::chrono::duration_cast<duration_t>(base_config.dt_) },
        m_particles{
            utility::compile_time_utility::array_factory<s_working_copies + 1>(particles)
        },
        m_simulation_size{ std::ranges::size(m_particles[0]) },
        m_solver(this, m_simulation_size, m_dt)
    {
        assert(m_dt > duration_t{ 0 });
        assert(m_simulation_duration > duration_t{ 0 });
    }

    auto run() noexcept -> void
    {
// Plotting is this ugly yet again unfortunately
// Actualy it just does not even work
#ifdef USE_ROOT_PLOTTING
        root_plotting::scatter_plot_3D scatter_plot;

        std::vector<float> x(m_simulation_size);
        std::vector<float> y(m_simulation_size);
        std::vector<float> z(m_simulation_size);
        std::size_t        iteration{};
#endif
        while (m_current_time < m_simulation_duration)
        {
            m_solver.run();
            m_current_time += m_dt;
            if (utility::random::srandom::randfloat<float>() < 0.02f)
            {
                std::cout << "Current time: " << m_current_time << '\n';
                std::cout << pm::energy::compute_kinetic_energy(current_system_state()) +
                                 pm::energy::compute_gravitational_potential_energy(
                                     current_system_state()
                                 )
                          << std::endl;
            }
        }
#ifdef USE_ROOT_PLOTTING
        if (iteration++ % 2 == 0)
        {
            for (auto j = decltype(m_simulation_size){}; j != m_simulation_size; ++j)
            {
                x[j] = static_cast<float>(m_particles[s_working_copies][j].position()[0]);
                y[j] = static_cast<float>(m_particles[s_working_copies][j].position()[1]);
                z[j] = static_cast<float>(m_particles[s_working_copies][j].position()[2]);
            }
            scatter_plot.plot(static_cast<int>(m_simulation_size), &x[0], &y[0], &z[0]);
        }
#endif
    }

    auto get_acceleration(std::size_t copy_idx, std::size_t p_idx) const noexcept
        -> acceleration_t
    {
        acceleration_t acc{};
        auto const&    p = m_particles[copy_idx][p_idx];
        for (auto const& other : m_particles[copy_idx])
        {
            if (other.id() != p.id()) [[likely]]
            {
                ++m_f_eval_count;
                acc = std::move(acc) + interaction_t::acceleration_contribution(p, other);
            }
        }
        return acc;
    }

    inline auto commit_buffer(std::size_t) noexcept -> void
    {
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
    std::size_t                                          m_simulation_size;
    solver_t                                             m_solver;
    mutable std::size_t                                  m_f_eval_count = 0;
};

} // namespace simulation::bf
