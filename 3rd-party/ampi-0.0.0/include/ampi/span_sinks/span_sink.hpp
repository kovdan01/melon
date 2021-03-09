#ifndef UUID_85B5B315_D9D6_4FC5_9FC4_F7A594BA7E88
#define UUID_85B5B315_D9D6_4FC5_9FC4_F7A594BA7E88

#include <ampi/span_sources/span_source.hpp>

namespace ampi
{
    template<typename T>
    concept span_sink = requires(T& ss,archetypes::span_source& source) {
        { ss(source) } -> coroutine_returning<void>;
    };
}

#endif
