#include <melon/core/log_configuration.hpp>

#include <ce/socket_session.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace melon::core::log
{

void setup()
{
    namespace bl = boost::log;
    bl::add_console_log(std::cerr,
                        bl::keywords::format =
                        (
                            bl::expressions::stream
                            << bl::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")  // -V747
                            << " [" << bl::trivial::severity << "] T"
                            << bl::expressions::attr<bl::attributes::current_thread_id::value_type>("ThreadID")
                            << " @" << ce::remote
                            << " : " << bl::expressions::smessage
                        ),
                        bl::keywords::auto_flush = true);
}

}  // namespace melon::core::log
