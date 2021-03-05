#ifndef UUID_1E2A8E01_B3A0_4D47_9C02_8C499A59AC33
#define UUID_1E2A8E01_B3A0_4D47_9C02_8C499A59AC33

#include <ampi/buffer.hpp>
#include <ampi/coro/coroutine.hpp>

namespace ampi
{
    namespace archetypes
    {
        struct span_source : nothing
        {
            template<ampi::buffer_factory BufferFactory>
            async_generator<buffer> operator()(BufferFactory& bf);
        };
    }

    template<typename T>
    concept span_source = requires(T& ss,archetypes::buffer_factory& bf) {
        { ss(bf) } -> async_generator_yielding<buffer>;
    };
}

#endif
