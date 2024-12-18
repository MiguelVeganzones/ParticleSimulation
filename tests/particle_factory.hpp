#pragma once

#include "factory.hpp"
#include "random_distributions.hpp"

namespace particle_factory
{

template <std::size_t N, std::floating_point F>
auto generate_particle_set(std::size_t size, F universe_radius)
{
    using namespace pm::factory;
    using namespace utility::random_distributions;

    auto mass_generator = []() mutable -> F {
        using distribution_t = random_distribution<F, DistributionCategory::Exponential>;
        using param_type     = typename distribution_t::param_type;
        const param_type      params(0.001);
        static distribution_t d(params);
        return d() * F{ 100 };
    };

    auto position_generator = [universe_radius]() mutable -> F {
        using distribution_a_t = random_distribution<F, DistributionCategory::Uniform>;
        using param_type_a     = typename distribution_a_t::param_type;
        const param_type_a      params_a(-universe_radius, universe_radius);
        static distribution_a_t d_a(params_a);
        using distribution_b_t = random_distribution<F, DistributionCategory::Gamma>;
        using param_type_b     = typename distribution_b_t::param_type;
        const param_type_b      params_b(F{ 1 }, F{ 1 });
        static distribution_b_t d_b(params_b);
        return d_a(); // + universe_radius;
    };

    auto velocity_generator = []() -> F { return F{ 0 }; };

    return particle_set_factory<N, F>(
        size, mass_generator, position_generator, velocity_generator
    );
}

} // namespace particle_factory
