#include <ce/io_context_signal_interrupter.hpp>

#include <boost/log/trivial.hpp>

namespace ce
{
    io_context_signal_interrupter::io_context_signal_interrupter(ba::io_context& ctx)
        : ss_{ctx,SIGINT,SIGTERM}
    {
        ss_.async_wait([&](boost::system::error_code ec,int signal){
            if(ec)
                // only operation_aborted possible.
                return;
            BOOST_LOG_TRIVIAL(info) << "Terminating in response to signal " << signal << '.';
            ctx.stop();
        });
    }
}
