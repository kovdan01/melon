#ifndef UUID_9DDE766A_6573_4166_9AD5_4E89D0B23CB1
#define UUID_9DDE766A_6573_4166_9AD5_4E89D0B23CB1

#include <cerrno>
#include <system_error>

namespace ce
{
    [[noreturn]] inline void throw_errno(const char* what)
    {
        throw std::system_error{errno,std::system_category(),what};
    }

    inline void throw_errno_if_negative(ptrdiff_t r,const char* what)
    {
        if(r<0)
            throw_errno(what);
    }
}

#endif
