#ifndef MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_
#define MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_

#include <melon/core/exception.hpp>
#include <ram_storage.hpp>

#include <QtSql/QtSql>
#include <QtWidgets>

namespace melon::client_desktop
{

class QtSqlException : public melon::Exception
{
public:
    using melon::Exception::Exception;
    ~QtSqlException() override;
};

void create_connection_with_db();

void exec_and_check_qtsql_query(QSqlQuery& qry, const QString& qry_string, const std::string& action);
void exec_and_check_qtsql_query(QSqlQuery& qry, const std::string& action);
void prepare_and_check_qtsql_query(QSqlQuery& qry, const QString& qry_string, const std::string& action);

class Domain : public ram::Domain
{
public:
    // For Insert
    Domain(const QString& hostname, bool external);
    // For Select
    Domain(const QString& hostname);

    void remove_from_db();
};

class User : public ram::User
{
public:
    // For Insert
    User(const QString& username, id_t domain_id, Status status);
    // For Select
    User(const QString& username, id_t domain_id);

    void remove_from_db();
};

class Message : public ram::Message
{
public:
    // For Insert
    Message(id_t chat_id, id_t domain_id_chat,
            id_t user_id, id_t domain_id_user,
            const QString& text, timestamp_t timestamp, Status status);
    // For Select
    Message(id_t message_id, id_t chat_id, id_t domain_id_chat);
    // For incomplete message
    Message(const QString& text);
    // for QVariant
    Message();

    [[nodiscard]] const QString& from() const noexcept;
    [[nodiscard]] bool is_edit() const noexcept;
    void remove_from_db();
    void set_text(const QString& text);
    void set_from();
    void set_is_edit(bool is_edit);

private:
    bool m_is_edit = false;
    QString m_from;
    void set_text(std::string text);  // hide base class function
};


class Chat : public ram::Chat
{
public:
    using message_handle_t = std::list<Message>::iterator;

    // For insert
    Chat(id_t domain_id, const QString& chatname);
    // For Select
    Chat(id_t chat_id, id_t domain_id);

    void remove_from_db();

    [[nodiscard]] const std::list<Message>& messages() const noexcept;
    [[nodiscard]] std::list<Message>& messages() noexcept;
    [[nodiscard]] bool empty() const;

    message_handle_t add_message(Message message);
    message_handle_t delete_message(message_handle_t message_handle);

    [[nodiscard]] message_handle_t last_message();
    [[nodiscard]] const Message& incomplete_message() const noexcept;

    void set_chatname(const QString& chatname);
    void set_incomplete_message(Message incomplete_message);

private:
    void set_chatname(std::string chatname);  // hide base class function
    std::list<Message> m_messages;
    Message m_incomplete_message;
};


class StorageSingletone
{
public:
    using chat_handle_t = std::list<Chat>::iterator;
    using const_chat_handle_t = std::list<Chat>::const_iterator;

    [[nodiscard]] static StorageSingletone& get_instance()
    {
        static StorageSingletone instance;
        return instance;
    }

    StorageSingletone(const StorageSingletone&) = delete;
    StorageSingletone& operator=(const StorageSingletone&) = delete;
    StorageSingletone(StorageSingletone&&) = delete;
    StorageSingletone& operator=(StorageSingletone&&) = delete;

    [[nodiscard]] const std::list<Chat>& chats() const;
    [[nodiscard]] std::list<Chat>& chats();

    chat_handle_t add_chat(Chat chat);
    chat_handle_t delete_chat(chat_handle_t chat_handle);

private:
    std::list<Chat> m_chats;

    StorageSingletone() = default;
    ~StorageSingletone() = default;
};

// This singletone is temporary to save two test users and one test domain
class UserDomainSingletone
{
public:
    [[nodiscard]] inline static UserDomainSingletone& get_instance()
    {
        static UserDomainSingletone instance;
        return instance;
    }

    UserDomainSingletone(const UserDomainSingletone&) = delete;
    UserDomainSingletone& operator=(const UserDomainSingletone&) = delete;
    UserDomainSingletone(UserDomainSingletone&&) = delete;
    UserDomainSingletone& operator=(UserDomainSingletone&&) = delete;

    [[nodiscard]] User me();
    [[nodiscard]] User another_user();
    [[nodiscard]] Domain my_domain();

private:
    User m_me{QStringLiteral("MelonUser"), 1};
    User m_another_user{QStringLiteral("SomeSender"), 1};
    Domain m_my_domain{QStringLiteral("melon")};

    UserDomainSingletone() = default;
    ~UserDomainSingletone() = default;
};

class DBSingletone
{
public:
    [[nodiscard]] inline static DBSingletone& get_instance()
    {
        static DBSingletone instance;
        return instance;
    }

    DBSingletone(const DBSingletone&) = delete;
    DBSingletone& operator=(const DBSingletone&) = delete;
    DBSingletone(DBSingletone&&) = delete;
    DBSingletone& operator=(DBSingletone&&) = delete;

    [[nodiscard]] QString db_name();

private:
    QString m_db_name = QStringLiteral("test_db");

    DBSingletone() = default;
    ~DBSingletone() = default;
};

}  // namespace melon::client_desktop

Q_DECLARE_METATYPE(melon::client_desktop::StorageSingletone::chat_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Chat::message_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Message)

#include "db_storage.ipp"
#endif  // MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_
