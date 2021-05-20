#include <melon/core.hpp>

#include <boost/functional/hash.hpp>

#include <array>
#include <random>

namespace melon::core
{

namespace detail
{

class RandomSingletone
{
public:
    static RandomSingletone& get_instance()
    {
        static RandomSingletone instance;
        return instance;
    }

    RandomSingletone(const RandomSingletone& root) = delete;
    RandomSingletone& operator=(const RandomSingletone&) = delete;
    RandomSingletone(RandomSingletone&& root) = delete;
    RandomSingletone& operator=(RandomSingletone&&) = delete;

    std::mt19937_64& prng()
    {
        return m_prng;
    }

    std::uniform_int_distribution<byte_t> byte_dist()
    {
        return m_byte_dist;
    }

private:
    RandomSingletone() = default;

    std::mt19937_64 m_prng{std::random_device{}()};
    std::uniform_int_distribution<byte_t> m_byte_dist{std::numeric_limits<byte_t>::min(),
                                                      std::numeric_limits<byte_t>::max()};
};

}  // namespace detail

class Token
{
public:
    static constexpr std::size_t BYTES_IN_TOKEN = 64;
    using token_t = std::array<byte_t, BYTES_IN_TOKEN>;

    Token()
    {
        auto& rand = detail::RandomSingletone::get_instance();
        for (byte_t& byte : m_token)
            byte = rand.byte_dist()(rand.prng());
    }

    [[nodiscard]] const token_t& token() const
    {
        return m_token;
    }

private:
    token_t m_token;
    std::time_t timestamp{std::time(nullptr)};
};

inline bool operator==(const Token& lhs, const Token& rhs) noexcept
{
    return lhs.token() == rhs.token();
}

struct TokenHasher
{
    [[nodiscard]] std::size_t operator()(const Token& x) const
    {
        std::size_t seed = 0;
        for (byte_t byte : x.token())
            boost::hash_combine<byte_t>(seed, byte);
        return seed;
    }
};

}  // namespace melon::core
