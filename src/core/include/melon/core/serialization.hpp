#ifndef MELON_CORE_SERIALIZATION_HPP_
#define MELON_CORE_SERIALIZATION_HPP_

#include <melon/core/export.h>
#include <melon/core.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <msgpack.hpp>

#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


namespace melon::core::serialization
{

class StringViewOverBinary
{
public:
    StringViewOverBinary(std::vector<melon::core::byte> binary)
        : m_binary(std::move(binary))
    {
        // ensure that binary data contains a null-terminated string
        assert(!m_binary.empty());
        assert(m_binary.back() == 0);

        auto* data_ptr = reinterpret_cast<char*>(m_binary.data());
        m_view = std::string_view(data_ptr, m_binary.size() - 1);
    }

    StringViewOverBinary(const StringViewOverBinary&) = default;
    StringViewOverBinary& operator=(const StringViewOverBinary&) = default;
    StringViewOverBinary(StringViewOverBinary&&) = default;
    StringViewOverBinary& operator=(StringViewOverBinary&&) = default;

    [[nodiscard]] std::string_view view() const
    {
        return m_view;
    }

private:
    std::vector<melon::core::byte> m_binary;
    std::string_view m_view;
};

[[nodiscard]] inline msgpack::sbuffer serialize(std::span<const melon::core::byte> in)
{
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> packer(sbuf);
    if (in.data() == nullptr)
    {
        assert(in.empty());
        melon::core::byte c = 0;
        packer.pack_bin(0);
        packer.pack_bin_body(reinterpret_cast<const char*>(&c), 0);
    }
    else
    {
        std::uint32_t size = static_cast<std::uint32_t>(in.size());
        packer.pack_bin(size);
        packer.pack_bin_body(reinterpret_cast<const char*>(in.data()), size);
    }
    return sbuf;
}

[[nodiscard]] inline msgpack::sbuffer serialize(const std::vector<melon::core::byte>& in)
{
    return serialize(std::span{ in.data(), in.size() });
}

// send strings as binary buffers because msgpack crashes
// on empty string deserialization
[[nodiscard]] inline msgpack::sbuffer serialize(std::string_view in)
{
    // precondition: std::string_view must be null-terminated
    // *in.end() == '\0'
    // use std::span to store not null-terminated data
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> packer(sbuf);
    std::uint32_t size = static_cast<std::uint32_t>(in.size()) + 1;
    packer.pack_bin(size);  // +1 for '\0'
    packer.pack_bin_body(in.data(), size);
    return sbuf;
}

[[nodiscard]] inline msgpack::sbuffer serialize(const std::string& in)
{
    return serialize(std::string_view{in.data(), in.size()});
}

template <typename T>
[[nodiscard]] msgpack::sbuffer serialize(const T& in)
{
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, in);
    return sbuf;
}

template <typename T>
[[nodiscard]] T deserialize(const std::string& s)
{
    msgpack::object_handle obj;
    msgpack::unpack(obj, s.data(), s.size());
    return obj.get().as<T>();
}

template <typename T>
[[nodiscard]] T deserialize(const std::vector<melon::core::byte>& s)
{
    msgpack::object_handle obj;
    msgpack::unpack(obj, reinterpret_cast<const char*>(s.data()), s.size());
    return obj.get().as<T>();
}

class Serializer
{
public:
    template <typename Stream, typename What>
    void serialize_to(Stream& to, const What& what)
    {
        m_out_sbuf = serialize(what);
        m_out_sbuf_size = static_cast<std::uint32_t>(m_out_sbuf.size());
        boost::asio::write(to, boost::asio::buffer(&m_out_sbuf_size, sizeof(m_out_sbuf_size)));
        boost::asio::write(to, boost::asio::buffer(m_out_sbuf.data(), m_out_sbuf.size()));
    }

    template <typename Stream, typename What>
    [[nodiscard]] What deserialize_from(Stream& from, std::size_t limit)
    {
        namespace ba = boost::asio;

        std::uint32_t receive_size;
        std::size_t n = ba::read(from, ba::buffer(&receive_size, sizeof(receive_size)),
                                 ba::transfer_exactly(sizeof(receive_size)), 0);
        if (n != sizeof(receive_size))
        {
            throw melon::Exception("Error: received " + std::to_string(n) + " bytes in size header, "
                                   "expected " + std::to_string(receive_size));
        }

        n = ba::read(from, ba::dynamic_vector_buffer{m_in_buf, limit},
                     ba::transfer_exactly(receive_size), 0);
        if (n != receive_size)
        {
            throw melon::Exception("Error: received " + std::to_string(n) + " bytes in payload, "
                                   "expected " + std::to_string(receive_size));
        }

        What reply = deserialize<What>(m_in_buf);
        m_in_buf.erase(m_in_buf.begin(), m_in_buf.begin() + n);

        return reply;
    }

private:
    msgpack::sbuffer m_out_sbuf;
    std::uint32_t m_out_sbuf_size;
    std::vector<melon::core::byte> m_in_buf;
};

}  // namespace melon::core::serialization

#endif // MELON_CORE_SERIALIZATION_HPP_
