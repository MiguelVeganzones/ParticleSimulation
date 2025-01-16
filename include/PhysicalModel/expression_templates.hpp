#include "concepts.hpp"
#include "error_handling.hpp"
#include "particle_concepts.hpp"
#include <functional>
#include <tuple>

namespace pm::magnitudes
{

template <typename T>
[[nodiscard]]
constexpr auto subscript(T const& v, std::integral auto idx) noexcept
{
    if constexpr (utility::concepts::arithmetic<T>)
    {
        return v;
    }
    else
    {
        return v[idx];
    }
}

template <typename Callable, typename... Operands>
class expr
{
public:
    using callable_t = Callable;

public:
    expr(callable_t f, Operands const&... args) :
        args_(args...),
        f_{ f }
    {
    }

    [[nodiscard]]
    inline constexpr auto operator[](std::integral auto idx) const
    {
        auto apply_at_index = [this, idx](Operands const&... args) {
            return f_(subscript(args, idx)...);
        };
        return std::apply(apply_at_index, args_);
    }

private:
    std::tuple<Operands const&...> args_;
    callable_t                     f_;
};

auto operator+(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) {
                    return std::invoke(
                        std::plus{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator-(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) {
                    return std::invoke(
                        std::minus{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator*(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) {
                    return std::invoke(
                        std::multiplies{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator/(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) {
                    return std::invoke(
                        std::divides{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

} // namespace pm::magnitudes
