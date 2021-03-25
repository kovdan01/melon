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

const std::uint64_t MY_USER_ID = 1;
const std::uint64_t ANOTHER_USER_ID = 2;
const std::uint64_t DOMAIN_ID = 1;

const QString DB_NAME = QStringLiteral("test_db");

class MessageRAM : public melon::core::Message
{
public:
    using timestamp_t = std::chrono::system_clock::time_point;

    // For Insert
    MessageRAM(std::uint64_t user_id, std::uint64_t chat_id, std::uint64_t domain_id,
               timestamp_t timestamp, QString text, Status status)
        : melon::core::Message(chat_id, domain_id, user_id, text.toStdString(), timestamp, status)
        , m_text(std::move(text))
    {
        set_from();
    }

    // For Select
    MessageRAM(std::uint64_t message_id, std::uint64_t chat_id, std::uint64_t domain_id)
        : melon::core::Message(message_id, chat_id, domain_id)
    {
    }

    // fr QVariant
    MessageRAM() = default;

    [[nodiscard]] const QString& from() const noexcept
    {
        return m_from;
    }

    [[nodiscard]] const QString& text_qstring() const noexcept
    {
        return m_text;
    }

    void set_text_qstring_RAM(QString text)
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

protected:
    void set_from()
    {
        if (this->user_id() == MY_USER_ID)
            m_from = QStringLiteral("Me");
        else
            m_from = QStringLiteral("Some Sender");
    }
private:
    void set_text(std::string);

    QString m_text;
    QString m_from;
    bool m_is_edit = false;
};

class ChatRAM : public melon::core::Chat
{
public:

    // For Insert & Select (temporary)
    ChatRAM(std::uint64_t domain_name, QString name)
        : melon::core::Chat(domain_name, name.toStdString())
        , m_name(std::move(name))
    {
    }

    [[nodiscard]] const QString& name_qstring() const noexcept
    {
        return m_name;
    }

    void set_name_qstring_RAM(QString chatname)
    {
        m_name = std::move(chatname);
        set_chatname_base(m_name.toStdString());
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

    QString m_name;
    int m_scrolling_position = 0;
};

}  // namespace melon::client_desktop



#endif  // MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_
