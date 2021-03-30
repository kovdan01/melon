#include <ce/socket_session.hpp>

#include <boost/log/attributes/constant.hpp>

namespace ce
{
    socket_session_logger_t make_socket_session_logger(const ba::ip::tcp::endpoint& endpoint)
    {
        socket_session_logger_t logger;
        logger.add_attribute(remote_attr_name,
            boost::log::attributes::constant<ba::ip::tcp::endpoint>(endpoint));
        return logger;
    }

    socket_session_base::~socket_session_base() = default;

    void socket_session_base::handle_exception(const std::exception& e)
    {
        BOOST_LOG_SEV(log(),boost::log::trivial::error)
            << "Unhandled exception: " << e.what();
    }
}
