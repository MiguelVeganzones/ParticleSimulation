#pragma once

#include "particle.hpp"
#include "physical_magnitudes.hpp"
#include <ranges>
#include <type_traits>
#include <vector>

namespace pm::factory
{

template <std::size_t N, std::floating_point F, auto U, typename Fn, typename... Args>
    requires std::
        is_invocable_r_v<F, std::remove_reference_t<Fn>, std::remove_reference_t<Args>...>
    [[nodiscard]]
    auto physical_magnitude_factory(Fn&& fn, Args&&... args) noexcept
    -> pm::magnitudes::physical_magnitude_t<N, F, U>
{
    using magnitude_t = magnitudes::physical_magnitude_t<N, F, U>;
    magnitude_t ret{};
    for (auto& e : ret)
    {
        e = std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
    return ret;
}

template <
    std::size_t         N,
    std::floating_point F,
    typename Mass_Generator,
    typename Pos_Generator,
    typename Vel_Generator,
    typename Acc_Generator>
    requires std::is_invocable_r_v<F, Mass_Generator> &&
             std::is_invocable_r_v<F, Vel_Generator> &&
             std::is_invocable_r_v<F, Pos_Generator> &&
             std::is_invocable_r_v<F, Acc_Generator>
[[nodiscard]]
auto particle_set_factory(
    std::size_t      size,
    Mass_Generator&& mass_gen,
    Pos_Generator&&  pos_gen,
    Vel_Generator&&  vel_gen,
    Acc_Generator&&  acc_gen
) noexcept -> std::vector<pm::particle::ndparticle<N, F>>
{
    using particle_t  = pm::particle::ndparticle<N, F>;
    using container_t = std::vector<particle_t>;
    container_t ret{};
    ret.reserve(size);

    for ([[maybe_unused]]
         auto _ : std::views::iota(0uz, size))
        ret.push_back(particle_t(
            physical_magnitude_factory<1, F, units::Units::kg>(
                std::forward<Mass_Generator>(mass_gen)
            ),
            physical_magnitude_factory<N, F, units::Units::m>(
                std::forward<Pos_Generator>(pos_gen)
            ),
            physical_magnitude_factory<N, F, units::Units::m_s>(
                std::forward<Vel_Generator>(vel_gen)
            ),
            physical_magnitude_factory<N, F, units::Units::m_s2>(
                std::forward<Acc_Generator>(acc_gen)
            )
        ));
    return ret;
}

} // namespace pm::factory
