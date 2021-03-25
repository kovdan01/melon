#ifndef MELON_CLIENT_DESKTOP_HELPERS_HPP_
#define MELON_CLIENT_DESKTOP_HELPERS_HPP_

#include <cstddef>

namespace melon::client_desktop
{

inline std::size_t to_size_t(int value)
{
    return static_cast<std::size_t>(value);
}

inline int to_int(std::size_t value)
{
    return static_cast<int>(value);
}

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_HELPERS_HPP_
