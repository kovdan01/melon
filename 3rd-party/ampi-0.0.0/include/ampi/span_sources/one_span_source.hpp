#ifndef UUID_FEA29053_69C9_4CA5_8D32_4B5BE2C3587A
#define UUID_FEA29053_69C9_4CA5_8D32_4B5BE2C3587A

#include <ampi/span_sources/span_source.hpp>

namespace ampi
{
    class one_span_source
    {
    public:
        one_span_source(binary_view_t bv) noexcept
            : bv_{bv}
        {}

        generator<buffer> operator()(buffer_factory auto& /*bf*/) /*[[clang::lifetimebound]]*/
        {
            co_yield buffer{bv_};
        }
    private:
        binary_view_t bv_;
    };
}

#endif
