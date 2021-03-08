#ifndef UUID_75319E13_00FD_480A_B04C_E146AF7F9374
#define UUID_75319E13_00FD_480A_B04C_E146AF7F9374

#include <ce/asio-utils/export.h>
#include <span>

namespace ce
{
    ASIO_UTILS_EXPORT int asio_main(int argc,char* argv[],
                                    int (*main_impl)(std::span<const char* const>));

    int main(std::span<const char* const> args);
}

#endif
