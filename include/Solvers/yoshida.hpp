#pragma once

#include "particle_concepts.hpp"
#include "utils.hpp"

#define DEBUG_PRINT_YOSHIDA (false)

namespace solvers
{

using namespace pm;

template <typename System, particle_concepts::Particle Particle_Type>
struct yoshida4_solver
{
    inline static constexpr auto s_order = 4;
    using particle_t                     = Particle_Type;
    using system_t                       = System;
    using value_type                     = typename particle_t::value_type;
    using position_t                     = typename particle_t::position_t;
    using velocity_t                     = typename particle_t::velocity_t;
    using acceleration_t                 = typename particle_t::acceleration_t;
    using mass_t                         = typename particle_t::mass_t;
    using duration_t = std::chrono::duration<value_type>; // default is seconds
    inline static constexpr auto s_working_copies = s_order;

    inline static constexpr auto x0 = value_type{ -1.70241438392 };
    inline static constexpr auto x1 = value_type{ 1.35120719196 };
    inline static constexpr auto c =
        std::array<value_type, s_order>{ value_type{ 0.5 } * x1,
                                         value_type{ 0.5 } * (x0 + x1),
                                         value_type{ 0.5 } * (x0 + x1),
                                         value_type{ 0.5 } * x1 };

    inline static constexpr auto d = std::array<value_type, s_order - 1>{ x1, x0, x1 };

    system_t*   system_;
    std::size_t size_;
    duration_t  dt_;

    yoshida4_solver(
        system_t*                              system,
        std::size_t                            size,
        utility::concepts::Duration auto const delta_t
    ) :
        system_{ system },
        size_{ size },
        dt_{ delta_t }
    {
    }

    // Some corners have been cut in this implementation...
    // TODO Fix, API too transparent and ugly
    auto run() -> void
    {
        const auto dt = dt_.count();

        for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
        {
            system_->position_buffer_write(
                0,
                p_idx,
                system_->position_read(p_idx) + c[0] * system_->velocity_read(p_idx) * dt
            );
            system_->velocity_buffer_write(0, p_idx, system_->velocity_read(p_idx));
        }
        value_type max_acc{};
        for (std::size_t i = 1; i != s_order; ++i)
        {
            for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
            {
                const auto a = system_->get_acceleration(i - 1, p_idx);
                max_acc      = std::max(max_acc, utils::l2_norm(a.value()));
                system_->velocity_buffer_write(
                    i,
                    p_idx,
                    system_->velocity_buffer_read(i - 1, p_idx) + d[i - 1] * a * dt
                );
                system_->position_buffer_write(
                    i,
                    p_idx,
                    system_->position_buffer_read(i - 1, p_idx) +
                        c[i] * system_->velocity_buffer_read(i, p_idx) * dt
                );
            }
        }
        for (std::size_t p_idx = 0; p_idx != size_; ++p_idx)
        {
            system_->position_write(
                p_idx,
                system_->position_buffer_read(s_order - 1, p_idx) +
                    c[s_order - 1] * system_->velocity_buffer_read(s_order - 1, p_idx) *
                        dt
            );
            system_->velocity_write(
                p_idx, system_->velocity_buffer_read(s_order - 1, p_idx)
            );
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
