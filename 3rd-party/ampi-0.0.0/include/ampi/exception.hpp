#ifndef UUID_DD715D3A_3F64_4B15_9523_E15408C29BBE
#define UUID_DD715D3A_3F64_4B15_9523_E15408C29BBE

#include <ampi/export.h>

#include <exception>

namespace ampi
{
    class AMPI_EXPORT exception : public std::exception
    {
    public:
        ~exception() override;
    };
}

#endif
