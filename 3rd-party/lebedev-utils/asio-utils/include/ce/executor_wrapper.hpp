#ifndef UUID_C670691C_D741_4325_AF61_4F0D9B907613
#define UUID_C670691C_D741_4325_AF61_4F0D9B907613

#include <ce/asio_ns.hpp>

#include <boost/asio/execution/execute.hpp>
#include <boost/asio/prefer.hpp>
#include <boost/asio/query.hpp>
#include <boost/asio/require.hpp>
#include <boost/asio/require_concept.hpp>

#include <utility>

namespace ce
{
    template<template <typename> typename DerivedTemplate,typename Executor>
    class executor_wrapper
    {
    protected:
        Executor ex_;
    public:
        using inner_executor_type = Executor;

        executor_wrapper(Executor ex) noexcept
            : ex_{std::move(ex)}
        {}

        const inner_executor_type& get_inner_executor() const noexcept
        {
            return ex_;
        }

        bool operator==(const executor_wrapper& other) const noexcept = default;

        template<typename F>
        std::enable_if_t<bae::can_execute_v<const Executor&,F>> execute(F&& f) const
        {
            bae::execute(ex_,std::forward<F>(f));
        }

        template<typename Property>
        std::enable_if_t<ba::can_query_v<const Executor&,Property>,
                         typename ba::query_result<const Executor&,Property>::type>
            query(const Property& prop) const
            noexcept(ba::is_nothrow_query_v<const Executor&,Property>)
        {
            return ba::query(ex_,prop);
        }

        template<typename Property>
        std::enable_if_t<ba::can_require_v<const Executor&,Property>,
                         DerivedTemplate<std::decay_t<
                             typename ba::require_result<const Executor&,Property>::type>>>
            require(const Property& prop) const
            noexcept(ba::is_nothrow_require_v<const Executor&,Property>)
        {
            return {ba::require(ex_,prop),
                    static_cast<const DerivedTemplate<Executor>&>(*this)};
        }

        template<typename Property>
        std::enable_if_t<ba::can_prefer_v<const Executor&,Property>,
                         DerivedTemplate<std::decay_t<
                             typename ba::prefer_result<const Executor&,Property>::type>>>
            prefer(const Property& prop) const
            noexcept(ba::is_nothrow_prefer_v<const Executor&,Property>)
        {
            return {ba::prefer(ex_,prop),
                    static_cast<const DerivedTemplate<Executor>&>(*this)};
        }
    };
}

#endif
