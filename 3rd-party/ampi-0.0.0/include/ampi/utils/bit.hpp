#ifndef UUID_AFD1A7C8_E9A9_4C5C_9522_F81355AE11E6
#define UUID_AFD1A7C8_E9A9_4C5C_9522_F81355AE11E6

#include <ampi/utils/stdtypes.hpp>

#include <bit>
#include <cassert>

namespace ampi
{
    template<typename T>
    constexpr T round_up_pot(T x,T y) noexcept
    {
        assert(std::has_single_bit(y));
        return (x+(y-1))&~(y-1);
    }

    template<typename T>
    constexpr inline size_t type_width = sizeof(T)*8;

    template<typename T>
    constexpr T shll(T x,unsigned n) noexcept
    {
        return T(n<type_width<T>?x<<n:T());
    }

    template<typename T>
    constexpr T bit_span(unsigned n,unsigned shift = 0) noexcept
    {
        return T(T(shll(T(1),n)-1)<<shift);
    }
}

#endif
