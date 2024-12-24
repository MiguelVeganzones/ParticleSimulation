#pragma once

#include <array>
#include <type_traits>

namespace utility::compile_time_utility
{

template <std::size_t N, typename Fn, typename... Args>
    requires std::is_invocable_v<Fn, std::size_t, Args...>
[[nodiscard]]
constexpr auto array_factory(Fn&& fn, Args&&... args) noexcept
    -> std::array<std::invoke_result_t<Fn, std::size_t, Args...>, N>
{
    using T = std::invoke_result_t<Fn, std::size_t, Args...>;
    return
        []<std::size_t... Indices>(Fn&& f, Args&&... a, std::index_sequence<Indices...>) {
            return std::array<T, N>{ f(Indices, a...)... };
        }(std::forward<Fn>(fn), std::forward<Args>(args)..., std::make_index_sequence<N>{}
        );
}

template <std::size_t N, typename T>
    requires(!std::is_invocable_v<T>)
[[nodiscard]]
constexpr auto array_factory(T const& value) noexcept -> std::array<T, N>
{
    return []<std::size_t... Indices>(T const& v, std::index_sequence<Indices...>) {
        return std::array<T, N>{ ((void)Indices, v)... };
    }(value, std::make_index_sequence<N>{});
}

} // namespace utility::compile_time_utility
