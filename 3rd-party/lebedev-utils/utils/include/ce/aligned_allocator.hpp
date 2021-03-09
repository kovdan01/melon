#ifndef UUID_24EB7089_013D_4327_9F53_3B026AE5DDF8
#define UUID_24EB7089_013D_4327_9F53_3B026AE5DDF8

#include <boost/align/aligned_allocator.hpp>

namespace ce
{
    // boost::container + boost::alignment::aligned_allocator + default_init
    // doesn't work. default_init_t is handled by boost::container::allocator_traits::construct,
    // which handles it only for std::allocator or when member construct is not callable
    // with default_init_t. boost::alignment::aligned_allocator does provide member construct,
    // which is unconstrained. While both of these do not align (pun intended) with new allocator
    // requirements, the quick fix is to just constrain member construct.

    template<typename T,std::size_t Alignment>
    class aligned_allocator : public boost::alignment::aligned_allocator<T,Alignment>
    {
        using base_t = boost::alignment::aligned_allocator<T,Alignment>; 
    public:
        using base_t::base_t;

        template<typename... Args>
            requires std::is_constructible_v<T,Args...>
        void construct(T* p,Args&&... args)
            noexcept(std::is_nothrow_constructible_v<T,Args...>)
        {
            base_t::construct(p,std::forward<Args>(args)...);
        }
    };
}

#endif
