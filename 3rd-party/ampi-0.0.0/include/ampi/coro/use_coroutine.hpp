#ifndef UUID_B5A8AB9C_DB77_464C_9654_C0BDC642FCC4
#define UUID_B5A8AB9C_DB77_464C_9654_C0BDC642FCC4

#include <ampi/coro/coroutine.hpp>
#include <ampi/execution/executor_wrapper.hpp>

#include <boost/asio/async_result.hpp>
#include <boost/system/system_error.hpp>

namespace ampi
{
    namespace detail
    {
        struct callback_args_to_result
        {
            template<typename... Ts>
            void operator()(Ts&&... args) noexcept
            {
                return pack_args(std::forward<Ts>(args)...);
            }

            template<typename... Ts>
            void operator()(std::exception_ptr e,Ts&&... args)
            {
                if(e)
                    std::rethrow_exception(std::move(e));
                return pack_args(std::forward<Ts>(args)...);
            }

            template<typename... Ts>
            void operator()(boost::system::error_code ec,Ts&&... args)
            {
                if(ec)
                    throw boost::system::system_error(ec);
                return pack_args(std::forward<Ts>(args)...);
            }

            template<typename... Ts>
            auto pack_args(Ts&&... args) noexcept
            {
                if constexpr(sizeof...(args)==0)
                    return;
                else if constexpr(sizeof...(args)==1)
                    return std::move(args...);
                else
                    return std::make_tuple(std::move(args)...);
            }
        };

        template<typename Args,typename Initiation,typename... InitArgs>
        struct use_coroutine_awaitable;

        template<typename... Args,typename Initiation,typename... InitArgs>
        struct use_coroutine_awaitable<std::tuple<Args...>,Initiation,InitArgs...>
        {
            struct handler
            {
                use_coroutine_awaitable* awaitable_;
                coro_handle_owner<> handle_;

                void operator()(Args... args)
                {
                    std::tuple<Args&&...> r{std::move(args)...};
                    awaitable_->result_ = &r;
                    std::move(handle_).release()();
                }
            };

            Initiation initiation_;
            std::tuple<InitArgs...> init_args_;
            std::tuple<Args&&...>* result_;

            auto await_resume() noexcept
            {
                return std::apply(detail::callback_args_to_result{},std::move(*result_));
            }
        };

        template<coroutine_options Options,typename Result,typename Executor>
        template<typename... Args,typename Initiation,typename... InitArgs>
        auto coroutine_promise<Options,Result,Executor>::await_transform(
            use_coroutine_awaitable<std::tuple<Args...>,Initiation,InitArgs...>&& a) const noexcept
        {
            static_assert(Options&coroutine_option::save_awaiter,
                "use_coroutine can only be used in a coroutine that saves awaiter");
            using awaitable_t = use_coroutine_awaitable<std::tuple<Args...>,Initiation,InitArgs...>;
            struct awaitable : awaitable_t
            {
                struct handler : awaitable_t::handler
                {
                    using executor_type = prefer_result_t<Executor,
                        boost::asio::execution::relationship_t::continuation_t>;
                    using allocator_type = coroutine_promise::allocator_type;

                    const coroutine_promise* promise_;

                    executor_type get_executor() const noexcept
                    {
                        return boost::asio::prefer(promise_->get_executor(),
                            boost::asio::execution::relationship.continuation);
                    }

                    allocator_type get_allocator() const noexcept
                    {
                        return promise_->get_work_allocator();
                    }
                };

                const coroutine_promise* promise_;

                // This is here instead of base class to prevent it from being an awaiter
                // before it gets transformed.
                bool await_ready() noexcept
                {
                    return false;
                }

                void await_suspend(stdcoro::coroutine_handle<> handle)
                {
                    std::apply([&](auto&&... init_args){
                        this->initiation_(handler{{this,handle},promise_},
                            std::forward<decltype(init_args)>(init_args)...);
                    },this->init_args_);
                }
            };
            return awaitable{{std::move(a)},this};
        }
    }

    constexpr inline struct use_coroutine_t
    {
        template<typename Executor>
        class executor_with_default
            : public executor_wrapper<executor_with_default,Executor>
        {
            using base_t = executor_wrapper<executor_with_default,Executor>;
        public:
            using default_completion_token_type = use_coroutine_t;

            using base_t::base_t;
        };

        template<typename T>
        using as_default_on_t = typename T::template rebind_executor<
            executor_with_default<typename T::executor_type>>::other;

        template<typename T>
        static as_default_on_t<T> as_default_on(T io_object)
        {
            return as_default_on_t<T>{std::move(io_object)};
        }
    } use_coroutine;
}

template<typename... Args>
class boost::asio::async_result<ampi::use_coroutine_t,void (Args...)>
{
public:
    template<typename Initiation,typename... InitArgs>
    static ampi::detail::use_coroutine_awaitable<std::tuple<Args...>,Initiation,InitArgs...>
        initiate(Initiation initiation,ampi::use_coroutine_t,InitArgs... init_args)
    {
        return {std::move(initiation),std::forward_as_tuple(std::move(init_args)...)};
    }
};

#endif
