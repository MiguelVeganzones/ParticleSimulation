#pragma once

#include "particle.hpp"
#include "physical_magnitudes.hpp"

auto create_solar_system()
{
    static constexpr auto s_dimension = 3;
    using namespace pm;
    auto earth = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 5.972e24 },
        magnitudes::position<s_dimension, double>{ 149.6e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 29780, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto sun = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 1.989e30 },
        magnitudes::position<s_dimension, double>{ 0, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 0, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    return std::vector<particle::ndparticle<s_dimension, double>>{ sun, earth };
}
