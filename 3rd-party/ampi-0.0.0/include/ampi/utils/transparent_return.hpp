#ifndef UUID_7B4B6D17_0431_473D_A271_7F2C95C885D6
#define UUID_7B4B6D17_0431_473D_A271_7F2C95C885D6

#define AMPI_TRANSPARENT_RETURN(...) \
    noexcept(noexcept(__VA_ARGS__))  \
    -> decltype(__VA_ARGS__)         \
    { return __VA_ARGS__; }          \
    /**/

#endif
