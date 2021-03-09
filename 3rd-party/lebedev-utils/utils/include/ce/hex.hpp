#ifndef UUID_379223EE_EF77_4D0A_96C7_746B43847003
#define UUID_379223EE_EF77_4D0A_96C7_746B43847003

#include <ce/utils/export.h>

#include <iosfwd>
#include <span>

namespace ce
{
    namespace detail
    {
        struct hex_t
        {
            std::span<const std::byte> data;
            std::size_t columns;
        };

        CE_UTILS_EXPORT std::ostream& operator<<(std::ostream& stream,const hex_t& ht);
    }

    inline auto hex(std::span<const std::byte> data,std::size_t columns = std::size_t(-1)) noexcept
    {
        return detail::hex_t{data,columns};
    }
}

#endif
