#include "barnes_hut_approximation.hpp"
#include "brute_force.hpp"
#include "factory.hpp"
#include "logging.hpp"
#include "particle.hpp"
#include "particle_interaction.hpp"
#include "random_distributions.hpp"
#include "simulation_config.hpp"
#include <array>
#include <concepts>
#include <cstdlib>
#include <iostream>

#define SEED1 104845342
#define SEED2 982523355
#define SEED3 223857254

constexpr auto universe_radius = 100.0;

template <std::floating_point F>
auto generate_particle_pair()
{
    const F m = 1e8;
    return std::array{ pm::particle::ndparticle<1, F>(
                           pm::magnitudes::mass<F>{ m },
                           pm::magnitudes::position<1, F>{ -universe_radius },
                           pm::magnitudes::linear_velocity<1, F>{ 0 },
                           pm::magnitudes::linear_acceleration<1, F>{ 0 }
                       ),
                       pm::particle::ndparticle<1, F>(
                           pm::magnitudes::mass<F>{ m },
                           pm::magnitudes::position<1, F>{ universe_radius },
                           pm::magnitudes::linear_velocity<1, F>{ 0 },
                           pm::magnitudes::linear_acceleration<1, F>{ 0 }
                       ) };
}

template <std::size_t N, std::floating_point F>
auto generate_particle_set(std::size_t size)
{
    using namespace pm::factory;
    using namespace utility::random_distributions;

    auto mass_generator = []() mutable -> F {
        using distribution_t = random_distribution<F, DistributionCategory::Exponential>;
        using param_type     = typename distribution_t::param_type;
        static const param_type params(1.0);
        static distribution_t   d(params, SEED1);
        return d() * F{ 0.01 };
    };

    auto position_generator = []() mutable -> F {
        using distribution_a_t = random_distribution<F, DistributionCategory::Uniform>;
        using param_type_a     = typename distribution_a_t::param_type;
        static const param_type_a params_a(-universe_radius, universe_radius);
        static distribution_a_t   d_a(params_a, SEED2);
        using distribution_b_t = random_distribution<F, DistributionCategory::Gamma>;
        using param_type_b     = typename distribution_b_t::param_type;
        static const param_type_b params_b(F{ 1 }, F{ 1 });
        static distribution_b_t   d_b(params_b, SEED3);
        return d_a(); // + universe_radius;
    };

    auto velocity_generator = []() -> F { return F{ 0 }; };

    return particle_set_factory<N, F>(
        size, mass_generator, position_generator, velocity_generator
    );
}

template <std::size_t N, std::floating_point F>
auto generate_charged_particle_set(std::size_t size)
{
    using namespace pm::factory;
    using namespace utility::random_distributions;

    auto charge_generator = []() mutable -> F {
        using distribution_c_t = random_distribution<F, DistributionCategory::Uniform>;
        using param_type_c     = typename distribution_c_t::param_type;
        static const param_type_c params(F{ -1e-6 }, F{ 1e-6 });
        static distribution_c_t   d_c(params);
        return d_c();
    };

    auto mass_generator = []() mutable -> F {
        using distribution_t = random_distribution<F, DistributionCategory::Exponential>;
        using param_type     = typename distribution_t::param_type;
        static const param_type params(0.001);
        static distribution_t   d(params);
        return d() * F{ 100 };
    };

    auto position_generator = []() mutable -> F {
        using distribution_a_t = random_distribution<F, DistributionCategory::Uniform>;
        using param_type_a     = typename distribution_a_t::param_type;
        static const param_type_a params_a(-universe_radius, universe_radius);
        static distribution_a_t   d_a(params_a);
        using distribution_b_t = random_distribution<F, DistributionCategory::Gamma>;
        using param_type_b     = typename distribution_b_t::param_type;
        static const param_type_b params_b(F{ 1 }, F{ 1 });
        static distribution_b_t   d_b(params_b);
        return d_a(); // + universe_radius;
    };

    auto velocity_generator = []() -> F { return F{ 0 }; };

    return particle_set_factory<N, F>(
        size, mass_generator, position_generator, velocity_generator, charge_generator
    );
}

