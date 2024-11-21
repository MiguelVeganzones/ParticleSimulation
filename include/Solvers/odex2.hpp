#pragma once

#include "particle_concepts.hpp"
#include "particle_interaction.hpp"

#define DEBUG_PRINT_ODEX2 (false)

namespace solvers
{

using namespace pm;
using namespace pm::interaction;

template <std::size_t Order, particle_concepts::Particle Particle_Type>
    requires(Order > 1)
struct odex2_solver
{
    inline static constexpr auto s_order = Order;
    using particle_t                     = Particle_Type;
    using value_type                     = typename particle_t::value_type;
    using position_t                     = typename particle_t::position_t;
    using velocity_t                     = typename particle_t::velocity_t;
    using mass_t                         = typename particle_t::mass_t;
    using interaction_t = gravitational_interaction_calculator<particle_t>;

    // Try with O-1 and do not copy x0
    // Also, not copy the mass all the time
    std::vector<mass_t>                              mass_buffer_{};
    std::array<std::vector<position_t>, s_order + 1> position_buffer_{};
    std::array<std::vector<velocity_t>, s_order + 1> velocity_buffer_{};
    interaction_t                                    interaction;
    std::span<particle_t>                            particles_;
    std::size_t                                      size_;

    odex2_solver(std::span<particle_t> particles) :
        particles_{ particles },
        size_{ std::ranges::size(particles) }
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

    auto run(utility::concepts::Duration auto delta_t) -> void
    {
        using duration_t = std::chrono::duration<value_type>; // default is seconds
        const auto H     = std::chrono::duration_cast<duration_t>(delta_t).count();
        const auto h     = H / (2 * s_order);
        const auto h2    = h * 2;

        for (auto& v : position_buffer_)
        {
            v.clear();
            v.resize(size_);
        }
        for (auto& v : velocity_buffer_)
        {
            v.clear();
            v.resize(size_);
        }

        for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
        {
            position_buffer_[0][p_idx] = particles_[p_idx].position();
        }
        for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
        {
            velocity_buffer_[0][p_idx] =
                particles_[p_idx].velocity() +
                interaction.get_acceleration(p_idx, mass_buffer_, position_buffer_[0]) *
                    h;
        }
        for (std::size_t i = 2; i != 2 * s_order + 4; i += 2)
        {
            for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
            {
                if (i > 2)
                {
                    // Compute y'_{i-1}
                    velocity_buffer_[(int)std::floor((i - 1) / 2)][p_idx] =
                        velocity_buffer_[(int)std::floor((i - 3) / 2)][p_idx] +
                        interaction.get_acceleration(
                            p_idx,
                            mass_buffer_,
                            position_buffer_[(int)std::floor((i - 2) / 2)]
                        ) * h2;
                }
                if (i != 2 * s_order + 2)
                {
                    // Compute y_i
                    position_buffer_[(int)std::floor(i / 2)][p_idx] =
                        position_buffer_[(int)std::floor((i - 2) / 2)][p_idx] +
                        velocity_buffer_[(int)std::floor((i - 1) / 2)][p_idx] * h2;
                }
            }
        }
        for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
        {
            particles_[p_idx].position() = position_buffer_[s_order][p_idx];
            particles_[p_idx].velocity() = (velocity_buffer_[s_order - 1][p_idx] +
                                            velocity_buffer_[s_order][p_idx]) *
                                           value_type{ 0.5 };
        }
#if DEBUG_PRINT_ODEX2
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
