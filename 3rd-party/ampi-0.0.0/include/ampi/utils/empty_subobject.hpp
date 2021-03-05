#ifndef UUID_3A252FF4_4118_47F0_A70C_8003145861E9
#define UUID_3A252FF4_4118_47F0_A70C_8003145861E9

#include <type_traits>

namespace ampi
{
    struct empty_subobject
    {
        empty_subobject() = default;

        constexpr empty_subobject(auto&&... /*args*/) noexcept {}
    };

    template<bool Condition,typename T>
    using subobject_if = std::conditional_t<Condition,T,empty_subobject>;
}

#endif
