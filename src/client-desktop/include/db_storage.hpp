#ifndef MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_
#define MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_

#include <ram_storage.hpp>

#include <QtSql/QtSql>
#include <QtWidgets>

namespace melon::client_desktop
{

QString create_connection_with_db();

class Domain : public ram::Domain
{
public:

    // For Insert
    Domain(QString hostname, bool external);

    // For Select
    Domain(QString hostname);

    void remove_from_db();
};

class User : public ram::User
{
public:

    // For Insert
    User(QString username, id_t domain_id, Status status);

    // For Select
    User(QString username, id_t domain_id);

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
    Message(const QString& text)
    {
        this->set_text(text);
    }

    // for QVariant
    Message() = default;

    void remove_from_db();

    void set_text(const QString& text);

private:
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

    [[nodiscard]] const std::list<Message>& messages() const noexcept
    {
        return m_messages;
    }

    [[nodiscard]] std::list<Message>& messages() noexcept
    {
        return m_messages;
    }

    message_handle_t add_message(Message message)
    {
        m_messages.emplace_back(std::move(message));
        return std::prev(m_messages.end());
    }

    message_handle_t delete_message(message_handle_t message_handle)
    {
        message_handle->remove_from_db();
        return m_messages.erase(message_handle);
    }

    void set_incomplete_message(Message incomplete_message)
    {
        m_incomplete_message = std::move(incomplete_message);
    }

    [[nodiscard]] message_handle_t last_message()
    {
        if (m_messages.empty())
            return m_messages.begin();
        return std::prev(m_messages.end());
    }

    [[nodiscard]] bool empty() const
    {
        return m_messages.empty();
    }

    [[nodiscard]] const Message& incomplete_message() const noexcept
    {
        return m_incomplete_message;
    }

    void set_chatname(const QString& chatname);

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

    chat_handle_t add_chat(Chat chat)
    {
        m_chats.emplace_back(std::move(chat));
        return std::prev(m_chats.end());
    }

    chat_handle_t delete_chat(chat_handle_t chat_handle)
    {
        chat_handle->remove_from_db();
        return m_chats.erase(chat_handle);
    }

    [[nodiscard]] const std::list<Chat>& chats() const
    {
        return m_chats;
    }

    [[nodiscard]] std::list<Chat>& chats()
    {
        return m_chats;
    }

    [[nodiscard]] QString db_name()
    {
        return m_db_name;
    }

private:
    std::list<Chat> m_chats;
    QString m_db_name = QStringLiteral("test_db");

    StorageSingletone() = default;
    ~StorageSingletone() = default;
};

}  // namespace melon::client_desktop

Q_DECLARE_METATYPE(melon::client_desktop::StorageSingletone::chat_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Chat::message_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Message)
//Q_DECLARE_METATYPE(meloid_t)

#endif // MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_
