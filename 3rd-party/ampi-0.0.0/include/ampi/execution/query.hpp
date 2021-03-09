#ifndef UUID_1120BF46_5829_47A2_AD55_AB5DA064A8C4
#define UUID_1120BF46_5829_47A2_AD55_AB5DA064A8C4

#include <boost/asio/query.hpp>

namespace ampi
{
    template<typename T,typename Property>
    using query_result_t = typename boost::asio::query_result<T,Property>::type;
}

#endif
