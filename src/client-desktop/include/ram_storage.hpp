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

using id_t = melon::core::id_t;

namespace ram
{

class Domain : public melon::core::Domain
{
public:
    // For Insert
    Domain (const QString& hostname, bool external);
    // For Select
    Domain (const QString& hostname);

    [[nodiscard]] QString hostname() const noexcept;
};

class User : public melon::core::User
{
public:
    // For Insert
    User (const QString& username, id_t domain_id, Status status);
    // For Select
    User (const QString& username, id_t domain_id);

    [[nodiscard]] QString username() const noexcept;
};

class Message : public melon::core::Message
{
public:
    using timestamp_t = std::chrono::system_clock::time_point;

    // For Insert
    Message(id_t chat_id, id_t domain_id_chat,
            id_t user_id, id_t domain_id_user,
            const QString& text, timestamp_t timestamp, Status status);
    // For Select
    Message(id_t message_id, id_t chat_id, id_t domain_id_chat);
    // for QVariant
    Message();

    [[nodiscard]] QString text() const noexcept;
    void set_text(const QString& text);
};

class Chat : public melon::core::Chat
{
public:
    // For Insert
    Chat(id_t domain_name, const QString& name);
    // For Select
    Chat(id_t chat_id, id_t domain_id);

    [[nodiscard]] QString chatname() const noexcept;
    [[nodiscard]] int scrolling_position() const noexcept;

    void set_chatname(const QString& chatname);
    void set_scrolling_position(int scrollbar) noexcept;

    // this function is just draft
    // it can be used only to test "unread" mark painting
    // set to false to force "unread" mark near each chat
    [[nodiscard]] bool is_read() const noexcept;

private:
    int m_scrolling_position = 0;
};

}  // namespace ram

}  // namespace melon::client_desktop


#include "ram_storage.ipp"
#endif  // MELON_CLIENT_DESKTOP_RAM_STORAGE_HPP_
