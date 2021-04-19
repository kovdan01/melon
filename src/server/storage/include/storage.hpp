#ifndef MELON_SERVER_STORAGE_HPP_
#define MELON_SERVER_STORAGE_HPP_

#include <melon/core/entities.hpp>
#include <melon/core/exception.hpp>
#include <melon/server/storage/wrappers/export.h>

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/update.h>

#include <stdexcept>

namespace melon::server::storage
{

class Domain;
class User;
class Chat;
class Message;

using id_t = melon::core::id_t;

STORAGE_WRAPPERS_EXPORT std::shared_ptr<sqlpp::mysql::connection_config> config_db();

class STORAGE_WRAPPERS_EXPORT IdNotFoundException : public melon::Exception
{
public:
    using melon::Exception::Exception;
    ~IdNotFoundException() override;
};

// The following functions throw IdNotFoundException if the requested entity is not found
STORAGE_WRAPPERS_EXPORT void check_if_domain_exists(sqlpp::mysql::connection& db, id_t domain_id);
STORAGE_WRAPPERS_EXPORT void check_if_domain_exists(sqlpp::mysql::connection& db, const std::string& hostname);
STORAGE_WRAPPERS_EXPORT void check_if_chat_exists(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id);

[[nodiscard]] STORAGE_WRAPPERS_EXPORT id_t max_user_id(sqlpp::mysql::connection& db, id_t domain_id);
[[nodiscard]] STORAGE_WRAPPERS_EXPORT id_t max_chat_id(sqlpp::mysql::connection& db, id_t domain_id);
[[nodiscard]] STORAGE_WRAPPERS_EXPORT id_t max_message_id(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id);

[[nodiscard]] STORAGE_WRAPPERS_EXPORT std::vector<std::string> get_all_usernames(sqlpp::mysql::connection& db);
[[nodiscard]] STORAGE_WRAPPERS_EXPORT std::vector<User> get_online_users(sqlpp::mysql::connection& db);

class STORAGE_WRAPPERS_EXPORT Domain : public melon::core::Domain
{
public:
    // For Insert
    Domain(sqlpp::mysql::connection& db, std::string hostname, bool external);
    // For Select
    Domain(sqlpp::mysql::connection& db, std::string hostname);
    // For Select by id
    Domain(sqlpp::mysql::connection& db, id_t domain_id);

    void remove();

protected:
    void set_external(bool external);

private:
    sqlpp::mysql::connection& m_db;
};

class STORAGE_WRAPPERS_EXPORT User : public melon::core::User
{
public:
    // For Insert
    User(sqlpp::mysql::connection& db, std::string username, id_t domain_id, Status status);
    // For Select
    User(sqlpp::mysql::connection& db, std::string username, id_t domain_id);
    // For Select by user_id and domain_id
    User(sqlpp::mysql::connection& db, id_t user_id, id_t domain_id);

    [[nodiscard]] std::vector<Chat> get_chats() const;

    void remove();

    void set_status(Status status);

private:
    sqlpp::mysql::connection& m_db;
};

class STORAGE_WRAPPERS_EXPORT Chat : public melon::core::Chat
{
public:
    // For Insert
    Chat(sqlpp::mysql::connection& db, id_t domain_id, std::string chatname);
    // For Select
    Chat(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id);

    [[nodiscard]] std::vector<User> get_users() const;
    [[nodiscard]] std::vector<Message> get_messages() const;

    void remove();

    void set_chatname(std::string chatname);

    void add_user(User& user);

private:
    sqlpp::mysql::connection& m_db;
};

class STORAGE_WRAPPERS_EXPORT Message : public melon::core::Message
{
public:
    // For Insert
    Message(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id_chat, id_t user_id, id_t domain_id_user,
            std::string text, std::chrono::system_clock::time_point timestamp, Status status);
    // For Select
    Message(sqlpp::mysql::connection& db, id_t message_id, id_t chat_id, id_t domain_id_chat);

    void remove();

    void set_text(std::string text);
    void set_status(Status status);
    void set_timestamp(timestamp_t timestamp);

private:
    sqlpp::mysql::connection& m_db;
};

}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_HPP_
