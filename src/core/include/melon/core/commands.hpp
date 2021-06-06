#include <cstdint>

namespace melon::core
{

enum class ClientCode : std::uint32_t
{
    SEND_TOKEN = 0,
    REQUEST_TOKEN = 1,
};

enum class ServerCode : std::uint32_t
{
    TOKEN_OK = 0,
    TOKEN_BAS = 1,
    ISSUED_A_TOKEN = 2,
    AUTH_PROBLEM = 3,
    SEND_SASL_DATA = 4,
};

}  // namespace melon::core
