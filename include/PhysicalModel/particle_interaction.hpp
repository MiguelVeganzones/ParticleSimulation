#pragma once

#include "particle_concepts.hpp"
#include "physical_constants.hpp"
#include "physical_magnitudes.hpp"
#include "utils.hpp"
#include <algorithm>
#include <exception>
#include <ranges>

#ifndef DEBUG_PRINT_RUNGE
#define DEBUG_PRINT_RUNGE (false)
#endif

namespace pm::interaction
{

using namespace particle_concepts;

template <Particle Particle_Type>
struct gravitational_interaction_calculator
{

    using particle_t     = Particle_Type;
    using value_type     = typename particle_t::value_type;
    using acceleration_t = typename particle_t::acceleration_t;
    using position_t     = typename particle_t::position_t;
    using mass_t         = typename particle_t::mass_t;

    auto acceleration_contribution(particle_t const& p1, particle_t const& p2) noexcept
        -> acceleration_t
    {
        const auto     distance = utils::distance(p1.position(), p2.position());
        const auto     d        = utils::l2_norm(distance.value());
        constexpr auto rs       = value_type{ 1e-2 };
#if DEBUG_PRINT_RUNGE
        std::cout << "Mj: " << p2.mass().magnitude() << '\n';
        std::cout << "D: " << distance << '\n';
        std::cout << "d3: " << d * d * d << '\n';
#endif
        return acceleration_t{ pm::physical_constants<value_type>::G *
                               p2.mass().magnitude() * distance /
                               std::pow(d * d + rs * rs, value_type{ 1.5 }) };
    }

    auto get_acceleration(std::size_t idx, std::span<particle_t> particles) noexcept
        -> magnitudes::linear_acceleration<
            Particle_Type::s_dimension,
            typename Particle_Type::value_type>

    {
        acceleration_t acc{};
        const auto&    p = particles[idx];
        for (std::size_t i = 0; i != std::ranges::size(particles); ++i)
        {
            if (i != idx) [[likely]]
            {
                const auto& other = particles[i];
                acc               = std::move(acc) + acceleration_contribution(p, other);
            }
        }
#if DEBUG_PRINT_RUNGE
        std::cout << "Acc " << idx << ": " << acc << '\n';
#endif
        return acc;
    }
};

template <std::size_t Order, particle_concepts::Particle Particle_Type>
    requires(Order > 1)
struct runge_kutta_solver
{
    inline static constexpr auto O = Order;
    using particle_t               = Particle_Type;
    using value_type               = typename particle_t::value_type;
    using position_t               = typename particle_t::position_t;
    using velocity_t               = typename particle_t::velocity_t;
    using interaction_t            = gravitational_interaction_calculator<particle_t>;

    // Try with O-1 and do not copy x0
    // Also, not copy the mass all the time
    std::array<std::vector<particle_t>, O> particle_buffer_{};
    interaction_t                          interaction;
    std::span<particle_t>                  particles_;
    std::size_t                            size_;

    runge_kutta_solver(std::span<particle_t> particles) :
        particles_{ particles },
        size_{ std::ranges::size(particles) }
    {
        std::cout << std::endl;
        for (auto& v : particle_buffer_)
        {
            v.reserve(size_);
        }
        for (std::size_t j = 0; j != O; ++j)
        {
            for (std::size_t i = 0; i != size_; ++i)
            {
                particle_buffer_[j].push_back(particles_[i]);
            }
        }
    }

    auto run(utility::concepts::Duration auto delta_t) -> void
    {
        using duration_t = std::chrono::duration<double>; // float seconds

        const auto h  = std::chrono::duration_cast<duration_t>(delta_t).count();
        const auto h2 = h / value_type{ 2 };
        const auto h_ = std::array{ value_type{ 0 }, h2, h2, h };

        for (std::size_t j = 0; j != O + 1; ++j)
        {
            for (auto i = decltype(size_){ 0 }; i != size_; ++i)
            {
                if (j == 0)
                {
                    particle_buffer_[j][i].mass()     = particles_[i].mass();
                    particle_buffer_[j][i].position() = particles_[i].position();
                    particle_buffer_[j][i].velocity() = particles_[i].velocity();
                }
                else if (j != O)
                {
                    particle_buffer_[j - 1][i].acceleration() =
                        interaction.get_acceleration(i, particle_buffer_[j - 1]);
                    particle_buffer_[j][i].position() =
                        particle_buffer_[0][i].position() +
                        h_[j] * particle_buffer_[j - 1][i].velocity();
                    particle_buffer_[j][i].velocity() =
                        particle_buffer_[0][i].velocity() +
                        h_[j] * particle_buffer_[j - 1][i].acceleration();
                }
                else
                {
                    particle_buffer_[j - 1][i].acceleration() =
                        interaction.get_acceleration(i, particle_buffer_[j - 1]);
                }
            }
        }
        for (auto i = decltype(size_){ 0 }; i != size_; ++i)
        {
            particles_[i].position() +=
                h / value_type{ 6 } *
                (particle_buffer_[0][i].velocity() +
                 value_type{ 2 } * particle_buffer_[1][i].velocity() +
                 value_type{ 2 } * particle_buffer_[2][i].velocity() +
                 particle_buffer_[3][i].velocity());
            particles_[i].velocity() +=
                h / value_type{ 6 } *
                (particle_buffer_[0][i].acceleration() +
                 value_type{ 2 } * particle_buffer_[1][i].acceleration() +
                 value_type{ 2 } * particle_buffer_[2][i].acceleration() +
                 particle_buffer_[3][i].acceleration());
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

} // namespace pm::interaction
