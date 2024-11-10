#pragma once

#include <type_traits>

namespace pm
{

template <std::floating_point F>
struct physical_constants
{
    static_assert(std::is_same_v<F, double>);
    inline static constexpr auto G = static_cast<F>(0.000000000066743);
};

} // namespace pm
