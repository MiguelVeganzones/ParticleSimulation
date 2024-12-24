#pragma once

#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include <ranges>

#ifndef DEBUG_PRINT_RUNGE
#define DEBUG_PRINT_RUNGE (false)
#endif

namespace solvers
{

using namespace pm;
using namespace pm::interaction;

template <std::size_t Order, particle_concepts::Particle Particle_Type>
    requires(Order > 1)
struct runge_kutta_solver
{
    inline static constexpr auto s_order = Order;
    using particle_t                     = Particle_Type;
    using value_type                     = typename particle_t::value_type;
    using position_t                     = typename particle_t::position_t;
    using velocity_t                     = typename particle_t::velocity_t;
    using acceleration_t                 = typename particle_t::acceleration_t;
    using interaction_t = gravitational_interaction_calculator<particle_t>;
    using mass_t        = typename particle_t::mass_t;
    using duration_t    = std::chrono::duration<value_type>; // default is seconds

    // Try with O-1 and do not copy x0
    // Also, not copy the mass all the time
    std::vector<mass_t>                              mass_buffer_{};
    std::array<std::vector<position_t>, s_order>     position_buffer_{};
    std::array<std::vector<velocity_t>, s_order>     velocity_buffer_{};
    std::array<std::vector<acceleration_t>, s_order> acceleration_buffer_{};
    interaction_t                                    interaction;
    std::span<particle_t>                            particles_;
    std::size_t                                      size_;
    duration_t                                       dt_;

    runge_kutta_solver(
        std::span<particle_t>            particles,
        utility::concepts::Duration auto delta_t
    ) :
        particles_{ particles },
        size_{ std::ranges::size(particles) },
        dt_{ std::chrono::duration_cast<duration_t>(delta_t) }
    {
        mass_buffer_.resize(size_);
        for (std::size_t i = 0; i != size_; ++i)
        {
            mass_buffer_[i] = particles[i].mass();
        }
        for (auto& v : position_buffer_)
        {
            v.resize(size_);
        }
        for (auto& v : velocity_buffer_)
        {
            v.resize(size_);
        }
        for (auto& v : acceleration_buffer_)
        {
            v.resize(size_);
        }
    }

    auto run() -> void
    {
        const auto h  = dt_.count();
        const auto h2 = h / value_type{ 2 };
        const auto h_ = std::array{ value_type{ 0 }, h2, h2, h };

        for (std::size_t j = 0; j != s_order + 1; ++j)
        {
            for (auto i = decltype(size_){ 0 }; i != size_; ++i)
            {
                if (j == 0)
                {
                    position_buffer_[j][i] = particles_[i].position();
                    velocity_buffer_[j][i] = particles_[i].velocity();
                }
                else if (j != s_order)
                {
                    acceleration_buffer_[j - 1][i] = interaction.get_acceleration(
                        i, mass_buffer_, position_buffer_[j - 1]
                    );
                    position_buffer_[j][i] =
                        position_buffer_[0][i] + h_[j] * velocity_buffer_[j - 1][i];
                    velocity_buffer_[j][i] =
                        velocity_buffer_[0][i] + h_[j] * acceleration_buffer_[j - 1][i];
                }
                else
                {
                    acceleration_buffer_[j - 1][i] = interaction.get_acceleration(
                        i, mass_buffer_, position_buffer_[j - 1]
                    );
                }
            }
        }
        for (auto i = decltype(size_){ 0 }; i != size_; ++i)
        {
            particles_[i].position() +=
                h / value_type{ 6 } *
                (velocity_buffer_[0][i] + value_type{ 2 } * velocity_buffer_[1][i] +
                 value_type{ 2 } * velocity_buffer_[2][i] + velocity_buffer_[3][i]);
            particles_[i].velocity() += h / value_type{ 6 } *
                                        (acceleration_buffer_[0][i] +
                                         value_type{ 2 } * acceleration_buffer_[1][i] +
                                         value_type{ 2 } * acceleration_buffer_[2][i] +
                                         acceleration_buffer_[3][i]);
        }
#if DEBUG_PRINT_RUNGE
        for (std::size_t i = 0; i != size_; ++i)
        {
            for (auto& v : particle_buffer_)
            {
                std::cout << "---------------------\n";
                std::cout << v[i] << '\n';
                std::cout << "---------------------\n";
            }
        }
        for (auto const& p : particles_)
        {
            std::cout << p << '\n';
        }
#endif
    }
};

} // namespace solvers
