#pragma once

#include "particle_concepts.hpp"
#include "particle_interaction.hpp"
#include "utils.hpp"

#define DEBUG_PRINT_YOSHIDA (false)

namespace solvers
{

using namespace pm;
using namespace pm::interaction;

template <particle_concepts::Particle Particle_Type>
struct yoshida4_solver
{
    inline static constexpr auto s_order = 4;
    using particle_t                     = Particle_Type;
    using value_type                     = typename particle_t::value_type;
    using position_t                     = typename particle_t::position_t;
    using velocity_t                     = typename particle_t::velocity_t;
    using acceleration_t                 = typename particle_t::acceleration_t;
    using mass_t                         = typename particle_t::mass_t;
    using interaction_t = gravitational_interaction_calculator<particle_t>;
    using duration_t    = std::chrono::duration<value_type>; // default is seconds

    inline static constexpr auto x0 = value_type{ -1.70241438392 };
    inline static constexpr auto x1 = value_type{ 1.35120719196 };
    inline static constexpr auto c =
        std::array<value_type, s_order>{ value_type{ 0.5 } * x1,
                                         value_type{ 0.5 } * (x0 + x1),
                                         value_type{ 0.5 } * (x0 + x1),
                                         value_type{ 0.5 } * x1 };

    inline static constexpr auto d = std::array<value_type, s_order - 1>{ x1, x0, x1 };

    std::vector<mass_t>                          mass_buffer_{};
    std::array<std::vector<position_t>, s_order> position_buffer_{};
    std::array<std::vector<velocity_t>, s_order> velocity_buffer_{};
    interaction_t                                interaction;
    std::span<particle_t>                        particles_;
    std::size_t                                  size_;
    duration_t                                   dt_;

    yoshida4_solver(
        std::span<particle_t>                  particles,
        utility::concepts::Duration auto const delta_t
    ) :
        particles_{ particles },
        size_{ std::ranges::size(particles) },
        dt_{ delta_t }
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
    }

    auto run() -> void
    {
        const auto dt = dt_.count();

        for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
        {
            position_buffer_[0][p_idx] =
                particles_[p_idx].position() + c[0] * particles_[p_idx].velocity() * dt;
            velocity_buffer_[0][p_idx] = particles_[p_idx].velocity();
        }
        value_type max_acc{};
        for (std::size_t i = 1; i != s_order; ++i)
        {
            for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
            {
                const auto a = interaction.get_acceleration(
                    p_idx, mass_buffer_, position_buffer_[i - 1]
                );
                max_acc = std::max(max_acc, utils::l2_norm(a.value()));
                velocity_buffer_[i][p_idx] =
                    velocity_buffer_[i - 1][p_idx] + d[i - 1] * a * dt;
                position_buffer_[i][p_idx] = position_buffer_[i - 1][p_idx] +
                                             c[i] * velocity_buffer_[i][p_idx] * dt;
            }
        }
        for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
        {
            particles_[p_idx].position() =
                position_buffer_[s_order - 1][p_idx] +
                c[s_order - 1] * velocity_buffer_[s_order - 1][p_idx] * dt;
            particles_[p_idx].velocity() = velocity_buffer_[s_order - 1][p_idx];
        }
#if DEBUG_PRINT_YOSHIDA
        for (std::size_t i = 0; i != size_; ++i)
        {
            for (std::size_t j = 0; j != s_order; ++j)
            {
                std::cout << "---------------------\n";
                std::cout << position_buffer_[j][i] << '\t';
                std::cout << velocity_buffer_[j][i] << '\n';
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
