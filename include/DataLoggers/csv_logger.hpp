#pragma once

#include "particle_concepts.hpp"
#include <fstream>
#include <iostream>
#include <ranges>
#ifdef USE_UNIT_SYSTEM
#include "unit_system.hpp"
#endif

namespace logger::csv
{

auto helper_write_to_csv(
    std::ranges::input_range auto const& particles,
    const std::string&                   filename
) -> void
    requires pm::particle_concepts::Particle<
        std::ranges::range_value_t<decltype(particles)>>
{
    using sample_t           = std::ranges::range_value_t<decltype(particles)>;
    constexpr auto dimension = sample_t::s_dimension;
    constexpr auto delimiter = ", ";

    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "ID" << delimiter;
    file << "Mass"
#ifdef USE_UNIT_SYSTEM
         << " [" << pm::units::repr<sample_t::mass_t::s_units>() << ']'
#endif
         << delimiter;
    for (auto i = decltype(dimension){ 0 }; i != dimension; ++i)
    {
        file << "pos_" << i
#ifdef USE_UNIT_SYSTEM
             << " [" << pm::units::repr<sample_t::position_t::s_units>() << ']'
#endif
             << delimiter;
    }
    for (auto i = decltype(dimension){ 0 }; i != dimension; ++i)
    {
        file << "vel_" << i
#ifdef USE_UNIT_SYSTEM
             << " [" << pm::units::repr<sample_t::velocity_t::s_units>() << ']'
#endif
             << delimiter;
    }
    file << '\n';

    for (auto const& p : particles)
    {
        file << p.id() << delimiter;
        file << p.mass().magnitude() << delimiter;
        for (auto v : p.position())
        {
            file << v << delimiter;
        }
        for (auto v : p.velocity())
        {
            file << v << delimiter;
        }
        file << '\n';
    }

    file.close();
    if (!file)
    {
        std::cerr << "Error writing to file " << filename << std::endl;
    }
    else
    {
        std::cout << "Data successfully saved to " << filename << std::endl;
    }
}

auto write_to_csv(
    std::ranges::input_range auto const& particles,
    const std::string&                   baseFilename
) -> void
{
    std::string filename = "./data/output/" + baseFilename;
    helper_write_to_csv(particles, filename);
}

} // namespace logger::csv
