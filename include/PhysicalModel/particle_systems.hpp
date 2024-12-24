#pragma once

#include "particle.hpp"
#include "physical_magnitudes.hpp"

namespace pm::particle_systems
{

template <std::floating_point F>
[[nodiscard, gnu::const]]
auto create_solar_system() -> std::vector<pm::particle::ndparticle<3, F>>
{
    using namespace pm;
    constexpr auto N  = 3;
    using container_t = std::vector<particle::ndparticle<N, F>>;

    const auto earth =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 5.972e24 } },
                                    magnitudes::position<N, F>{ F{ 152.1e6 }, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 29290, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    const auto mercury =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 0.33e24 } },
                                    magnitudes::position<N, F>{ F{ 69.82e6 }, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 47400, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    const auto venus =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 4.87e24 } },
                                    magnitudes::position<N, F>{ F{ 108.9e6 }, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 35000, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    const auto mars =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 0.64e24 } },
                                    magnitudes::position<N, F>{ F{ 249.3e6 }, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 24100, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    const auto jupiter =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 1898e24 } },
                                    magnitudes::position<N, F>{ F{ 816.4e6 }, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 13100, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    const auto saturn =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 568e24 } },
                                    magnitudes::position<N, F>{ F{ 1506.5e6 }, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 9700, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    const auto neptune =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 102e24 } },
                                    magnitudes::position<N, F>{ F{ 4558.9e6 }, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 5400, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    auto uranus =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 86.8e24 } },
                                    magnitudes::position<N, F>{ F{ 3001.4e6 }, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 6800, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    auto sun =
        particle::ndparticle<N, F>{ magnitudes::mass<F>{ F{ 1.989e30 } },
                                    magnitudes::position<N, F>{ 0, 0, 0 },
                                    magnitudes::linear_velocity<N, F>{ 0, 0, 0 },
                                    magnitudes::linear_acceleration<N, F>{ 0, 0, 0 } };
    return container_t{
        sun, mercury, venus, earth, mars, saturn, jupiter, uranus, neptune
    };
}

} // namespace pm::particle_systems
