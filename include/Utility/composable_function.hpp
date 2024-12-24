#ifndef INCLUDED_COMPOSABLE_FUNCTION
#define INCLUDED_COMPOSABLE_FUNCTION

#include "concepts.hpp"
#include <functional>
#include <type_traits>

#ifndef DEBUG_PRINT
#define DEBUG_PRINT (false)
#if DEBUG_PRINT
#ifndef DEBUG_OSTREAM
#include <iostream>
#define DEBUG_OSTREAM std::cout
#endif
#endif
#endif

namespace utility::composable_function
{

template <typename... Fns>
class composable;

template <typename Fn>
class composable<Fn>
{
    using function_type = std::decay_t<Fn>;

public:
    constexpr composable(Fn&& fn
    ) noexcept(std::is_nothrow_constructible_v<function_type, Fn&&>) :
        fn_(std::forward<function_type>(fn))
    {
    }

    template <typename... Args>
        requires std::is_invocable_v<function_type, Args...>
    inline constexpr auto operator()(Args&&... args) const
        noexcept(noexcept(std::invoke(fn_, std::forward<Args>(args)...)))
            -> decltype(auto)
    {
#if DEBUG_PRINT
        if constexpr (sizeof...(Args) > 0)
        {
            DEBUG_OSTREAM << '(' << (args << ...) << ')' << " -> ";
        }
        else
        {
            DEBUG_OSTREAM << "() -> ";
        }
        auto v =
#else
        return
#endif
            std::invoke(fn_, std::forward<Args>(args)...);
#if DEBUG_PRINT
        DEBUG_OSTREAM << v << '\n';
        return v;
#endif
        // return std::invoke(fn_, std::forward<Args>(args)...);
    }

private:
    mutable function_type fn_;
};

template <typename Fn, typename... Fns>
class composable<Fn, Fns...>
{
    using function_type = std::decay_t<Fn>;
    using nested_type   = composable<Fns...>;

public:
    constexpr composable(Fn&& fn, Fns&&... fns) noexcept(
        noexcept(std::is_nothrow_constructible_v<function_type, Fn&&>) &&
        noexcept(std::is_nothrow_constructible_v<nested_type, Fns...>)
    ) :
        fn_(std::forward<function_type>(fn)),
        next_(std::forward<Fns>(fns)...)
    {
    }

    template <typename... Args>
        requires std::is_invocable_v<function_type, Args...> &&
                 std::is_invocable_v<
                     nested_type,
                     std::invoke_result_t<function_type, Args...>>
    inline constexpr auto operator()(Args&&... args) const
        noexcept(noexcept(std::invoke(fn_, std::forward<Args>(args)...)))
            -> decltype(auto)
    {
#if DEBUG_PRINT
        if constexpr (sizeof...(Args) > 0)
        {
            DEBUG_OSTREAM << '(' << (args << ...) << ')' << " -> ";
        }
        else
        {
            DEBUG_OSTREAM << "() -> ";
        }
#endif
        return std::invoke(next_, std::invoke(fn_, std::forward<Args>(args)...));
    }

private:
    mutable function_type fn_;
    nested_type           next_;
};

template <typename... Fns>
constexpr auto make_composable(Fns&&... fns)
{
    return composable_function::composable<std::decay_t<Fns>...>(std::forward<Fns>(fns)...
    );
}

template <typename... Fns>
using composed_function_t =
    typename composable_function::composable<std::decay_t<Fns>...>;

} // namespace utility::composable_function

#endif // INCLUDED_COMPOSABLE_FUNCTION
