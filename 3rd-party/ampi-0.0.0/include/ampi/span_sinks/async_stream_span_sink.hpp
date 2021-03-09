#ifndef UUID_5E84D5A1_AA30_42B1_8AD3_D4A000BCA4EA
#define UUID_5E84D5A1_AA30_42B1_8AD3_D4A000BCA4EA

#include <ampi/coro/use_coroutine.hpp>
#include <ampi/pmr/reusable_monotonic_buffer_resource.hpp>
#include <ampi/span_sinks/span_sink.hpp>

#include <boost/asio/write.hpp>
#include <boost/container/pmr/global_resource.hpp>

namespace ampi
{
    template<typename AsyncWriteStream>
    class async_stream_span_sink
    {
    public:
        using executor_type = typename AsyncWriteStream::executor_type;

        async_stream_span_sink(AsyncWriteStream& stream,size_t n_iovecs = 128) noexcept
            : stream_{&stream},
              n_iovecs_{n_iovecs}
        {
            assert(n_iovecs);
            iovecs_.reserve(n_iovecs);
        }

        executor_type get_executor() const noexcept
        {
            return stream_->get_executor();
        }

        buffer get_buffer(size_t min_size = buffer::default_size)
        {
            return {{static_cast<byte*>(mr_.allocate(min_size)),min_size}};
        }

        template<span_source SpanSource>
        coroutine<void,executor_type> operator()(SpanSource& ss) [[clang::lifetimebound]]
        {
            return [](executor_type ex,async_stream_span_sink& this_,SpanSource& ss)
                    -> coroutine<void,executor_type> {
                auto ss_coro = ss(this_);
                auto drain = [](executor_type,async_stream_span_sink& this_)
                        -> async_subgenerator<std::nullptr_t,executor_type> {
                    for(;;){
                        co_await boost::asio::async_write(*this_.stream_,this_.iovecs_,use_coroutine);
                        this_.iovecs_.clear();
                        this_.mr_.reuse();
                        co_yield {};
                    }
                }(ex,this_);
                while(auto buf = co_await ss_coro()){
                    if(!this_.iovecs_.empty()&&!buf->allocator()){
                        buffer& last = this_.iovecs_.back();
                        if(!last.allocator()&&last.data()+last.size()==buf->data()){
                            last = {last.data(),last.size()+buf->size()};
                            continue;
                        }
                    }
                    this_.iovecs_.push_back(std::move(*buf));
                    if(this_.iovecs_.size()==this_.n_iovecs_)
                        co_await drain;
                }
                if(!this_.iovecs_.empty())
                    co_await drain;
            }(get_executor(),*this,ss);
        }
    private:
        AsyncWriteStream* stream_;
        vector<buffer> iovecs_;
        size_t n_iovecs_;
        reusable_monotonic_buffer_resource mr_{boost::container::pmr::get_default_resource()};
    };
}

#endif
