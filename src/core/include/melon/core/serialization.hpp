#ifndef MELON_CORE_SERIALIZATION_HPP_
#define MELON_CORE_SERIALIZATION_HPP_

#include <melon/core/export.h>
#include <melon/core.hpp>

#include <msgpack.hpp>

#include <span>
#include <string>
#include <vector>



#include <iostream>

namespace melon::core::serialization
{

inline std::pair<std::uint32_t, msgpack::sbuffer> serialize(std::span<const melon::core::byte> in)
{
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> packer(sbuf);
    //msgpack::pack(sbuf, in);
    packer.pack_bin(in.size());
    packer.pack_bin_body(reinterpret_cast<const char*>(in.data()), in.size());
    std::uint32_t size = static_cast<std::uint32_t>(sbuf.size());
    std::cerr << "serialize sbuf: " << (int)sbuf.data()[0] << ", " << sbuf.size() << std::endl;
    return { size, std::move(sbuf) };
}

template <typename T>
std::pair<std::uint32_t, msgpack::sbuffer> serialize(const T& in)
{
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, in);
    std::uint32_t size = static_cast<std::uint32_t>(sbuf.size());
    std::cerr << "serialize sbuf: " << (int)sbuf.data()[0] << ", " << sbuf.size() << std::endl;
    return { size, std::move(sbuf) };
}

template <typename T>
T deserialize(const std::string& s)
{
    msgpack::object_handle obj;
    std::cerr << "s.data() = " << (void*)s.data() << ", " << (int)s[0] << ", " << s.size() << std::endl;
    msgpack::unpack(obj, s.data(), s.size());
    std::cerr << "after unpack" << std::endl;
    return obj.get().as<T>();
}

}  // namespace melon::core::serialization

#endif // MELON_CORE_SERIALIZATION_HPP_
