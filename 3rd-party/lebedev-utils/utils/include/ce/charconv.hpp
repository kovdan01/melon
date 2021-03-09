#ifndef UUID_2404B559_1764_4ADF_A985_BC6FEC22E214
#define UUID_2404B559_1764_4ADF_A985_BC6FEC22E214

#include <charconv>
#include <optional>
#include <string_view>
#include <system_error>

namespace ce
{
    template<typename T>
    std::optional<T> from_chars(std::string_view sv) noexcept
    {
        T out;
        auto end = sv.data()+sv.size();
        auto res = std::from_chars(sv.data(),end,out);
        if(res.ec==std::errc{}&&res.ptr==end)
            return out;
        return {};
    }
}

#endif
