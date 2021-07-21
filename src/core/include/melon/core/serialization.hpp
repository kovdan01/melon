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


namespace melon::core
{

namespace serialization
{

template <typename T>
[[nodiscard]] msgpack::sbuffer serialize(const T& in)
{
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, in);
    return sbuf;
}

template <typename T>
[[nodiscard]] T deserialize(const buffer_t& s)
{
    msgpack::object_handle obj;
    msgpack::unpack(obj, reinterpret_cast<const char*>(s.data()), s.size());
    return obj.get().as<T>();
}

class Serializer
{
public:
    template <typename What, typename Stream, typename YieldContext>
    void async_serialize_to(Stream& to, const What& what, YieldContext& yc, boost::system::error_code& ec)
    {
        namespace ba = boost::asio;

        m_out_sbuf = serialize(what);
        m_out_sbuf_size = static_cast<std::uint32_t>(m_out_sbuf.size());
        ba::async_write(to, ba::buffer(&m_out_sbuf_size, sizeof(m_out_sbuf_size)), yc[ec]);
        ba::async_write(to, ba::buffer(m_out_sbuf.data(), m_out_sbuf.size()), yc[ec]);
    }

    template <typename What = buffer_t, typename Stream, typename YieldContext>
    [[nodiscard]] What async_deserialize_from(Stream& from, std::size_t limit, YieldContext& yc, boost::system::error_code& ec)
    {
        namespace ba = boost::asio;

        std::uint32_t receive_size;
        std::size_t n = ba::async_read(from, ba::buffer(&receive_size, sizeof(receive_size)),
                                       ba::transfer_exactly(sizeof(receive_size)), yc[ec]);
        if (n != sizeof(receive_size))
        {
            throw melon::core::Exception("Error: received " + std::to_string(n) + " bytes in size header, "
                                         "expected " + std::to_string(receive_size));
        }

        n = ba::async_read(from, ba::dynamic_vector_buffer{m_in_buf, limit},
                           ba::transfer_exactly(receive_size), yc[ec]);
        if (n != receive_size)
        {
            throw melon::core::Exception("Error: received " + std::to_string(n) + " bytes in payload, "
                                         "expected " + std::to_string(receive_size));
        }

        What reply = deserialize<What>(m_in_buf);
        m_in_buf.erase(m_in_buf.begin(), m_in_buf.begin() + n);

        return reply;
    }

    template <typename What, typename Stream>
    void serialize_to(Stream& to, const What& what)
    {
        namespace ba = boost::asio;

        m_out_sbuf = serialize(what);
        m_out_sbuf_size = static_cast<std::uint32_t>(m_out_sbuf.size());
        ba::write(to, ba::buffer(&m_out_sbuf_size, sizeof(m_out_sbuf_size)));
        ba::write(to, ba::buffer(m_out_sbuf.data(), m_out_sbuf.size()));
    }

    template <typename What = buffer_t, typename Stream>
    [[nodiscard]] What deserialize_from(Stream& from, std::size_t limit)
    {
        namespace ba = boost::asio;

        std::uint32_t receive_size;
        std::size_t n = ba::read(from, ba::buffer(&receive_size, sizeof(receive_size)),
                                 ba::transfer_exactly(sizeof(receive_size)), 0);
        if (n != sizeof(receive_size))
        {
            throw melon::core::Exception("Error: received " + std::to_string(n) + " bytes in size header, "
                                         "expected " + std::to_string(receive_size));
        }

        n = ba::read(from, ba::dynamic_vector_buffer{m_in_buf, limit},
                     ba::transfer_exactly(receive_size), 0);
        if (n != receive_size)
        {
            throw melon::core::Exception("Error: received " + std::to_string(n) + " bytes in payload, "
                                         "expected " + std::to_string(receive_size));
        }

        What reply = deserialize<What>(m_in_buf);
        m_in_buf.erase(m_in_buf.begin(), m_in_buf.begin() + n);

        return reply;
    }

private:
    msgpack::sbuffer m_out_sbuf;
    std::uint32_t m_out_sbuf_size;
    buffer_t m_in_buf;
};

}  // namespace serialization

}  // namespace melon::core

#endif  // MELON_CORE_SERIALIZATION_HPP_
