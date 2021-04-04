#ifndef UUID_9E658A0C_5CF9_4A98_A343_D3A38A052C03
#define UUID_9E658A0C_5CF9_4A98_A343_D3A38A052C03

#include <ce/asio_ns.hpp>
#include <ce/asio-utils/export.h>

#include <boost/asio/signal_set.hpp>

namespace ce
{
    class ASIO_UTILS_EXPORT io_context_signal_interrupter
    {
    public:
        io_context_signal_interrupter(ba::io_context& ctx);
    private:
        ba::signal_set ss_;
    };
}

#endif
