#ifndef MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_
#define MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_

#include <melon/core/entities.hpp>

#include <QString>
#include <QMetaType>

#include <chrono>
#include <list>
#include <utility>
#include <vector>

namespace melon::client_desktop
{

inline constexpr melon::core::id_t MY_USER_ID = 1;
inline constexpr melon::core::id_t ANOTHER_USER_ID = 2;
inline constexpr melon::core::id_t DOMAIN_ID = 1;

const QString DB_NAME = QStringLiteral("test_db");

class MessageRAM : public melon::core::Message
{
public:
    using timestamp_t = std::chrono::system_clock::time_point;

    // For Insert
    MessageRAM(std::uint64_t chat_id, std::uint64_t domain_id_chat,
               std::uint64_t user_id, std::uint64_t domain_id_user,
                QString text, timestamp_t timestamp, Status status)
        : melon::core::Message(chat_id, domain_id_chat, user_id, domain_id_user, text.toStdString(), timestamp, status)
        , m_text(std::move(text))
    {
        this->set_from();
    }

    // For Select
    MessageRAM(std::uint64_t message_id, std::uint64_t chat_id, std::uint64_t domain_id_chat)
        : melon::core::Message(message_id, chat_id, domain_id_chat)
    {
    }

    // for QVariant
    MessageRAM() = default;

    [[nodiscard]] const QString& from() const noexcept
    {
        return m_from;
    }

    [[nodiscard]] const QString& text() const noexcept
    {
        return m_text;
    }

    void set_text(const QString& text)
    {
        melon::core::Message::set_text(text.toStdString());
    }

    [[nodiscard]] bool is_edit() const noexcept
    {
        return m_is_edit;
    }

    void set_is_edit(bool is_edit)
    {
        m_is_edit = is_edit;
    }

protected:
    void set_from()
    {
        if (this->user_id() == MY_USER_ID)
            m_from = QStringLiteral("Me");
        else
            m_from = QStringLiteral("Some Sender");
    }

    void set_text_base(std::string text) override
    {
        melon::core::Message::set_text_base(std::move(text));
        m_text = QString::fromStdString(melon::core::Message::text());
    }

private:
    QString m_text;
    QString m_from;
    bool m_is_edit = false;
};

class ChatRAM : public melon::core::Chat
{
public:
    // For Insert
    ChatRAM(std::uint64_t domain_name, QString name)
        : melon::core::Chat(domain_name, name.toStdString())
        , m_chatname(std::move(name))
    {
    }

    // For Select
    ChatRAM(std::uint64_t chat_id, std::uint64_t domain_id)
        : melon::core::Chat(chat_id, domain_id)
    {
    }

    [[nodiscard]] const QString& chatname() const noexcept
    {
        return m_chatname;
    }

    void set_chatname(const QString& chatname)
    {
        melon::core::Chat::set_chatname(chatname.toStdString());
    }

    [[nodiscard]] int scrolling_position() const noexcept
    {
        return m_scrolling_position;
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

    std::vector<melon::core::User::Ptr> get_users() const override
    {
        return {};  // just a placeholder
    }

    std::vector<melon::core::Message::Ptr> get_messages() const override
    {
        return {};  // just a placeholder
    }

protected:
    void set_chatname_base(std::string chatname) override
    {
        melon::core::Chat::set_chatname_base(std::move(chatname));
        m_chatname = QString::fromStdString(melon::core::Chat::chatname());
    }

private:

    QString m_chatname;
    int m_scrolling_position = 0;
};

}  // namespace melon::client_desktop



#endif  // MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_
