#ifndef UUID_E66ED34A_397A_4026_A920_04453AAF1D62
#define UUID_E66ED34A_397A_4026_A920_04453AAF1D62

#include <concepts>

namespace ampi::archetypes
{
    struct nothing
    {
        nothing(nothing&&) = delete;
        ~nothing() = delete;
    };
}

#endif
