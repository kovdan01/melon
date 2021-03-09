#ifndef UUID_88A96BEE_28F1_4081_8101_D20CDB973C3E
#define UUID_88A96BEE_28F1_4081_8101_D20CDB973C3E

#include <ampi/execution/prefer.hpp>
#include <ampi/execution/query.hpp>
#include <ampi/execution/require.hpp>

#include <boost/asio/execution/execute.hpp>

#include <utility>

namespace ampi
{
    template<template <typename> typename DerivedTemplate,typename Executor>
        requires boost::asio::execution::is_executor_v<Executor>
    class executor_wrapper
    {
    protected:
        Executor ex_;
    public:
        using inner_executor_type = Executor;

        executor_wrapper(Executor ex) noexcept
            : ex_{std::move(ex)}
        {}

        template<typename OtherExecutor>
        executor_wrapper(Executor ex,DerivedTemplate<OtherExecutor> /*other*/) noexcept
            : ex_{std::move(ex)}
        {}

        const inner_executor_type& get_inner_executor() const noexcept
        {
            return ex_;
        }

        bool operator==(const executor_wrapper& other) const noexcept = default;

        template<typename F>
            requires boost::asio::execution::can_execute_v<const Executor&,F>
        void execute(F&& f) const
        {
            boost::asio::execution::execute(ex_,std::forward<F>(f));
        }

        template<typename Property>
            requires boost::asio::can_query_v<const Executor&,Property>
        query_result_t<const Executor&,Property> query(const Property& prop) const
            noexcept(boost::asio::is_nothrow_query_v<const Executor&,Property>)
        {
            return boost::asio::query(ex_,prop);
        }

        template<typename Property>
            requires boost::asio::can_require_v<const Executor&,Property>
        DerivedTemplate<std::decay_t<require_result_t<const Executor&,Property>>>
            require(const Property& prop) const
            noexcept(boost::asio::is_nothrow_require_v<const Executor&,Property>)
        {
            return {boost::asio::require(ex_,prop),
                    static_cast<const DerivedTemplate<Executor>&>(*this)};
        }

        template<typename Property>
            requires boost::asio::can_prefer_v<const Executor&,Property>
        DerivedTemplate<std::decay_t<prefer_result_t<const Executor&,Property>>>
            prefer(const Property& prop) const
            noexcept(boost::asio::is_nothrow_prefer_v<const Executor&,Property>)
        {
            return {boost::asio::prefer(ex_,prop),
                    static_cast<const DerivedTemplate<Executor>&>(*this)};
        }
    };
}

#endif
