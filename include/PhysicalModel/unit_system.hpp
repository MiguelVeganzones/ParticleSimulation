#ifndef INCLUDED_UNIT_SYSTEM
#define INCLUDED_UNIT_SYSTEM

#include <iostream>
#include <tuple>
#include <type_traits>

namespace units
{

enum class BaseUnit
{
    s,
    kg,
    m,
    rad,  // Radians are adimensional... Look away
    adim, // Adimensional
};

template <BaseUnit Unit, int Power>
struct unit_power
{
    static constexpr BaseUnit s_unit  = Unit;
    static constexpr int      s_power = Power;
};

template <typename... Units>
struct composite_unit
{
    inline static constexpr auto s_units = std::tuple<Units...>{};
};

template <typename U1, typename U2>
struct is_same_unit : std::false_type
{
};

template <BaseUnit Unit, int Power1, int Power2>
struct is_same_unit<unit_power<Unit, Power1>, unit_power<Unit, Power2>> : std::true_type
{
};

template <typename Unit, typename... Other_Units>
struct in_parameter_pack
{
    inline static constexpr auto value =
        std::disjunction_v<in_parameter_pack<Unit, Other_Units>...>;
};

template <typename Unit, typename Other_Unit>
struct in_parameter_pack<Unit, Other_Unit> : is_same_unit<Unit, Other_Unit>
{
};

template <std::size_t Idx, typename Head, typename... Rest>
    requires(Idx >= 0 && Idx <= sizeof...(Rest))
struct index_impl
{
    using type =
        std::conditional_t<Idx == 0, Head, typename index_impl<Idx - 1, Rest...>::type>;
};

template <std::size_t Idx, typename... Units>
using index_at_t = typename index_impl<Idx, Units...>::type;

template <typename Unit, typename... Other_Units>
struct merge_powers_impl;

template <typename Unit, typename Other_Unit>
struct merge_powers_impl<Unit, Other_Unit>
{
    using type = typename std::conditional_t<
        is_same_unit<Unit, Other_Unit>::value,
        unit_power<Unit::s_unit, Other_Unit::s_power>,
        unit_power<Unit::s_unit, 0>>;
};

template <typename Unit, typename Other_Head, typename... Other_Rest>
struct merge_powers_impl<Unit, Other_Head, Other_Rest...>
{
    using type = unit_power<
        Unit::s_unit,
        merge_powers_impl<Unit, Other_Head>::type::s_power +
            merge_powers_impl<Unit, Other_Rest...>::type::s_power>;
};

template <typename Unit, typename Head, typename... Tail>
using merge_powers_t = typename merge_powers_impl<Unit, Head, Tail...>::type;

template <typename... Units>
struct unit_pack
{
    using type = std::tuple<Units...>;
};

template <typename Unit, typename Pack>
struct append_unique;

template <typename Unit, typename... Other_Units>
struct append_unique<Unit, unit_pack<Other_Units...>>
{
    using type = std::conditional_t<
        std::disjunction_v<is_same_unit<Unit, Other_Units>...>,
        unit_pack<Other_Units...>,
        unit_pack<Other_Units..., Unit>>;
};

template <typename T, typename Pack = unit_pack<>>
struct unique_types_impl
{
    using type = Pack;
};

template <typename Pack, typename T, typename... Ts>
struct unique_types_impl<unit_pack<T, Ts...>, Pack>
    : unique_types_impl<unit_pack<Ts...>, typename append_unique<T, Pack>::type>
{
};

template <typename Pack>
using unique_types = typename unique_types_impl<Pack>::type;

template <typename... Units>
struct unit_merging_engine
{
    using type = float;
};

template <BaseUnit Unit, int Power>
auto operator<<(std::ostream& os, unit_power<Unit, Power>) noexcept -> std::ostream&
{
    using up_t               = unit_power<Unit, Power>;
    constexpr auto unit_name = [](BaseUnit unit) noexcept -> std::string_view {
        switch (unit)
        {
        case BaseUnit::m: return "m";
        case BaseUnit::rad: return "rad";
        case BaseUnit::kg: return "kg";
        case BaseUnit::s: return "s";
        case BaseUnit::adim: return "";
        default: return "??";
        }
    };
    os << unit_name(up_t::s_unit);
    if constexpr (up_t::s_power != 1)
    {
        std::cout << '^' << up_t::s_power;
    }
    return os;
}

template <typename... Units>
auto operator<<(std::ostream& os, composite_unit<Units...>) noexcept -> std::ostream&
{
    using ug_t = composite_unit<Units...>;
    std::apply(
        [&os](auto... up) {
            os << '[';
            std::size_t n{ 0 };
            ((os << up << (++n != sizeof...(Units) ? " " : "")), ...);
            os << ']';
        },
        ug_t::s_units
    );
    return os;
}

} // namespace units

#endif // INCLUDED_UNIT_SYSTEM
