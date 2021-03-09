#ifndef UUID_B5953537_6D38_402D_932F_661C0C7E41AF
#define UUID_B5953537_6D38_402D_932F_661C0C7E41AF

#include <boost/container_hash/hash_fwd.hpp>

#include <span>

template<typename T,std::size_t N>
struct boost::hash<std::span<T,N>>
{
    std::size_t operator()(std::span<T,N> s) const noexcept
    {
        return boost::hash_range(s.begin(),s.end());
    }    
};

template<typename T,std::size_t N>
struct std::hash<std::span<T,N>> : boost::hash<std::span<T,N>> {};

#endif
