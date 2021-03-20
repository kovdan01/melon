#ifndef UUID_993B3294_A113_406A_83E2_7AB28151A3AE
#define UUID_993B3294_A113_406A_83E2_7AB28151A3AE

#include <ampi/serialize/serializable.hpp>

namespace ampi
{
    template<deserializable T,serializable U>
    void transmute(T& x,const U& y)
    {
        auto dmr = boost::container::pmr::get_default_resource();
        segmented_stack_resource ssr1{dmr},ssr2{dmr};
        auto get_executor = [](boost::container::pmr::memory_resource& mr){
            return boost::asio::require(boost::asio::system_executor{},
                boost::asio::execution::allocator(
                    boost::container::pmr::polymorphic_allocator<std::byte>{&mr}));
        };
        auto source = serial_event_source(type_tag<U>)(get_executor(ssr1),y);
        auto sink = serial_event_sink(type_tag<T>)(get_executor(ssr2),source,x);
        boost::outcome_v2::boost_result<void,std::exception_ptr> res = boost::outcome_v2::success();
        [](auto& source,auto& sink) -> coroutine<void> {
            co_await std::move(sink);
            auto e = co_await source;
            if(e)
                throw structure_error{structure_error::reason_t::unexpected_event,
                                    boost::typeindex::type_id<T>(),{},std::move(e)};
        }(source,sink).async_run(
            [&](boost::outcome_v2::boost_result<void,std::exception_ptr> r){
                res = std::move(r);
            }
        );
        res.value();
    }
    
    template<deserializable T,serializable U>
    T transmute(const U& x)
    {
        T y;
        transmute(y,x);
        return y;
    }
}

#endif
