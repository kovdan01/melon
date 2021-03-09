#ifndef UUID_BB7698D8_1AF9_4061_B605_C41C6F472CB1
#define UUID_BB7698D8_1AF9_4061_B605_C41C6F472CB1

#include <ampi/span_sources/span_source.hpp>

#include <istream>

namespace ampi
{
    class istream_span_source
    {
    public:
        istream_span_source(std::istream& stream) noexcept
            : stream_{&stream}
        {}

        generator<buffer> operator()(buffer_factory auto& bf) [[clang::lifetimebound]]
        {
            while(*stream_){
                auto buf = bf.get_buffer();
                if(!stream_->read(reinterpret_cast<char*>(buf.data()),std::streamsize(buf.size())))
                    co_return;
                co_yield {std::move(buf),0,size_t(stream_->gcount())};
            }
        }
    private:
        std::istream* stream_;
    };
}

#endif
