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

template <typename U1, typename U2>
struct is_same_unit : std::false_type
{
};

template <BaseUnit Unit, int Power1, int Power2>
struct is_same_unit<unit_power<Unit, Power1>, unit_power<Unit, Power2>> : std::true_type
{
};

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

template <typename Unit, typename... All_Units>
using merge_powers_t = typename merge_powers_impl<Unit, All_Units...>::type;

template <typename... Units>
struct unit_pack
{
    using type                          = std::tuple<Units...>;
    inline static constexpr auto s_size = sizeof...(Units);
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

template <typename Unique_Units, typename All_Units, typename Result>
struct merger
{
    using type = unit_pack<>;
};

template <
    typename Unique_Units_Head,
    typename... Unique_Units_Tail,
    typename... All_Units,
    typename... Result>
struct merger<
    unit_pack<Unique_Units_Head, Unique_Units_Tail...>,
    unit_pack<All_Units...>,
    unit_pack<Result...>>
{
    using merged_unit = merge_powers_t<Unique_Units_Head, All_Units...>;

    using type = typename merger<
        unit_pack<Unique_Units_Tail...>,
        unit_pack<All_Units...>,
        std::conditional_t<
            merged_unit::s_power != 0,
            typename append_unique<merged_unit, unit_pack<Result...>>::type,
            unit_pack<Result...>>>::type;
};

template <typename Unique_Units_Head, typename... All_Units, typename... Result>
struct merger<unit_pack<Unique_Units_Head>, unit_pack<All_Units...>, unit_pack<Result...>>
{
    using merged_unit = merge_powers_t<Unique_Units_Head, All_Units...>;
    using type        = std::conditional_t<
               merged_unit::s_power != 0,
               typename append_unique<merged_unit, unit_pack<Result...>>::type,
               unit_pack<Result...>>;
};

template <typename... Units>
using unit_merging_engine_t =
    merger<unique_types<unit_pack<Units...>>, unit_pack<Units...>, unit_pack<>>::type::
        type;

template <typename... Units>
struct composite_unit
{
    inline static constexpr std::tuple s_units = unit_merging_engine_t<Units...>{};
    inline static constexpr auto       s_size  = std::tuple_size_v<decltype(s_units)>;
};

template <typename, typename>
struct is_same_composite_unit : std::false_type
{
};

template <typename... Units1, typename... Units2>
struct is_same_composite_unit<composite_unit<Units1...>, composite_unit<Units2...>>
{
    using units_type_1 = decltype(composite_unit<Units1...>::s_units);
    using units_type_2 = decltype(composite_unit<Units2...>::s_units);
    inline static constexpr auto value =
        std::tuple_size_v<units_type_1> == std::tuple_size_v<units_type_2> &&
        std::is_same_v<units_type_1, units_type_2>;
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
    using cu_t = composite_unit<Units...>;
    std::apply(
        [&os](auto... up) {
            os << '[';
            std::size_t n{ 0 };
            ((os << up << (++n != cu_t::s_size ? " " : "")), ...);
            os << ']';
        },
        cu_t::s_units
    );
    return os;
}

} // namespace units

#endif // INCLUDED_UNIT_SYSTEM
