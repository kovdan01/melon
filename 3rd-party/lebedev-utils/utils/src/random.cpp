#include <ce/random.hpp>

#include <random>

namespace ce
{
    byte_vector random_bytes(std::size_t n,std::uint64_t seed)
    {
        byte_vector v(n,boost::container::default_init);
        using prng_t = std::mt19937_64;
        std::independent_bits_engine<prng_t,8,std::uint8_t> ibe{prng_t{seed}};
        std::generate(v.begin(),v.end(),[&]{
            return std::byte{ibe()};
        });
        return v;
    }
}
