#ifndef CONFIG_DB_HPP
#define CONFIG_DB_HPP

#include <melon/server/storage/entities/export.h>

#include <sqlpp11/mysql/mysql.h>

namespace melon::server::storage
{

MELON_STORAGE_ENTITIES_EXPORT std::shared_ptr<sqlpp::mysql::connection_config> config_db();

}  // namespace melon::server::storage

#endif // CONFIG_DB_HPP
