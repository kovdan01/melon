#ifndef UUID_42282A13_C447_4885_AFE6_96E56C27B2EC
#define UUID_42282A13_C447_4885_AFE6_96E56C27B2EC

#include <ampi/utils/bit.hpp>

#include <compare>

namespace ampi
{
    template<typename T,ptrdiff_t TagBits>
    class tagged_pointer
    {
        constexpr static ptrdiff_t free_bits = std::countr_zero(alignof(
            std::conditional_t<std::is_void_v<T>,std::max_align_t,T>));
        static_assert(free_bits>=TagBits,"insufficient least significant bits");
        constexpr static uintptr_t mask = bit_span<uint64_t>(TagBits);
    public:
        tagged_pointer() noexcept = default;

        constexpr tagged_pointer(T* pointer,uintptr_t tag = 0) noexcept
            : p_{reinterpret_cast<uintptr_t>(pointer)|tag}
        {
            assert(tag<=mask);
        }

        template<typename OtherT,ptrdiff_t OtherTagBits>
            requires (!(std::is_same_v<T,OtherT>&&TagBits!=OtherTagBits))&&
                     std::is_convertible_v<OtherT*,T*>&&(OtherTagBits<=TagBits)
        constexpr tagged_pointer(tagged_pointer<OtherT,OtherTagBits> other) noexcept
            : tagged_pointer{other.pointer(),other.tag()}
        {}

        explicit constexpr tagged_pointer(uintptr_t value) noexcept
            : p_{value}
        {
            assert(std::countr_zero(value&~mask)>=free_bits);
        }

        T* pointer() const noexcept
        {
            return reinterpret_cast<T*>(p_&~mask);
        }

        constexpr void set_pointer(T* pointer) noexcept
        {
            auto pv = reinterpret_cast<uintptr_t>(pointer);
            assert(!(pv&mask));
            p_ = pv|tag();
        }

        constexpr uintptr_t tag() const noexcept
        {
            return p_&mask;
        }

        constexpr void set_tag(uintptr_t new_tag) noexcept
        {
            assert(new_tag<=mask);
            p_ = (p_&~mask)|new_tag;
        }

        explicit constexpr operator uintptr_t() const noexcept
        {
            return p_;
        }

        constexpr auto operator<=>(const tagged_pointer& other) const noexcept = default;
    private:
        uintptr_t p_;
    };
}

#endif
