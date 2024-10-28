#ifndef INCLUDED_ERROR_HANDLING_UTILITY
#define INCLUDED_ERROR_HANDLING_UTILITY

#include <cassert>
#include <utility>

namespace utility::error_handling
{

[[noreturn]]
inline auto assert_unreachable() -> void
{
#if !NDEBUG
    assert(false);
#endif
    std::unreachable();
}

} // namespace utility::error_handling

#endif // INCLUDED_ERROR_HANDLING_UTILITY
