#ifndef MELON_CORE_SERIALIZATION_HPP_
#define MELON_CORE_SERIALIZATION_HPP_

#include <melon/core/export.h>

#include <msgpack.hpp>

#include <string>

namespace melon::core::serialization
{

template <typename T>
std::pair<std::uint32_t, std::string> serialize(const T& in)
{
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, in);
    auto size = static_cast<uint32_t>(sbuf.size());
    return { size, {sbuf.data()} };
}

template <typename T>
T deserialize(const std::string& s)
{
    msgpack::object_handle obj;
    unpack(obj, s.data(), s.size());
    return obj.get().as<T>();
}

}  // namespace melon::core::serialization

#endif // MELON_CORE_SERIALIZATION_HPP_
