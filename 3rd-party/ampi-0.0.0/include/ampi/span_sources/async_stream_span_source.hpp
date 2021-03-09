#ifndef UUID_41E57842_C9B6_4BFC_AB6E_9BE046FD20B1
#define UUID_41E57842_C9B6_4BFC_AB6E_9BE046FD20B1

#include <ampi/span_sources/span_source.hpp>

#include <boost/asio/experimental/as_single.hpp>
#include <boost/asio/read.hpp>
#include <boost/system/system_error.hpp>

namespace ampi
{
    template<typename AsyncReadStream>
    class async_stream_span_source
    {
    public:
        using executor_type = typename AsyncReadStream::executor_type;

        async_stream_span_source(AsyncReadStream& stream) noexcept
            : stream_{&stream}
        {}

        executor_type get_executor() const noexcept
        {
            return stream_->get_executor();
        }

        template<buffer_factory BufferFactory>
        async_generator<buffer,executor_type> operator()(BufferFactory& bf) /*[[clang::lifetimebound]]*/
        {
            return [](executor_type,AsyncReadStream& stream,BufferFactory& bf)
                    -> async_generator<buffer,executor_type> {
                for(;;){
                    auto buf = bf.get_buffer();
                    auto [ec,n] = boost::asio::async_read(stream,buf,
                        boost::asio::experimental::as_single(use_coroutine));
                    if(ec&&ec!=boost::asio::error::eof)
                        throw boost::system::system_error(ec);
                    if(n)
                        co_yield buffer(std::move(buf),0,n);
                    if(ec==boost::asio::error::eof)
                        co_return;
                }
            }(get_executor(),*stream_,bf);
        }
    private:
        AsyncReadStream* stream_;
    };
}

#endif
