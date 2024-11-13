#pragma once

#include "error_handling.hpp"
#include <algorithm>
#include <concepts>
#include <random>
#include <type_traits>
#include <utility>

namespace generators
{

template <typename Generator_Type>
    requires requires {
        typename Generator_Type::result_type;
        typename Generator_Type::param_type;
        {
            std::declval<Generator_Type&>()()
        } -> std::same_as<typename Generator_Type::result_type>;
    }
class value_generator
{
public:
    using generator_impl_t = Generator_Type;
    using param_type       = typename generator_impl_t::param_type;
    using result_type      = typename generator_impl_t::result_type;

    explicit value_generator(
        param_type   params,
        unsigned int seed = std::random_device{}()
    ) noexcept :
        gen_(params, seed)
    {
    }

    [[nodiscard]]
    auto yield() noexcept(noexcept(std::declval<generator_impl_t&>())) -> result_type
    {
        return gen_();
    }

private:
    generator_impl_t gen_;
};

} // namespace generators
