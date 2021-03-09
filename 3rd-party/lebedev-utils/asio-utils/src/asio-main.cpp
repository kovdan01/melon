#include <ce/asio-main.hpp>
#include <ce/socket_session.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <iostream>

namespace ce
{
    int asio_main(int argc,char* argv[],int (*main_impl)(std::span<const char* const>))
    try{
        // Setup logging and failsafe exception handlers.
        std::ios_base::sync_with_stdio(false);
        namespace bl = boost::log;
        bl::add_console_log(std::cerr,
            bl::keywords::format = (
                bl::expressions::stream
                    << bl::expressions::format_date_time<boost::posix_time::ptime>(
                        "TimeStamp","%Y-%m-%d %H:%M:%S.%f"
                    )
                    << " [" << bl::trivial::severity << "] T"
                    << bl::expressions::attr<bl::attributes::current_thread_id::value_type>("ThreadID")
                    << " @" << ce::remote
                    << " : " << bl::expressions::smessage
            ),
            bl::keywords::auto_flush = true
        );
        bl::add_common_attributes();
        try{
            return main_impl({argv,std::size_t(argc)});
        }
        catch(...){
            BOOST_LOG_TRIVIAL(fatal) << boost::current_exception_diagnostic_information();
            return 1;
        }
    }
    catch(...){
        std::cerr << '\n' << boost::current_exception_diagnostic_information() << '\n';
        return 1;
    }
}
