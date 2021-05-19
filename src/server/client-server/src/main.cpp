#include <melon/core.hpp>
#include <melon/core/entities.hpp>

#include <ctime>
#include <functional>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>


struct Token
{
    Token() : timestamp(std::time(nullptr)), mt(std::time(nullptr))
    {
        token = mt();
    }
    std::uint64_t token;
    std::time_t timestamp;
    std::mt19937_64 mt;
    bool friend operator==(const Token& lhs, const Token& rhs) noexcept;
};

bool operator==(const Token& lhs, const Token& rhs) noexcept
{
    return lhs.token == rhs.token && lhs.timestamp == rhs.timestamp;
}

template<>
struct std::hash<Token>
{
    size_t operator()(const Token& x) const
    {
        return std::hash<uint64_t>{}(x.token);
    }
};



std::unordered_map<melon::core::User, std::unordered_set<Token>> SessionTracker;

int main()
{
    melon::core::hello();


    return 0;
}
