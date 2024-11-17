#pragma once

#include "particle.hpp"
#include "physical_magnitudes.hpp"

auto create_solar_system()
{
    static constexpr auto s_dimension = 3;
    using namespace pm;
    auto earth = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 5.972e24 },
        magnitudes::position<s_dimension, double>{ 152.1e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 29290, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto mercury = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 0.33e24 },
        magnitudes::position<s_dimension, double>{ 69.82e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 47400, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto venus = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 4.87e24 },
        magnitudes::position<s_dimension, double>{ 108.9e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 35000, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto mars = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 0.64e24 },
        magnitudes::position<s_dimension, double>{ 249.3e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 24100, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto jupiter = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 1898e24 },
        magnitudes::position<s_dimension, double>{ 816.4e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 13100, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto saturn = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 568e24 },
        magnitudes::position<s_dimension, double>{ 1506.5e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 9700, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto neptune = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 102e24 },
        magnitudes::position<s_dimension, double>{ 4558.9e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 5400, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto uranus = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 86.8e24 },
        magnitudes::position<s_dimension, double>{ 3001.4e6, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 6800, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    auto sun = particle::ndparticle<s_dimension, double>{
        magnitudes::mass<double>{ 1.989e30 },
        magnitudes::position<s_dimension, double>{ 0, 0, 0 },
        magnitudes::linear_velocity<s_dimension, double>{ 0, 0, 0 },
        magnitudes::linear_acceleration<s_dimension, double>{ 0, 0, 0 }
    };
    return std::vector<particle::ndparticle<s_dimension, double>>{
        sun, mercury, venus, earth, mars, saturn, jupiter, uranus, neptune
    };
}
