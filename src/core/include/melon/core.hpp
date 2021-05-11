#ifndef MELON_CORE_MELON_CORE_HPP_
#define MELON_CORE_MELON_CORE_HPP_

#include <melon/core/export.h>
#include <melon/core/yaml_config.hpp>

#include <span>
#include <vector>

namespace melon::core
{

using byte_t = unsigned char;
using buffer_t = std::vector<byte_t>;
using buffer_view_t = std::span<const byte_t>;

MELON_CORE_EXPORT void hello();

}  // namespace melon::core

#endif  // MELON_CORE_MELON_CORE_HPP_
