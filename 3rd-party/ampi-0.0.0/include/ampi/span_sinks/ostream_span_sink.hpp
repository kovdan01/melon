#ifndef UUID_DB3AA6D4_A3A2_492A_9C1F_E730BBD24187
#define UUID_DB3AA6D4_A3A2_492A_9C1F_E730BBD24187

#include <ampi/span_sources/span_source.hpp>

#include <boost/container/container_fwd.hpp>
#include <ostream>

namespace ampi
{
    class ostream_span_sink
    {
    public:
        ostream_span_sink(std::ostream& stream,size_t buffer_size = buffer::default_size) noexcept
            : stream_{&stream},
              buf_{buffer_size,boost::container::default_init}
        {}

        buffer get_buffer(size_t min_size = 0) noexcept
        {
            if(buf_.size()<min_size)
                buf_.resize(min_size,boost::container::default_init);
            return buffer{buf_};
        }

        coroutine<> operator()(span_source auto& ss) [[clang::lifetimebound]]
        {
            auto ss_coro = ss(*this);
            while(auto buf = co_await ss_coro())
                if(!stream_->write(reinterpret_cast<const char*>(buf->data()),
                                   std::streamsize(buf->size())))
                    co_return;
        }
    private:
        std::ostream* stream_;
        vector<byte> buf_;
    };
}

#endif
