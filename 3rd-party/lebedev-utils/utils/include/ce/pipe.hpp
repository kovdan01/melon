#ifndef UUID_7AE8C09A_5708_44CB_A929_FCC861F4E827
#define UUID_7AE8C09A_5708_44CB_A929_FCC861F4E827

#include <ce/file_descriptor.hpp>

#include <array>

// See also boost::process::{async_,}pipe.

namespace ce
{
    class pipe : public std::array<file_descriptor,2>
    {
    public:
        pipe()
            : pipe{[]{
                  std::array<int,2> fds;
                  ce::throw_errno_if_negative(::pipe(&fds[0]),"pipe");
                  return fds;
              }()}
        {}
    private:
        pipe(std::array<int,2> fds)
            : std::array<file_descriptor,2>{{{fds[0],"pipe-read"},{fds[1],"pipe-write"}}}
        {}
    };
}

#endif
