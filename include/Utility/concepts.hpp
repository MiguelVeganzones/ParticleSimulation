#ifndef INCLUDED_UTILITY_CONCEPTS
#define INCLUDED_UTILITY_CONCEPTS

#include <chrono>
#include <concepts>
#include <type_traits>

namespace utility::concepts
{

namespace initializable::detail
{
template <typename T>
concept initializable_concept = requires { T::init(); };

template <typename T>
struct is_initializable : std::false_type
{
};

template <typename T>
    requires initializable_concept<T>
struct is_initializable<T> : std::true_type
{
};

template <typename T>
inline constexpr bool is_initializable_v = is_initializable<T>::value;

} // namespace initializable::detail

using initializable::detail::is_initializable;
using initializable::detail::is_initializable_v;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept random_distribution = requires(T t) {
    typename T::param_type;
    typename T::result_type;
    { t() } -> std::same_as<typename T::result_type>;
    requires std::constructible_from<T, typename T::param_type>;
};

namespace manual_clock_impl
{
template <typename T>
concept is_manual_tick_clock_concept = requires {
    T::tick();
    std::chrono::is_clock_v<T>;
};

template <typename T>
struct is_manual_tick_clock : std::false_type
{
};

template <typename T>
    requires is_manual_tick_clock_concept<T>
struct is_manual_tick_clock<T> : std::true_type
{
};

template <typename T>
inline constexpr bool is_manual_tick_clock_v = is_manual_tick_clock<T>::value;

} // namespace manual_clock_impl

using manual_clock_impl::is_manual_tick_clock;
using manual_clock_impl::is_manual_tick_clock_v;

template <typename Clock_Type>
concept Clock = std::chrono::is_clock_v<Clock_Type>;

template <typename T>
concept Duration = requires(T t) { std::chrono::duration_cast<std::chrono::seconds>(t); };

} // namespace utility::concepts

#endif // INCLUDED_UTILITY_CONCEPTS
