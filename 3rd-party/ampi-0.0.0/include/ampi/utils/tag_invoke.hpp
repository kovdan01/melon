#ifndef UUID_81726E98_A81D_418F_867B_96105BFA2396
#define UUID_81726E98_A81D_418F_867B_96105BFA2396

#include <ampi/utils/transparent_return.hpp>

#include <type_traits>
#include <utility>

namespace ampi
{
    namespace tag_invoke_ns
    {
        void tag_invoke() = delete;

        struct tag_invoke_fn
        {
            template<typename CPO,typename... Args>
            constexpr auto operator()(CPO cpo,Args&&... args) const
            AMPI_TRANSPARENT_RETURN(tag_invoke(cpo,std::forward<Args>(args)...))
        };
    }

    inline namespace tag_invoke_cpo
    {
        constexpr inline tag_invoke_ns::tag_invoke_fn tag_invoke;
    }

    template<auto& CPO>
    using tag_t = std::remove_cvref_t<decltype(CPO)>;

    template<typename CPO,typename... Args>
    using tag_invoke_result_t = std::invoke_result_t<decltype(tag_invoke),CPO,Args...>;

    template<typename CPO,typename... Args>
    constexpr inline bool is_tag_invocable_v = std::is_invocable_v<decltype(tag_invoke),CPO,Args...>;
    
    template<typename CPO,typename... Args>
    struct is_tag_invocable : std::bool_constant<is_tag_invocable_v<CPO,Args...>> {};

    template<typename CPO,typename... Args>
    constexpr inline bool is_nothrow_tag_invocable_v =
        std::is_nothrow_invocable_v<decltype(tag_invoke),CPO,Args...>;

    template<typename CPO,typename... Args>
    struct is_nothrow_tag_invocable : std::bool_constant<is_nothrow_tag_invocable_v<CPO,Args...>> {};
}

#endif
