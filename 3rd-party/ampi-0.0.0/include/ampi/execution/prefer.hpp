#ifndef UUID_F227599B_0795_4273_87FB_787D0804A121
#define UUID_F227599B_0795_4273_87FB_787D0804A121

#include <boost/asio/prefer.hpp>

namespace ampi
{
    template<typename T,typename... Properties>
    using prefer_result_t = typename boost::asio::prefer_result<T,Properties...>::type;
}

#endif
