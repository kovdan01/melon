#ifndef MELON_SERVER_STORAGE_CONFIG_DB_HPP_
#define MELON_SERVER_STORAGE_CONFIG_DB_HPP_

#include <melon/server/storage/entities/export.h>

#include <sqlpp11/mysql/mysql.h>

namespace melon::server::storage
{

MELON_STORAGE_ENTITIES_EXPORT std::shared_ptr<sqlpp::mysql::connection_config> config_db();

}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_CONFIG_DB_HPP_
