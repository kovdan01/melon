#ifndef MELON_SERVER_STORAGE_HPP_
#define MELON_SERVER_STORAGE_HPP_

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

#include "melondb.h"
#include <melon/storage_class.hpp>

namespace mc = melon::core;

const melon::Users G_USERS;
const melon::Messages G_MESSAGES;
const melon::Chats G_CHATS;

namespace melon::server::storage
{

/* Init and connect to db */

std::shared_ptr<sqlpp::mysql::connection_config> config_melondb();

/* Users */

void add_user(sqlpp::mysql::connection& db, const mc::User& user);
std::vector<std::string> get_online_users_names(sqlpp::mysql::connection& db);
std::vector<mc::User> get_online_users(sqlpp::mysql::connection& db);
void make_user_online(sqlpp::mysql::connection& db, const mc::User& user);
void make_user_offline(sqlpp::mysql::connection& db, const mc::User& user);

/* Messages */

void add_message(sqlpp::mysql::connection& db, const mc::Message& message);

/* Chat */

void add_chat(sqlpp::mysql::connection& db, const mc::Chat& chat);
std::vector<mc::Message> get_messages_for_chat(sqlpp::mysql::connection& db, const mc::Chat& chat);


}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_HPP_
