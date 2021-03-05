#ifndef UUID_CACA5936_A497_40B1_B28A_8B853A4A7009
#define UUID_CACA5936_A497_40B1_B28A_8B853A4A7009

#include <boost/asio/require.hpp>

namespace ampi
{
    template<typename T,typename... Properties>
    using require_result_t = typename boost::asio::require_result<T,Properties...>::type;
}

#endif
