#ifndef MELON_CLIENT_DESKTOP_ENTITIES_DB_HPP_
#define MELON_CLIENT_DESKTOP_ENTITIES_DB_HPP_

#include <entities_qt.hpp>
#include <melon/core/exception.hpp>

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

void exec_and_check_qtsql_query(QSqlQuery& qry, const QString& qry_string, const std::string& action);
void exec_and_check_qtsql_query(QSqlQuery& qry, const std::string& action);
void prepare_and_check_qtsql_query(QSqlQuery& qry, const QString& qry_string, const std::string& action);

class Domain : public detail::Domain
{
public:
    // For Insert
    Domain(const QString& hostname, bool external);
    // For Select
    Domain(const QString& hostname);
    // To be initialized in DBStorage constructor
    Domain() = default;

    void remove_from_db();
};

class User : public detail::User
{
public:
    // For Insert
    User(const QString& username, id_t domain_id, Status status);
    // For Select
    User(const QString& username, id_t domain_id);
    // To be initialized in DBStorage constructor
    User() = default;

    void remove_from_db();
};

class Message : public detail::Message
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
    // For QVariant
    Message() = default;

    [[nodiscard]] const QString& from() const noexcept;
    [[nodiscard]] bool is_edit() const noexcept;
    void set_text(const QString& text);

    void remove_from_db();

private:
    bool m_is_edit = false;
    QString m_from;

    void set_text(std::string text);  // hide base class function
    void set_from();
    void set_is_edit(bool is_edit);
};


class Chat : public detail::Chat
{
public:
    using message_handle_t = std::list<Message>::iterator;

    // For insert
    Chat(id_t domain_id, const QString& chatname);
    // For Select
    Chat(id_t chat_id, id_t domain_id);

    [[nodiscard]] const std::list<Message>& messages() const noexcept;
    [[nodiscard]] std::list<Message>& messages() noexcept;
    [[nodiscard]] bool empty() const noexcept;

    message_handle_t add_message(Message message);
    message_handle_t delete_message(message_handle_t message_handle);

    [[nodiscard]] message_handle_t last_message();
    [[nodiscard]] const Message& incomplete_message() const noexcept;

    void set_chatname(const QString& chatname);
    void set_incomplete_message(Message incomplete_message);

    void remove_from_db();

private:
    void set_chatname(std::string chatname);  // hide base class function
    std::list<Message> m_messages;
    Message m_incomplete_message;
};

}  // namespace melon::client_desktop

Q_DECLARE_METATYPE(melon::client_desktop::Chat::message_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Message)

#include "entities_db.ipp"

#endif  // MELON_CLIENT_DESKTOP_ENTITIES_DB_HPP_
