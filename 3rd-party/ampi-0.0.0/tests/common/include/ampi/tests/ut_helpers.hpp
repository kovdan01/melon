#ifndef UUID_26DDB987_D5D9_4C05_BB4D_E13C982C46FB
#define UUID_26DDB987_D5D9_4C05_BB4D_E13C982C46FB

#include <boost/ut.hpp>

namespace ampi
{
    constexpr inline struct : boost::ut::detail::op
    {
        constexpr operator std::nullptr_t() const noexcept
        {
            return nullptr;
        }
    } nullptr_v;
}

#endif
