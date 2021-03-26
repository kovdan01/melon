#ifndef MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_
#define MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_

#include <ram_storage.hpp>

#include <QtSql/QtSql>
#include <QtWidgets>

namespace melon::client_desktop
{

QString create_connection_with_db();

class Message : public MessageRAM
{
public:
    // For Insert
    Message(std::uint64_t chat_id, std::uint64_t domain_id, std::uint64_t user_id,
            timestamp_t timestamp, QString text, Status status);

    // For Select
    Message(std::uint64_t message_id, std::uint64_t chat_id, std::uint64_t domain_id);

    // For incomplete message
    Message(QString text)
    {
        this->set_text_qstring_RAM(std::move(text));
    }

    // for QVariant
    Message() = default;

    void set_text_qstring(QString text);
    void remove_from_db();
};


class Chat : public ChatRAM
{
public:
    using message_handle_t = std::list<Message>::iterator;

    // For insert
    Chat(std::uint64_t domain_id, QString name);

    // For Select
    Chat(std::uint64_t chat_id, std::uint64_t domain_id);

    void remove_from_db();
    void set_name_qstring(QString text);

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

private:
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

    StorageSingletone(const StorageSingletone& root) = delete;
    StorageSingletone& operator=(const StorageSingletone&) = delete;
    StorageSingletone(StorageSingletone&& root) = delete;
    StorageSingletone& operator=(StorageSingletone&&) = delete;

    chat_handle_t add_chat(const Chat& chat)
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
private:
    std::list<Chat> m_chats;

    StorageSingletone() = default;
    ~StorageSingletone() = default;
};

}  // namespace melon::client_desktop

Q_DECLARE_METATYPE(melon::client_desktop::StorageSingletone::chat_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Chat::message_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Message)
Q_DECLARE_METATYPE(std::uint64_t)

#endif // MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_
