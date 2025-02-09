#pragma once

#include "generics.hpp"
#include "logging.hpp"
#include "particle_concepts.hpp"
#include <boost/program_options.hpp>
#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace simulation::config
{

enum struct SimulationType
{
    _none_,
    barnes_hut,
    brute_force
};

namespace detail
{

[[nodiscard]]
inline auto simulation_type_parse(std::string_view sim_type) -> SimulationType
{
    using namespace std::literals;
    static const std::unordered_map<std::string_view, SimulationType> map{
        { "barnes_hut"sv, SimulationType::barnes_hut },
        { "brute_force"sv, SimulationType::brute_force }
    };
    return map.at(sim_type);
}

[[nodiscard]]
inline auto simulation_type_to_str(SimulationType sim_type) -> std::string_view
{
    using namespace std::literals;
    static const std::unordered_map<SimulationType, std::string_view> map{
        { SimulationType::barnes_hut, "barnes_hut"sv },
        { SimulationType::brute_force, "brute_force"sv }
    };
    return map.at(sim_type);
}

} // namespace detail

template <pm::particle_concepts::Particle Particle_Type>
struct universe_config
{
    using value_type = typename Particle_Type::value_type;

    auto is_valid() const noexcept -> bool
    {
        if (universe_radius_ <= 0)
        {
            utility::logging::default_source::log(
                utility::logging::severity_level::error,
                "Universe radius must be positive.\n"
            );
            return false;
        }
        return true;
    }

    auto print() const noexcept -> void
    {
        std::cout << "Universe Config:\n"
                  << "\tUniverse Radius: " << universe_radius_ << "\n";
    }

    value_type universe_radius_;
};

template <pm::particle_concepts::Particle Particle_Type>
struct physics_config
{
    using value_type = typename Particle_Type::value_type;

    auto is_valid() const noexcept -> bool
    {
        if (gravitational_constant_.has_value() && gravitational_constant_.value() <= 0)
        {
            utility::logging::default_source::log(
                utility::logging::severity_level::error,
                "Gravitational constant must be positive.\n"
            );
            return false;
        }
        return true;
    }

    auto print() const noexcept -> void
    {
        std::cout << "Physics Config:\n"
                  << "\tGravitational Constant: "
                  << (gravitational_constant_.has_value()
                          ? std::to_string(gravitational_constant_.value())
                          : "Default")
                  << "\n";
    }

    std::optional<value_type> gravitational_constant_;
};

template <pm::particle_concepts::Particle Particle_Type>
struct simulation_common_config
{
    using value_type = typename Particle_Type::value_type;
    using size_type  = std::size_t;
    using duration_t = std::chrono::duration<value_type>;

    [[nodiscard]]
    auto is_valid() const noexcept -> bool
    {
        if (dt_ <= duration_t::zero())
        {
            utility::logging::default_source::log(
                utility::logging::severity_level::error,
                "Time step (dt) must be positive.\n"
            );
            return false;
        }
        if (duration_ < dt_)
        {
            utility::logging::default_source::log(
                utility::logging::severity_level::error,
                "Duration must be greater than the time step (dt).\n"
            );
            return false;
        }
        if (particle_count_ <= 0)
        {
            utility::logging::default_source::log(
                utility::logging::severity_level::error,
                "Particle count must be greater than zero.\n"
            );
            return false;
        }
        return true;
    }

    auto print() const noexcept -> void
    {
        std::cout << "General Config:\n"
                  << "\tTime Step (dt): " << dt_.count() << " seconds\n"
                  << "\tDuration: " << duration_.count() << " seconds\n"
                  << "\tParticle Count: " << particle_count_ << "\n"
                  << "\tSimulation Type: " << detail::simulation_type_to_str(sim_type_)
                  << "\n";
    }

    duration_t     dt_;
    duration_t     duration_;
    size_type      particle_count_;
    SimulationType sim_type_;
};

template <pm::particle_concepts::Particle Particle_Type>
struct brute_force_specific_config
{
    [[nodiscard]]
    auto is_valid() const noexcept -> bool
    {
        return true;
    }

    auto print() const noexcept -> void
    {
    }
};

template <pm::particle_concepts::Particle Particle_Type>
struct barnes_hut_specific_config
{
    using value_type = simulation_common_config<Particle_Type>::value_type;
    using size_type  = simulation_common_config<Particle_Type>::size_type;
    using depth_t    = unsigned int;
    inline static constexpr auto s_theta_range =
        utility::generics::interval{ value_type{ 0 }, value_type{ 1 } };

    [[nodiscard]]
    auto is_valid() const noexcept -> bool
    {
        if (tree_max_depth_ <= 0)
        {
            utility::logging::default_source::log(
                utility::logging::severity_level::error,
                "ndTree max depth must be greater than 0.\n"
            );
            return false;
        }
        if (tree_box_capacity_ <= 0)
        {
            utility::logging::default_source::log(
                utility::logging::severity_level::error,
                "ndTree box capacity must be greater than 0.\n"
            );
            return false;
        }
        if (!utility::generics::in(theta_, s_theta_range))
        {
            utility::logging::default_source::log(
                utility::logging::severity_level::error,
                "Barnes Hut approximation prameter theta must be in the range [0, 1].\n"
            );
            return false;
        }
        return true;
    }

    auto print() const noexcept -> void
    {
        std::cout << "Barnes-Hut Specific Config:\n"
                  << "\tTree Max Depth: " << tree_max_depth_ << "\n"
                  << "\tTree Box Capacity: " << tree_box_capacity_ << "\n"
                  << "\tTheta: " << theta_ << "\n";
    }

    depth_t    tree_max_depth_;
    size_type  tree_box_capacity_;
    value_type theta_;
};

template <pm::particle_concepts::Particle Particle_Type>
struct simulation_config
{
    using value_type        = typename Particle_Type::value_type;
    using base_config_t     = simulation_common_config<Particle_Type>;
    using bf_config_t       = brute_force_specific_config<Particle_Type>;
    using bh_config_t       = barnes_hut_specific_config<Particle_Type>;
    using physics_config_t  = physics_config<Particle_Type>;
    using universe_config_t = universe_config<Particle_Type>;

    [[nodiscard]]
    auto is_valid() const noexcept -> bool
    {
        return physics_config_.is_valid() && universe_config_.is_valid() &&
               simulation_general_config_.is_valid() &&
               std::visit(
                   [this](auto&& cfg) { return cfg.is_valid(); },
                   simulation_specific_config_
               );
    }

    auto print() const noexcept -> void
    {
        physics_config_.print();
        universe_config_.print();
        simulation_general_config_.print();
        std::visit([this](auto&& cfg) { cfg.print(); }, simulation_specific_config_);
    }

    [[nodiscard]]
    inline auto general_config() const noexcept -> base_config_t const&
    {
        return simulation_general_config_;
    }

    [[nodiscard]]
    inline auto barnes_hut_config() const noexcept -> bh_config_t const&
    {
        return std::get<bh_config_t>(simulation_specific_config_);
    }

    physics_config_t                       physics_config_;
    universe_config_t                      universe_config_;
    base_config_t                          simulation_general_config_;
    std::variant<bf_config_t, bh_config_t> simulation_specific_config_;
};

template <pm::particle_concepts::Particle Particle_Type>
auto parse_config(std::string const& file_path) -> simulation_config<Particle_Type>
{
    using value_type = typename Particle_Type::value_type;
    namespace po     = boost::program_options;
    // Define options for different sections
    po::options_description general_desc("General Configuration");
    general_desc
        .add_options()("GeneralConfig.dt", po::value<value_type>(), "Time step (dt)")("GeneralConfig.duration", po::value<value_type>(), "Simulation duration")("GeneralConfig.particle_count", po::value<std::size_t>(), "Number of particles")(
            "GeneralConfig.simulation_type",
            po::value<std::string>(),
            "Simulation approximaton type"
        );

    po::options_description physics_desc("Physics Configuration");
    physics_desc.add_options()(
        "PhysicsConfig.gravitational_constant",
        po::value<value_type>(),
        "Gravitational constant"
    );

    po::options_description universe_desc("Universe Configuration");
    physics_desc.add_options()(
        "UniverseConfig.universe_radius", po::value<value_type>(), "Universe radius"
    );

    po::options_description barnes_hut_desc("Barnes-Hut Configuration");
    barnes_hut_desc
        .add_options()("BarnesHutConfig.tree_max_depth", po::value<unsigned int>(), "Max tree depth")("BarnesHutConfig.tree_box_capacity", po::value<std::size_t>(), "Box capacity")(
            "BarnesHutConfig.theta", po::value<value_type>(), "Theta parameter"
        );

    po::options_description all_desc;
    all_desc.add(general_desc).add(physics_desc).add(barnes_hut_desc);

    // Parse the configuration file
    po::variables_map vm;
    std::ifstream     config_file(file_path);
    if (!config_file.is_open())
    {
        std::cerr << "Unable to open configuration file: " << file_path;
        std::terminate();
    }

    po::store(po::parse_config_file(config_file, all_desc), vm);
    po::notify(vm);

    simulation_config<Particle_Type> config{};

    if (vm.count("GeneralConfig.dt"))
    {
        config.simulation_general_config_.dt_ =
            typename simulation_common_config<Particle_Type>::duration_t(
                vm["GeneralConfig.dt"].as<value_type>()
            );
    }
    if (vm.count("GeneralConfig.duration"))
    {
        config.simulation_general_config_.duration_ =
            typename simulation_common_config<Particle_Type>::duration_t(
                vm["GeneralConfig.duration"].as<value_type>()
            );
    }
    if (vm.count("GeneralConfig.particle_count"))
    {
        config.simulation_general_config_.particle_count_ =
            vm["GeneralConfig.particle_count"]
                .as<typename simulation_common_config<Particle_Type>::size_type>();
    }
    if (vm.contains("GeneralConfig.simulation_type"))
    {
        config.simulation_general_config_.sim_type_ = detail::simulation_type_parse(
            vm["GeneralConfig.simulation_type"].as<std::string>()
        );
    }

    if (vm.count("PhysicsConfig.gravitational_constant"))
    {
        config.physics_config_.gravitational_constant_ =
            vm["PhysicsConfig.gravitational_constant"]
                .as<typename physics_config<Particle_Type>::value_type>();
    }

    if (vm.count("UniverseConfig.universe_radius"))
    {
        config.universe_config_.universe_radius_ =
            vm["UniverseConfig.universe_radius"]
                .as<typename universe_config<Particle_Type>::value_type>();
    }

    if (config.simulation_general_config_.sim_type_ == SimulationType::barnes_hut)
    {
        barnes_hut_specific_config<Particle_Type> bh_config{};
        if (vm.count("BarnesHutConfig.tree_max_depth"))
        {
            bh_config.tree_max_depth_ =
                vm["BarnesHutConfig.tree_max_depth"]
                    .as<typename barnes_hut_specific_config<Particle_Type>::depth_t>();
        }
        if (vm.count("BarnesHutConfig.tree_box_capacity"))
        {
            bh_config.tree_box_capacity_ =
                vm["BarnesHutConfig.tree_box_capacity"]
                    .as<typename barnes_hut_specific_config<Particle_Type>::size_type>();
        }
        if (vm.count("BarnesHutConfig.theta"))
        {
            bh_config.theta_ =
                vm["BarnesHutConfig.theta"]
                    .as<typename barnes_hut_specific_config<Particle_Type>::value_type>();
        }

        config.simulation_specific_config_ = bh_config;
    }

    return config;
}

} // namespace simulation::config
