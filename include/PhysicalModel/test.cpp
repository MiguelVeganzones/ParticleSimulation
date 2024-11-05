#include "unit_system.hpp"
#include <iostream>

// Example usage
int main()
{
    using namespace units;

    // Define length and time unit groups using unit_power
    using meters         = unit_power<BaseUnit::m, 1>;
    using kilograms      = unit_power<BaseUnit::kg, 1>;
    using kilograms_1    = unit_power<BaseUnit::kg, -1>;
    using seconds_sq     = unit_power<BaseUnit::s, 2>;
    using seconds_div_sq = unit_power<BaseUnit::s, -2>;
    using length         = composite_unit<meters>;
    using time           = composite_unit<unit_power<BaseUnit::s, 1>>;
    using mskg2 = composite_unit<meters, kilograms_1, seconds_sq, seconds_div_sq>;
    using mskg  = composite_unit<meters, kilograms, seconds_sq>;

    std::cout << meters{} << '\n';
    std::cout << time{} << '\n';
    std::cout << mskg{} << '\n';
    std::cout << mskg2{} << '\n';
    std::cout << unit_merging_engine<meters, meters, seconds_sq>::type{} << '\n';
    std::cout << merge_powers_t<meters, meters, meters, seconds_sq, meters>{} << '\n';

    std::apply(
        []<typename... Ts>(Ts const&... tupleArgs) {
            std::cout << '[';
            std::size_t n{ 0 };
            ((std::cout << tupleArgs << (++n != sizeof...(Ts) ? " || " : "")), ...);
            std::cout << ']';
        },
        unique_types<
            unit_pack<meters, meters, meters, seconds_sq, kilograms, seconds_div_sq>>::
            type{}
    );

    std::cout << "\nCompile-time unit checking example." << std::endl;
    return 0;
}
