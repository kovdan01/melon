#ifndef MELON_CORE_LOG_CONFIGURATION_HPP_
#define MELON_CORE_LOG_CONFIGURATION_HPP_

#include <ce/socket_session.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace melon::core::log_conf
{

    static void setup_boost_log()
    {
        namespace bl = boost::log;
        bl::add_console_log(std::cerr, bl::keywords::format = (bl::expressions::stream
                        << bl::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
                        << " [" << bl::trivial::severity << "] T"
                        << bl::expressions::attr<bl::attributes::current_thread_id::value_type>("ThreadID")
                        << " @" << ce::remote
                        << " : " << bl::expressions::smessage
                        ),
                        bl::keywords::auto_flush = true);
        bl::add_common_attributes();
    }

}  // namespace melon::core::log_conf

#endif // MELON_CORE_LOG_CONFIGURATION_HPP_
