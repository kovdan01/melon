#ifndef UUID_C94FE859_CF75_4BC2_95AB_88610223A4DD
#define UUID_C94FE859_CF75_4BC2_95AB_88610223A4DD

#include <ce/asio_ns.hpp>

#include <boost/beast/core/stream_traits.hpp>

namespace ce
{
    namespace detail
    {
        template<typename Stream,typename = void>
        struct get_socket_impl
        {
            Stream& operator()(Stream& s) const noexcept
            {
                return s;
            }
        };

        template<typename Stream>
        struct get_socket_impl<Stream,std::void_t<decltype(std::declval<Stream&>().socket())>>
        {
            auto& operator()(Stream& s) const noexcept
            {
                return s.socket();
            }
        };
    }

    // beast::get_lowest_layer doesn't look through beast::tcp_stream::socket.
    template<typename Stream>
    auto& get_socket(Stream& s) noexcept
    {
        return detail::get_socket_impl<Stream>{}(bb::get_lowest_layer(s));
    }
}

#endif
