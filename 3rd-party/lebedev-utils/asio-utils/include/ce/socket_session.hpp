#ifndef UUID_A62EF946_4125_424B_AC63_B3B73F116664
#define UUID_A62EF946_4125_424B_AC63_B3B73F116664

#include <ce/asio-utils/export.h>
#include <ce/executor_wrapper.hpp>
#include <ce/get_socket.hpp>

#include <boost/asio/execution/relationship.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

#include <exception>
#include <memory>
#include <string_view>
#include <type_traits>

namespace ce
{
    constexpr inline const char* remote_attr_name = "Remote";

    // In many cases, especially when using boost::beast::tcp_stream,
    // we're guaranteed to be operating under strand and never using log
    // from multiple threads. Still, there are same cases like proxying,
    // where we might have several callbacks in flight that might be invoked
    // in parallel, so we're using a multi-threading safe logger.
    using socket_session_logger_t =
        boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;

    ASIO_UTILS_EXPORT socket_session_logger_t make_socket_session_logger(
        const ba::ip::tcp::endpoint& endpoint);

    BOOST_LOG_ATTRIBUTE_KEYWORD(remote,remote_attr_name,ba::ip::tcp::socket::endpoint_type)

    template<typename Executor>
    class catching_executor;

    class ASIO_UTILS_EXPORT socket_session_base
    {
    public:
        virtual ~socket_session_base();

        socket_session_logger_t& log() noexcept
        {
            return *log_;
        }

        virtual void handle_exception(const std::exception& e);
    protected:
        std::optional<socket_session_logger_t> log_;
    };

    template<typename Executor>
    class catching_executor : public executor_wrapper<catching_executor,Executor>
    {
    public:
        catching_executor(Executor ex,socket_session_base& ssb) noexcept
            : executor_wrapper<catching_executor,Executor>{std::move(ex)},
              ssb_{&ssb}
        {}

        template<typename OtherExecutor>
        catching_executor(Executor ex,const catching_executor<OtherExecutor>& ce) noexcept
            : catching_executor{std::move(ex),ce.session()}
        {}

        socket_session_base& session() const noexcept
        {
            return *ssb_;
        }

        template<typename F>
        std::enable_if_t<bae::can_execute_v<const Executor&,F>> execute(F&& f) const
        {
            bae::execute(this->ex_,[ssb=ssb_,f=std::forward<F>(f)]() mutable {
                try{
                    std::forward<F>(f)();
                }
                catch(const std::exception& e){
                    ssb->handle_exception(e);
                }
            });
        }
    private:
        socket_session_base* ssb_;
    };

    // Base class for session objects that need enable_shared_from_this functionality,
    // store a stream object, provide a logger with severity and remote endpoint attributes and
    // catch exceptions from default and continuation executor.

    template<typename Derived,
             typename Stream>
    class socket_session : public std::enable_shared_from_this<Derived>,
                           public socket_session_base
    {
    public:
        using executor_type = catching_executor<typename Stream::executor_type>;
        using stream_t = typename Stream::template rebind_executor<executor_type>::other;

        template<typename... Ts>
        socket_session(Ts&&... args) noexcept
            : m_stream{executor_type{typename Stream::executor_type{std::forward<Ts>(args)...},*this}}
        {}

        stream_t& stream() noexcept
        {
            return m_stream;
        }

        void create_logger()
        {
            this->log_ = make_socket_session_logger(get_socket(m_stream).remote_endpoint());
        }

        void start()
        {
            bae::execute(m_stream.get_executor(),[this_=static_cast<Derived*>(this)]{
                this_->start_protocol();
            });
        }

        void handle_exception(const std::exception& e) override
        {
            socket_session_base::handle_exception(e);
            m_stream.close();
        }
    protected:
        stream_t m_stream;

        auto executor() noexcept
        {
            return m_stream.get_executor();
        }

        auto cont_executor() noexcept
        {
            return ba::prefer(m_stream.get_executor(),bae::relationship.continuation);
        }
    };
}

#endif
