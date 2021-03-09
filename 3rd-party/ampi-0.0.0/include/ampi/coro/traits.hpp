#ifndef UUID_0B770B7F_5E0E_4A67_975A_4C6E80994435
#define UUID_0B770B7F_5E0E_4A67_975A_4C6E80994435

#include <ampi/coro/stdcoro.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace ampi
{
    // P1288R0

    namespace detail
    {
        template<typename T>
        struct valid_await_suspend_return : std::false_type {};

        template<>
        struct valid_await_suspend_return<void> : std::true_type {};

        template<>
        struct valid_await_suspend_return<bool> : std::true_type {};

        template<typename Promise>
        struct valid_await_suspend_return<stdcoro::coroutine_handle<Promise>> : std::true_type {};

        template<typename T>
        concept await_suspend_return = valid_await_suspend_return<T>::value;

        template<typename T>
        concept has_member_co_await = requires(T&& a){
            std::forward<T>(a).operator co_await();
        };

        template<typename T>
        concept has_free_co_await = requires(T&& a){
            operator co_await(std::forward<T>(a));
        };
    }

    template<typename T>
    concept awaiter = requires(T&& a,stdcoro::coroutine_handle<void> h){
        a.await_ready()?0:0;
        { a.await_suspend(h) } -> detail::await_suspend_return;
        a.await_resume();
    };

    template<typename T,typename Result>
    concept awaiter_of = awaiter<T>&&
        requires(T&& a) {
            { a.await_resume() } -> std::convertible_to<Result>;
        };

    template<detail::has_member_co_await Awaitable>
    decltype(auto) get_awaiter(Awaitable&& a)
        noexcept(noexcept(std::forward<Awaitable>(a).operator co_await()))
    {
        return std::forward<Awaitable>(a).operator co_await();
    }

    template<detail::has_free_co_await Awaitable>
    decltype(auto) get_awaiter(Awaitable&& a)
        noexcept(noexcept(operator co_await(std::forward<Awaitable>(a))))
    {
        return operator co_await(std::forward<Awaitable>(a));
    }

    template<typename Awaitable>
        requires (!detail::has_member_co_await<Awaitable>)&&(!detail::has_free_co_await<Awaitable>)
    decltype(auto) get_awaiter(Awaitable&& a) noexcept
    {
        return std::forward<Awaitable>(a);
    }

    template<typename T>
    struct awaiter_type
    {
        using type = decltype(get_awaiter(std::declval<T>()));
    };

    template<typename T>
    using awaiter_type_t = typename awaiter_type<T>::type;

    // awaitable needs std::moveable
    // awaitable_of needs awaitable
    // await_result needs awaitable
    // await_result_t needs await_result

    // end of P1288R0

    template<typename Awaitable>
    struct is_asymmetric_awaitable : std::false_type {};

    template<typename Awaitable>
    constexpr inline bool is_asymmetric_awaitable_v = is_asymmetric_awaitable<Awaitable>{};
}

#endif
