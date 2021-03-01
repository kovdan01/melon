#ifndef MELON_SERVER_STORAGE_HPP_
#define MELON_SERVER_STORAGE_HPP_

#include <melon/core/storage_class.hpp>

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

namespace melon::server::storage
{

/* Init and connect to db */

std::shared_ptr<sqlpp::mysql::connection_config> config_melondb();

/* Users */

void add_user(sqlpp::mysql::connection& db, const melon::core::User& user);
std::vector<std::string> get_online_users_names(sqlpp::mysql::connection& db);
std::vector<melon::core::User> get_online_users(sqlpp::mysql::connection& db);
void make_user_online(sqlpp::mysql::connection& db, const melon::core::User& user);
void make_user_offline(sqlpp::mysql::connection& db, const melon::core::User& user);

/* Messages */

void add_message(sqlpp::mysql::connection& db, const melon::core::Message& message);

/* Chat */

void add_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat);
std::vector<melon::core::Message> get_messages_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat);

}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_HPP_
