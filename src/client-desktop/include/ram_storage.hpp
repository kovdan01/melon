#ifndef MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_
#define MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_

#include <QListWidget>
#include <QString>

#include <chrono>
#include <list>
#include <utility>
#include <vector>
#include <unordered_set>
#include <iostream>

namespace melon::client_desktop
{

using byte_t = std::uint8_t;

class Message
{
public:
    class Attachment
    {
    public:
        Attachment() = default;
        Attachment(const Attachment&) = default;
        Attachment& operator=(const Attachment&) = default;
        Attachment(Attachment&&) = default;
        Attachment& operator=(Attachment&&) = default;

        explicit Attachment(std::vector<byte_t> buffer)
            : m_buffer(std::move(buffer))
        {
        }

        bool operator==(const Attachment&) const noexcept = default;

        [[nodiscard]] const std::vector<byte_t>& buffer() const noexcept
        {
            return m_buffer;
        }

    private:
        std::vector<byte_t> m_buffer;
    };

    using timestamp_t = std::chrono::high_resolution_clock::time_point;

    Message() = default;
    Message(const Message&) = default;
    Message& operator=(const Message&) = default;
    Message(Message&&) = default;
    Message& operator=(Message&&) = default;

    Message(QString from, QString text, std::list<Attachment> attachments, timestamp_t timestamp)
        : m_attachments(std::move(attachments))
        , m_timestamp(timestamp)
        , m_text(std::move(text))
        , m_from(std::move(from))
    {
    }

    bool operator==(const Message&) const noexcept = default;

    [[nodiscard]] const std::list<Attachment>& attachments() const noexcept
    {
        return m_attachments;
    }

    [[nodiscard]] timestamp_t timestamp() const noexcept
    {
        return m_timestamp;
    }

    [[nodiscard]] const QString& text() const noexcept
    {
        return m_text;
    }

    [[nodiscard]] const QString& from() const noexcept
    {
        return m_from;
    }

    void set_text(const QString& text)
    {
        m_text = text;
    }

private:
    std::list<Attachment> m_attachments;
    timestamp_t m_timestamp;
    QString m_text;
    QString m_from;
};

class Chat
{
public:
    using id_t = std::size_t;
    using message_handle_t = std::list<Message>::iterator;

    Chat(QString name, id_t id)
        : m_incomplete_message(/* from */ QLatin1String(""),
                               /* text */ QStringLiteral(""),
                               /* attachments */ {},
                               /* timestamp */ {})
        , m_name(std::move(name))
        , m_id(id)
    {
    }

    Chat() = delete;
    Chat(const Chat&) = default;
    Chat& operator=(const Chat&) = default;
    Chat(Chat&&) = default;
    Chat& operator=(Chat&&) = default;

    [[nodiscard]] const std::list<Message>& messages() const noexcept
    {
        return m_messages;
    }

    [[nodiscard]] std::list<Message>& messages() noexcept
    {
        return m_messages;
    }

    [[nodiscard]] const Message& incomplete_message() const noexcept
    {
        return m_incomplete_message;
    }

    [[nodiscard]] int scrolling_position() const noexcept
    {
        return m_scrolling_position;
    }

    [[nodiscard]] const QString& name() const noexcept
    {
        return m_name;
    }

    void set_name(QString name)
    {
        m_name = std::move(name);
    }

    [[nodiscard]] id_t id() const noexcept
    {
        return m_id;
    }

    message_handle_t add_message(Message message)
    {
        m_messages.emplace_back(std::move(message));
        return std::prev(m_messages.end());
    }

    message_handle_t delete_message(message_handle_t message_handle)
    {
        return m_messages.erase(message_handle);
    }

    void set_incomplete_message(Message incomplete_message)
    {
        m_incomplete_message = std::move(incomplete_message);
    }

    void set_scrolling_position(int scrollbar) noexcept
    {
        m_scrolling_position = scrollbar;
    }

private:
    std::list<Message> m_messages = {};
    Message m_incomplete_message;
    QString m_name;
    id_t m_id;
    int m_scrolling_position = 0;
};


class RAMStorageSingletone
{
public:
    using chat_handle_t = std::list<Chat>::iterator;
    using const_chat_handle_t = std::list<Chat>::const_iterator;

    [[nodiscard]] static RAMStorageSingletone& get_instance()
    {
        static RAMStorageSingletone instance;
        return instance;
    }

    RAMStorageSingletone(const RAMStorageSingletone& root) = delete;
    RAMStorageSingletone& operator=(const RAMStorageSingletone&) = delete;
    RAMStorageSingletone(RAMStorageSingletone&& root) = delete;
    RAMStorageSingletone& operator=(RAMStorageSingletone&&) = delete;

    chat_handle_t add_chat(Chat chat)
    {
        m_chats.emplace_back(std::move(chat));
        return std::prev(m_chats.end());
    }

    chat_handle_t delete_chat(chat_handle_t chat_handle)
    {
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

    RAMStorageSingletone() = default;
    ~RAMStorageSingletone() = default;
};

}  // namespace melon::client_desktop

Q_DECLARE_METATYPE(melon::client_desktop::RAMStorageSingletone::chat_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Chat::message_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Message)


#endif  // MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_
