#ifndef MELON_SERVER_STORAGE_HPP_
#define MELON_SERVER_STORAGE_HPP_

#include <melon/core/entities.hpp>
#include <melon/core/exception.hpp>

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/update.h>

#include <stdexcept>

namespace melon::server::storage
{

using id_t = melon::core::id_t;

std::shared_ptr<sqlpp::mysql::connection_config> config_db();

class IdNotFoundException : public melon::Exception
{
public:
    using melon::Exception::Exception;
    using melon::Exception::operator=;
    ~IdNotFoundException() override;
};

// The following functions throw IdNotFoundException if the requested entity is not found
void check_if_domain_exists(sqlpp::mysql::connection& db, id_t domain_id);
void check_if_chat_exists(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id);

[[nodiscard]] id_t max_user_id(sqlpp::mysql::connection& db, id_t domain_id);
[[nodiscard]] id_t max_chat_id(sqlpp::mysql::connection& db, id_t domain_id);
[[nodiscard]] id_t max_message_id(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id);

[[nodiscard]] std::vector<std::string> get_all_usernames(sqlpp::mysql::connection& db);
[[nodiscard]] std::vector<melon::core::User::Ptr> get_online_users(sqlpp::mysql::connection& db);

class Domain : public melon::core::Domain
{
public:
    // For Insert
    Domain(sqlpp::mysql::connection& db, std::string hostname, bool external);
    // For Select
    Domain(sqlpp::mysql::connection& db, std::string hostname);

    void remove();

protected:
    void set_external_impl(bool external) override;

private:
    sqlpp::mysql::connection& m_db;
};

class User : public melon::core::User
{
public:
    // For Insert
    User(sqlpp::mysql::connection& db, std::string username, id_t domain_id, Status status);
    // For Select
    User(sqlpp::mysql::connection& db, std::string username, id_t domain_id);

    [[nodiscard]] std::vector<melon::core::Chat::Ptr> get_chats() const override;

    void remove();

protected:
    void set_status_impl(Status status) override;

private:
    sqlpp::mysql::connection& m_db;
};

class Chat : public melon::core::Chat
{
public:
    // For Insert
    Chat(sqlpp::mysql::connection& db, id_t domain_id, std::string chatname);
    // For Select
    Chat(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id);

    [[nodiscard]] std::vector<melon::core::User::Ptr> get_users() const override;
    [[nodiscard]] std::vector<melon::core::Message::Ptr> get_messages() const override;

    void remove();

protected:
    void set_chatname_impl(const std::string& chatname) override;

private:
    sqlpp::mysql::connection& m_db;
};

class Message : public melon::core::Message
{
public:
    // For Insert
    Message(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id_chat, id_t user_id, id_t domain_id_user,
            std::string text, std::chrono::system_clock::time_point timestamp, Status status);
    // For Select
    Message(sqlpp::mysql::connection& db, id_t message_id, id_t chat_id, id_t domain_id_chat);

    void remove();

protected:
    void set_text_impl(const std::string& text) override;
    void set_status_impl(Status status) override;
    void set_timestamp_impl(timestamp_t timestamp) override;

private:
    sqlpp::mysql::connection& m_db;
};

}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_HPP_
