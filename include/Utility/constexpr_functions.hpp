#ifndef INCLUDED_CONSTEXPR_FUNNCTIONS
#define INCLUDED_CONSTEXPR_FUNNCTIONS

namespace utility::cx_functions
{

// Do not use this for anything serious
template <typename T>
inline constexpr auto pow(T base, T exp) noexcept -> T
{
    T result{ 1 };
    for (; exp > 0; --exp)
    {
        result *= base;
    }
    return result;
}

} // namespace utility::cx_functions

#endif // INCLUDED_CONSTEXPR_FUNNCTIONS
