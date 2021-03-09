#ifndef UUID_52998625_9971_474C_B3C3_3053D6F327FC
#define UUID_52998625_9971_474C_B3C3_3053D6F327FC

#include <boost/container/flat_map.hpp>
#include <boost/container_hash/hash_fwd.hpp>

template<typename Key,typename T,typename Compare,typename AllocatorOrContainer>
struct boost::hash<boost::container::flat_map<Key,T,Compare,AllocatorOrContainer>>
{
    std::size_t operator()(const boost::container::flat_map<Key,T,Compare,AllocatorOrContainer>& m)
            const noexcept
    {
        return boost::hash_range(m.begin(),m.end());
    }
};

template<typename Key,typename T,typename Compare,typename AllocatorOrContainer>
struct std::hash<boost::container::flat_map<Key,T,Compare,AllocatorOrContainer>>
    : boost::hash<boost::container::flat_map<Key,T,Compare,AllocatorOrContainer>> {};

#endif