int barnes_hut_bench()
{
    using namespace pm;
    using F                    = double;
    static constexpr auto N    = 3;
    using particle_t           = particle::ndparticle<N, F>;
    constexpr auto interaction = pm::interaction::InteractionType::Gravitational;

#ifdef NDEBUG
    const auto config_file_path = "data/input/release/config.ini";
#else
    const auto config_file_path = "data/input/debug/config.ini";
#endif

    const auto config    = simulation::config::parse_config<particle_t>(config_file_path);
    const auto size      = config.general_config().particle_count_;
    auto       particles = generate_particle_set<N, F>(size);

    assert(config.is_valid());
    config.print();
    if (config.physics_config_.gravitational_constant_.has_value())
    {
        pm::physical_parameters<F>::set_gravitational_constant(
            config.physics_config_.gravitational_constant_.value()
        );
    }

    simulation::bh_approx::barnes_hut_approximation<particle_t, interaction> simulation_a(
        particles, config.general_config(), config.barnes_hut_config()
    );

    std::cout << "Simulation A\n";
    simulation_a.run();
    std::cout << "Done\n";

    return EXIT_SUCCESS;
}

int brute_force_bench()
{
    using namespace pm;
    using F                    = double;
    static constexpr auto N    = 3;
    using particle_t           = particle::ndparticle<N, F>;
    constexpr auto interaction = pm::interaction::InteractionType::Gravitational;

#ifdef NDEBUG
    const auto config_file_path = "data/input/release/config.ini";
#else
    const auto config_file_path = "data/input/debug/config.ini";
#endif

    const auto config    = simulation::config::parse_config<particle_t>(config_file_path);
    const auto size      = config.general_config().particle_count_;
    auto       particles = generate_particle_set<N, F>(size);

    assert(config.is_valid());
    config.print();
    if (config.physics_config_.gravitational_constant_.has_value())
    {
        pm::physical_parameters<F>::set_gravitational_constant(
            config.physics_config_.gravitational_constant_.value()
        );
    }

    simulation::bf::brute_force_computation<particle_t, interaction> simulation_a(
        particles, config.general_config()
    );

    std::cout << "Simulation A\n";
    simulation_a.run();
    std::cout << "Done\n";

    return EXIT_SUCCESS;
}

int barnes_hut_test()
{
    using namespace pm;
    using F                    = double;
    static constexpr auto N    = 3;
    using particle_t           = particle::ndparticle<N, F>;
    constexpr auto interaction = pm::interaction::InteractionType::Gravitational;

#ifdef NDEBUG
    const auto config_file_path = "data/input/release/config.ini";
#else
    const auto config_file_path = "data/input/debug/config.ini";
#endif

    const auto config    = simulation::config::parse_config<particle_t>(config_file_path);
    const auto size      = config.general_config().particle_count_;
    auto       particles = generate_particle_set<N, F>(size);

    assert(config.is_valid());
    config.print();
    if (config.physics_config_.gravitational_constant_.has_value())
    {
        pm::physical_parameters<F>::set_gravitational_constant(
            config.physics_config_.gravitational_constant_.value()
        );
    }

    simulation::bh_approx::barnes_hut_approximation<particle_t, interaction> simulation_a(
        particles, config.general_config(), config.barnes_hut_config()
    );

    std::cout << "Simulation A\n";
    simulation_a.run();

    simulation::bf::brute_force_computation<particle_t, interaction> simulation_b(
        particles, config.general_config()
    );

    std::cout << "Simulation B\n";
    simulation_b.run();

    return EXIT_SUCCESS;
}

int electrostatic_test()
{
    using namespace pm;
    using F                    = double;
    static constexpr auto N    = 3;
    using particle_t           = particle::ndparticle<N, F>;
    constexpr auto interaction = pm::interaction::InteractionType::Electrostatic;

#if NDEBUG
    const auto config_file_path = "data/input/release/config.ini";
#else
    const auto config_file_path = "data/input/debug/config.ini";
#endif

    const auto config    = simulation::config::parse_config<particle_t>(config_file_path);
    const auto size      = config.general_config().particle_count_;
    auto       particles = generate_charged_particle_set<N, F>(size);

    assert(config.is_valid());
    config.print();

    simulation::bf::brute_force_computation<particle_t, interaction> simulation(
        particles, config.general_config()
    );

    std::cout << "Electrostatic Simulation\n";
    simulation.run();

    return EXIT_SUCCESS;
}

int main()
{
#ifdef USE_ROOT_PLOTTING
    TApplication app = TApplication("Root app", 0, nullptr);
#endif
    utility::logging::default_source::log(
        utility::logging::severity_level::info, "Inside main function."
    );
    utility::logging::default_source::log(
        utility::logging::severity_level::error, "Huge error or sth..."
    );
    // barnes_hut_bench();
    brute_force_bench();
    // electrostatic_test();

#ifdef USE_ROOT_PLOTTING
    app.Run();
#endif

    utility::logging::default_source::log(
        utility::logging::severity_level::info, "Main function terminated."
    );
    return EXIT_SUCCESS;
}
