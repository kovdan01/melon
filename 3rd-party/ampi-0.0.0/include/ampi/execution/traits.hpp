#ifndef UUID_655C6AD6_E576_4778_868A_E118A6631E1D
#define UUID_655C6AD6_E576_4778_868A_E118A6631E1D

#include <boost/asio/system_executor.hpp>

#include <type_traits>

namespace ampi
{
    template<typename Executor>
    struct is_trivial_executor : std::false_type {};

    template<typename Blocking,typename Relationship,typename Allocator>
    struct is_trivial_executor<
            boost::asio::basic_system_executor<Blocking,Relationship,Allocator>>
        : std::bool_constant<!std::is_same_v<Blocking,boost::asio::execution::blocking_t::never_t>> {};

    template<typename Executor>
    constexpr inline bool is_trivial_executor_v = is_trivial_executor<Executor>{};
}

#endif
