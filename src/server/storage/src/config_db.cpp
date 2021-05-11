#include <config_db.hpp>

#include <sqlpp11/mysql/mysql.h>

namespace melon::server::storage
{

std::shared_ptr<sqlpp::mysql::connection_config> config_db()
{
    auto config = std::make_shared<sqlpp::mysql::connection_config>();

    static auto fromenv = [](const std::string& env_name, const std::string& default_value) -> std::string
    {
        const char* env_value = std::getenv(env_name.c_str());
        return (env_value == nullptr ? default_value : std::string(env_value));
    };
    config->user     = fromenv("DB_USER",     "melon");
    config->password = fromenv("DB_PASSWORD", "melonpass");
    config->database = fromenv("DB_DATABASE", "melon");
    config->host     = fromenv("DB_HOST",     "localhost");

    static constexpr int default_port = 3306;
    const char* env_value = std::getenv("DB_PORT");
    config->port = (env_value == nullptr ? default_port : std::stoi(env_value));

#ifndef NDEBUG
    config->debug = true;
#else
    config->debug = false;
#endif
    return config;
}

}  // namespace melon::server::storage
