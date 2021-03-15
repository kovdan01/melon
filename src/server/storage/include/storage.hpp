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

/* Should I also create Chats_Users class?  or should I fill values of Chats_Users table in MariaDB inside Chats, Users, Messages*/

/* class Domain */

class Domain : public melon::core::Domain
{
public:
    Domain(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string& hostname, bool external);

    void remove();

private:
    sqlpp::mysql::connection& m_db;
    std::uint64_t m_this_domain_id;
};


/* class Message */

class Message : public melon::core::Message
{
public:
    Message(sqlpp::mysql::connection& db, std::uint64_t message_id, std::uint64_t domain_id, std::uint64_t user_id,
            std::uint64_t chat_id, std::string text, Status status);

    Message(sqlpp::mysql::connection& db, std::uint64_t message_id, std::uint64_t domain_id,
            std::uint64_t user_id, std::uint64_t chat_id, std::string text, Status status,
            const std::string& hostname, const std::string& username, const std::string& chatname);

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
    Chat(sqlpp::mysql::connection& db, std::uint64_t chat_id, std::uint64_t domain_id, std::string chatname);

    Chat(sqlpp::mysql::connection& db, std::uint64_t chat_id, std::uint64_t domain_id,
         std::string chatname, const std::string& hostname);

    void set_chatname(const std::string new_chatname) override;
    void remove();

private:
    sqlpp::mysql::connection& m_db;
};


/* class User */

class User : public melon::core::User
{
public:
    User(sqlpp::mysql::connection& db, std::uint64_t user_id, std::uint64_t domain_id, std::string username, Status status);

    User(sqlpp::mysql::connection& db, std::uint64_t user_id, std::uint64_t domain_id,
         std::string username, Status status, const std::string& hostname);

    void remove();
    void set_status(Status new_status) override;

private:
    sqlpp::mysql::connection& m_db;
};


/* Domains */

std::uint64_t get_domain_id_by_hostname(sqlpp::mysql::connection& db, const std::string& hostname);

/* Users */

std::uint64_t get_user_id_by_username_and_domain_id(sqlpp::mysql::connection& db, const std::string& username, std::uint64_t domain_id);
std::vector<std::string> get_names_of_all_users(sqlpp::mysql::connection& db);
std::vector<std::string> get_online_users_names(sqlpp::mysql::connection& db);
std::vector<melon::core::User> get_online_users(sqlpp::mysql::connection& db);

/* Messages */

std::uint64_t get_message_id_by_chat_id_and_domain_id_and_user_id(sqlpp::mysql::connection& db, std::uint64_t chat_id,
                                                                  std::uint64_t domain_id, std::uint64_t user_id);

/* Chats */

std::uint64_t get_chat_id_by_chatname_and_domain_id(sqlpp::mysql::connection& db, const std::string& chatname, std::uint64_t domain_id);
std::vector<melon::core::Message> get_messages_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat);


}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_HPP_
