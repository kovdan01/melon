#ifndef YAML_CONFIG_HPP
#define YAML_CONFIG_HPP

#include <melon/core/export.h>
#include <string>
#include <vector>
#include <filesystem>

namespace melon::core::yaml_conf
{
const std::string conf_filename = "m_config.yaml";

/*enum class Services // supported service types
 {
     SERVER_SERVER,
     AUTH,
     CLIENT_SERVER,
     STORAGE,
     MESSAGING
 };

enum class Locations // supported location types
 {
    LOCAL,
    REMOTE
 };
*/

namespace fs = std::filesystem;

struct networking {
    std::string protocol;
    std::string ip;
    uint64_t port;
};
struct location {
    std::string service_kind;
    std::string location_kind;
    fs::path path;
    std::string protocol;
    std::string ip;
    uint64_t port;
};


/*namespace server_conf {
enum class Policies // supported location types
 {
    DIGEST_MD5,
    PLAIN
 };
}*/

// returns a vector of missing strings
MELON_CORE_EXPORT std::vector<std::string> check_reqired_params(const std::vector<std::string> & keys );
MELON_CORE_EXPORT std::vector<std::string> check_superfluous_params(const std::vector<std::string> & keys );
MELON_CORE_EXPORT std::pair<networking, std::vector<location>> parse_common_params();
}  // namespace melon::core


#endif // YAML_CONFIG_HPP
