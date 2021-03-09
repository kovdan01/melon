#ifndef UUID_4624525F_8473_4409_811A_09C1F9CCFAA1
#define UUID_4624525F_8473_4409_811A_09C1F9CCFAA1

#include <sstream>
#include <utility>

namespace ce
{
    template<typename... Ts>
    std::string format(Ts&&... args)
    {
        std::ostringstream oss;
        (oss << ... << std::forward<Ts>(args));
        return std::move(oss).str();
    }
}

#endif
