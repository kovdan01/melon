#ifndef MELON_CORE_SESSION_HPP_
#define MELON_CORE_SESSION_HPP_

#include <melon/core/export.h>
#include <melon/core/serialization.hpp>
#include <melon/core.hpp>

#include <ce/format.hpp>
#include <ce/io_context_signal_interrupter.hpp>
#include <ce/spawn.hpp>
#include <ce/tcp_listener.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/static_thread_pool.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <msgpack.hpp>

#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


namespace melon::core
{

namespace ba = boost::asio;
namespace bb = boost::beast;
namespace bae = boost::asio::execution;

using socket_executor_t = ba::strand<ba::io_context::executor_type>;
using tcp_socket = ba::basic_stream_socket<ba::ip::tcp, socket_executor_t>;
using tcp_stream = bb::basic_stream<ba::ip::tcp,
                                    socket_executor_t,
                                    bb::simple_rate_policy>;

class AsyncSession : public ce::socket_session<AsyncSession, tcp_stream>
{
public:
    static constexpr std::size_t NUMBER_LIMIT = 8192,
                                 BYTES_PER_SECOND_LIMIT = 8192;
    static constexpr ba::steady_timer::duration TIME_LIMIT = std::chrono::seconds(60);

    AsyncSession(const ba::io_context::executor_type& ex)
        : socket_session<AsyncSession, tcp_stream>{ex}
    {
        m_stream.rate_policy().read_limit(BYTES_PER_SECOND_LIMIT);
    }

    virtual void start_protocol() = 0;

protected:
    template <typename What = buffer_t, typename YieldContext>
    What async_recieve(std::size_t limit, YieldContext& yc, boost::system::error_code& ec)
    {
        m_stream.expires_after(TIME_LIMIT);
        What data = m_serializer.async_deserialize_from(m_stream, limit, yc, ec);
        return data;
    }

    template <typename What, typename YieldContext>
    void async_send(const What& what, YieldContext& yc, boost::system::error_code& ec)
    {
        m_stream.expires_after(TIME_LIMIT);
        m_serializer.async_serialize_to(m_stream, what, yc, ec);
    }

private:
    serialization::Serializer m_serializer{};
};

class SyncSession
{
public:
    static constexpr std::size_t BUFFER_LIMIT = 8192;

    SyncSession(const std::string& ip, const std::string& port)
    {
        boost::asio::connect(m_stream, boost::asio::ip::tcp::resolver{m_io_context}.resolve(ip, port));
    }

    template <typename What>
    void send(const What& what)
    {
        m_serializer.serialize_to(m_stream, what);
    }

    template <typename What = buffer_t>
    [[nodiscard]] What receive(std::size_t limit = BUFFER_LIMIT)
    {
        return m_serializer.deserialize_from(m_stream, limit);
    }

private:
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_stream{m_io_context};
    serialization::Serializer m_serializer{};
};

}  // namespace melon::core

#endif  // MELON_CORE_SESSION_HPP_
