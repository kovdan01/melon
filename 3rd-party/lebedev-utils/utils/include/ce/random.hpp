#ifndef UUID_03295679_36EB_46EA_A4EE_F16E6D88D90C
#define UUID_03295679_36EB_46EA_A4EE_F16E6D88D90C

#include <ce/aligned_allocator.hpp>
#include <ce/utils/export.h>

#include <boost/container/vector.hpp>

namespace ce
{
    using byte_vector = boost::container::vector<std::byte,
                                                 ce::aligned_allocator<std::byte,32>>;

    CE_UTILS_EXPORT byte_vector random_bytes(std::size_t n,std::uint64_t seed = 0);
}

#endif
