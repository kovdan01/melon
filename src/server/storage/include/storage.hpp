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


/* class Domain */

class Domain : public melon::core::Domain
{
public:
    // For Insert
    Domain(sqlpp::mysql::connection& db, std::string hostname, bool external);
    // For Select
    Domain(sqlpp::mysql::connection& db, std::string hostname);

    void remove();

private:
    sqlpp::mysql::connection& m_db;
};


/* class Message */

class Message : public melon::core::Message
{
public:
    // For Insert
    Message(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::uint64_t user_id, std::uint64_t chat_id,
            std::string text, Status status);
    // For Select
    Message(sqlpp::mysql::connection& db, std::uint64_t message_id, std::uint64_t domain_id, std::uint64_t chat_id);

    void set_text(const std::string new_text) override;
    void set_status(Status new_status) override;
    void remove();

private:
    sqlpp::mysql::connection& m_db;
};


/* class Chat */

class Chat : public melon::core::Chat
{
public:
    // For Insert
    Chat(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string chatname);
    // For Select
    Chat(sqlpp::mysql::connection& db, std::uint64_t chat_id, std::uint64_t domain_id);

    void set_chatname(const std::string new_chatname) override;
    void remove();

private:
    sqlpp::mysql::connection& m_db;
};


/* class User */

class User : public melon::core::User
{
public:
    // For Insert
    User(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string username, Status status);
    // For Select
    User(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string username);

    void remove();
    void set_status(Status new_status) override;

private:
    sqlpp::mysql::connection& m_db;
};

std::uint64_t max_domain_id(sqlpp::mysql::connection& db);
std::uint64_t max_message_id(sqlpp::mysql::connection& db);
std::uint64_t max_chat_id(sqlpp::mysql::connection& db);
std::uint64_t max_user_id(sqlpp::mysql::connection& db);


/* Domains */

std::uint64_t get_domain_id_by_hostname(sqlpp::mysql::connection& db, const std::string& hostname);

/* Users */

std::vector<melon::core::Chat> get_chats_for_user(sqlpp::mysql::connection& db, melon::core::User user);
std::vector<std::string> get_names_of_all_users(sqlpp::mysql::connection& db);
std::vector<std::string> get_online_users_names(sqlpp::mysql::connection& db);
std::vector<melon::core::User> get_online_users(sqlpp::mysql::connection& db);

/* Chats */

std::vector<melon::core::User> get_users_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat);
std::vector<melon::core::Message> get_messages_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat);


}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_HPP_
