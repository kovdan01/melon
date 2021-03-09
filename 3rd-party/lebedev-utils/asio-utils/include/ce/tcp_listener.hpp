#ifndef UUID_2E00BB3D_3CAF_4FA6_A4C2_CF334E559CCA
#define UUID_2E00BB3D_3CAF_4FA6_A4C2_CF334E559CCA

#include <ce/socket_session.hpp>

#include <boost/asio/ip/v6_only.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/log/trivial.hpp>

#include <cstdint>
#include <functional>

namespace ce
{
    template<typename SocketSession,typename Executor>
    class tcp_listener
    {
    public:
        using session_factory_t = std::function<std::shared_ptr<SocketSession> ()>;

        // Start a TCP server that listens on port.
        // Acceptor uses the specified executor.
        // Session factory provides new sessions.
        tcp_listener(Executor ex,std::uint16_t port,session_factory_t session_factory)
            : acceptor_{std::move(ex)},
              session_factory_{std::move(session_factory)}
        {
            // acceptor can be constructed with an executor and port directly,
            // but we want to reset ipv6_only option to uniformly accept
            // ipv4/ipv6 connections everywhere.
            ba::ip::tcp::endpoint ep{ba::ip::tcp::v6(),port};
            acceptor_.open(ep.protocol());
            acceptor_.set_option(ba::ip::v6_only{false});
            // While we don't want to load-balance multiple servers binding
            // to the same address, this makes quickly restarting debugging
            // sessions much more painless due to TIME_WAIT socket timeouts.
            acceptor_.set_option(ba::socket_base::reuse_address{true});
            acceptor_.bind(ep);
            acceptor_.listen();
            log_ = make_socket_session_logger(ep);
            BOOST_LOG_SEV(log_,boost::log::trivial::info) << "Listening...";
            accept();
        }

        // Same as above, sessions are constructed from the listener's executor.
        tcp_listener(Executor ex,std::uint16_t port)
            : tcp_listener{std::move(ex),port,[this]{
                  return std::make_shared<SocketSession>(acceptor_.get_executor());
              }}
        {}
    private:
        ba::basic_socket_acceptor<ba::ip::tcp,Executor> acceptor_;
        socket_session_logger_t log_;
        session_factory_t session_factory_;
        std::shared_ptr<SocketSession> session_;

        void accept()
        {
            try{
                session_ = session_factory_();
            }
            catch(const std::exception& e){
                BOOST_LOG_SEV(log_,boost::log::trivial::error)
                    << "Failed to create session: " << e.what();
                return;
            }
            acceptor_.async_accept(get_socket(session_->stream()),
                                   [this](boost::system::error_code ec){
                if(ec==ba::error::operation_aborted)
                    return;
                if(ec){
                    BOOST_LOG_TRIVIAL(error) << "Failed to accept connection: " << ec.message();
                    return;
                }
                auto s = std::move(session_);
                accept();
                try{
                    s->create_logger();
                }
                catch(const std::exception& e){
                    BOOST_LOG_SEV(log_,boost::log::trivial::error)
                        << "Failed to create logger for connection from "
                        << get_socket(s->stream()).remote_endpoint()
                        << ": " << e.what();
                    return;
                }
                try{
                    BOOST_LOG_SEV(s->log(),boost::log::trivial::info) << "Connection accepted";
                    s->start();
                }
                catch(const std::exception& e){
                    BOOST_LOG_SEV(s->log(),boost::log::trivial::error)
                        << "Failed to start session: " << e.what();
                }
            });
        }
    };
}

#endif
