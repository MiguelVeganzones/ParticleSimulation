#pragma once
#include "physical_magnitudes.hpp"
#include <concepts>

namespace pm::magnitudes
{

template <std::size_t N, std::floating_point F>
using position = physical_magnitude_t<N, F, units::Units::m>;
template <std::size_t N, std::floating_point F>
using distance = physical_magnitude_t<N, F, units::Units::m>;
template <std::size_t N, std::floating_point F>
using linear_velocity = physical_magnitude_t<N, F, units::Units::m_s>;
template <std::size_t N, std::floating_point F>
using linear_acceleration = physical_magnitude_t<N, F, units::Units::m_s2>;
template <std::size_t N, std::floating_point F>
using angular_position = physical_magnitude_t<N, F, units::Units::rad>;
template <std::size_t N, std::floating_point F>
using angular_velocity = physical_magnitude_t<N, F, units::Units::rad_s>;
template <std::size_t N, std::floating_point F>
using angular_acceleration = physical_magnitude_t<N, F, units::Units::rad_s2>;
template <std::floating_point F>
using mass = physical_magnitude_t<1, F, units::Units::kg>;
template <std::size_t N, std::floating_point F>
using force = physical_magnitude_t<N, F, units::Units::newton>;
template <std::size_t N, std::floating_point F>
using energy = physical_magnitude_t<1, F, units::Units::joule>;
template <std::size_t N, std::floating_point F>
using runtime_unit = physical_magnitude_t<N, F, units::Units::_runtime_unit_>;

} // namespace pm::magnitudes
