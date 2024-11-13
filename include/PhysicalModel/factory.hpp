#pragma once

#include "generator.hpp"
#include "particle.hpp"
#include "physical_magnitudes.hpp"
#include "random.hpp"
#include "random_distributions.hpp"
#include <type_traits>

namespace pm::factory
{

template <std::size_t N, std::floating_point F>
struct particle_config
{
};

template <std::size_t N, std::floating_point F, auto U, typename Fn, typename... Args>
    requires std::is_invocable_r_v<
                 F,
                 std::remove_reference_t<Fn>,
                 std::remove_reference_t<Args>...>
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

template <std::size_t N, std::floating_point F, typename Fn, typename... Args>
    requires std::is_invocable_r_v<
                 F,
                 std::remove_reference_t<Fn>,
                 std::remove_reference_t<Args>...>
[[nodiscard]]
auto particle_factory(Fn&& fn, Args&&... args) noexcept -> pm::particle::ndparticle<N, F>
{
    using particle_t = pm::particle::ndparticle<N, F>;
    return particle_t(
        physical_magnitude_factory<1, F, units::Units::kg>(
            std::forward<Fn>(fn), std::forward<Args>(args)...
        ),
        physical_magnitude_factory<N, F, units::Units::m>(
            std::forward<Fn>(fn), std::forward<Args>(args)...
        ),
        physical_magnitude_factory<N, F, units::Units::m_s>(
            std::forward<Fn>(fn), std::forward<Args>(args)...
        ),
        physical_magnitude_factory<N, F, units::Units::m_s2>(
            std::forward<Fn>(fn), std::forward<Args>(args)...
        )
    );
}

} // namespace pm::factory
