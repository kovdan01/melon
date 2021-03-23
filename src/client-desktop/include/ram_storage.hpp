#ifndef MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_
#define MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_

#include <melon/core/storage_class.hpp>

#include <QString>
#include <QMetaType>

#include <chrono>
#include <list>
#include <utility>
#include <vector>

namespace melon::client_desktop
{

using byte_t = std::uint8_t;

const std::uint64_t MY_USER_ID = 1;
const std::uint64_t ANOTHER_USER_ID = 2;
const std::uint64_t DRAFT_CHAT_ID = 1;
const std::uint64_t DRAFT_DOMAIN_ID = 1;

class Message : public melon::core::Message
{
public:
    using timestamp_t = std::chrono::system_clock::time_point;
    //using Status = melon::core::Message::Status;

    //For Insert & Select (temporary)
    Message(std::uint64_t user_id, std::uint64_t chat_id, std::uint64_t domain_id,
            timestamp_t timestamp, QString text, Status status)
        : melon::core::Message(user_id, chat_id, domain_id, timestamp, text.toStdString(), status)
        , m_text(std::move(text))
    {
        if (this->user_id() == MY_USER_ID)
            m_from = QStringLiteral("Me");
        else
            m_from = QStringLiteral("Some Sender");
    }

    Message() = default;

    [[nodiscard]] const QString& from() const noexcept
    {
        return m_from;
    }

    [[nodiscard]] const QString& text_qstring() const noexcept
    {
        return m_text;
    }

    void set_text_qstring(QString text)
    {
        m_text = std::move(text);
        set_text_base(m_text.toStdString());
    }

    [[nodiscard]] bool is_edit() const noexcept
    {
        return m_is_edit;
    }

    void set_is_edit(bool is_edit)
    {
        m_is_edit = is_edit;
    }

private:
    void set_text(std::string);

    QString m_text;
    QString m_from;
    bool m_is_edit = false;
};

class Chat : public melon::core::Chat
{
public:
    using message_handle_t = std::list<Message>::iterator;

    // For Insert & Select (temporary)
    Chat(std::uint64_t domain_name, QString name)
        : melon::core::Chat(domain_name, name.toStdString())
        , m_incomplete_message(/* from */ MY_USER_ID,
                               /* chat_id*/ DRAFT_CHAT_ID,
                               /* domain_id*/ DRAFT_DOMAIN_ID,
                               /* timestamp */ {},
                               /* text */ QStringLiteral(""),
                               /* status*/ melon::core::Message::Status::FAIL)
    {
    }

    [[nodiscard]] const std::list<Message>& messages() const noexcept
    {
        return m_messages;
    }

    [[nodiscard]] std::list<Message>& messages()noexcept
    {
        return m_messages;
    }

    [[nodiscard]] const QString& name_qstring() const noexcept
    {
        return m_name;
    }

    void set_name_qstring(QString chatname)
    {
        m_name = std::move(chatname);
        set_chatname_base(m_name.toStdString());
    }

    [[nodiscard]] const Message& incomplete_message() const noexcept
    {
        return m_incomplete_message;
    }

    [[nodiscard]] int scrolling_position() const noexcept
    {
        return m_scrolling_position;
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

    [[nodiscard]] bool is_read() const noexcept
    {
        // this function is just draft
        // it can be used only to test "unread" mark painting
        // set to false to force "unread" mark near each chat
        return true;
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

    std::vector<melon::core::User::Ptr> get_users() const override
    {
        std::vector<melon::core::User::Ptr> answer;
        return answer;
    }

    std::vector<melon::core::Message::Ptr> get_messages() const override
    {
        std::vector<melon::core::Message::Ptr> answer;
        return answer;
    }

private:
    void set_chatname(std::string);

    std::list<Message> m_messages = {};
    Message m_incomplete_message;
    QString m_name;
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
