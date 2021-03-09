#ifndef UUID_AC4BC2BF_CEEB_45A2_8223_8BA0136B043F
#define UUID_AC4BC2BF_CEEB_45A2_8223_8BA0136B043F

#include <cassert>
#include <compare>

namespace ampi
{
    inline std::strong_ordering make_strong_ordering(std::partial_ordering o) noexcept
    {
        assert(o!=std::partial_ordering::unordered);
        return o==std::partial_ordering::equivalent?std::strong_ordering::equivalent:
               o==std::partial_ordering::less      ?std::strong_ordering::less:
                                                    std::strong_ordering::greater;
    }
}

#endif
