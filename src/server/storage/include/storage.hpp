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

/*
 * 5. make arrayy of users in chat
 * 13. change status of message for sent->recieved->seen
 * sets last_login time to the current timestamp
 * sorted messengers of chat
 *
 * /


/* Init and connect to db */

std::shared_ptr<sqlpp::mysql::connection_config> config_melondb();

/* Domains */

void add_domain(sqlpp::mysql::connection& db, const std::string& searched_hostname);
void add_domain(sqlpp::mysql::connection& db, const melon::core::Domain& domain);
std::uint64_t get_domain(sqlpp::mysql::connection& db, const std::string& searched_hostname);
std::uint64_t find_or_insert_domain_id(sqlpp::mysql::connection& db, const std::string& searched_hostname);
//std::uint64_t find_domain_id(sqlpp::mysql::connection& db, std::string searched_hostname);
void remove_domain(sqlpp::mysql::connection& db, const melon::core::Domain& domain);


/* Users */

void count_users(sqlpp::mysql::connection& db);
std::vector<std::string> get_names_of_all_users(sqlpp::mysql::connection& db);
void add_user(sqlpp::mysql::connection& db, const melon::core::User& user, const std::string& searched_hostname);
void remove_user(sqlpp::mysql::connection& db, const melon::core::User& user);
std::vector<std::string> get_online_users_names(sqlpp::mysql::connection& db);
std::vector<melon::core::User> get_online_users(sqlpp::mysql::connection& db);
void make_user_online(sqlpp::mysql::connection& db, const melon::core::User& user);
void make_user_offline(sqlpp::mysql::connection& db, const melon::core::User& user);


/* Messages */

void count_number_recieved_messages (sqlpp::mysql::connection& db);
void add_message(sqlpp::mysql::connection& db, const melon::core::Message& message);
void update_text(sqlpp::mysql::connection& db, const std::string& new_text, const melon::core::Message& message);
void update_status_for_seen(sqlpp::mysql::connection& db, const melon::core::Message& message);
void remove_message(sqlpp::mysql::connection& db, const melon::core::Message& message);

/* Chat */

void add_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat, std::string searched_hostname);
void remove_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat);
void update_chatname(sqlpp::mysql::connection& db, const std::string& new_chatname, const melon::core::Chat& chat);
std::vector<melon::core::Message> get_messages_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat);


void find(sqlpp::mysql::connection& db, std::string searched_hostname);

}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_HPP_
