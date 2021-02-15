#include <melon/storage_class.hpp>

#include <iostream>

namespace melon::core
{


User::User(std::uint64_t user_id, std::string username, Status status)
    : m_user_id(user_id)
    , m_username(std::move(username))
    , m_status(status)
{
}

User::User(std::string username)
    : m_username(std::move(username))
{
}

[[nodiscard]] User::Status User::status() const
{
    return m_status;
}

[[nodiscard]] std::uint64_t User::user_id() const
{
    return m_user_id;
}

[[nodiscard]] const std::string& User::username() const
{
    return m_username;
}

Message::Message(std::uint64_t message_id, std::string text, Message::Status status, bool seen,
                 std::uint64_t user_id, std::uint32_t chat_id)
    : m_message_id(message_id)
    , m_text(std::move(text))
    , m_user_id(user_id)
    , m_chat_id(chat_id)
    , m_status(status)
    , m_seen(seen)
{
}

Message::Message(std::string text, Message::Status status, bool seen,
                 std::uint64_t user_id, std::uint32_t chat_id)
    : m_text(std::move(text))
    , m_user_id(user_id)
    , m_chat_id(chat_id)
    , m_status(status)
    , m_seen(seen)
{
}

[[nodiscard]] std::uint64_t Message::message_id() const
{
    return m_message_id;
}

[[nodiscard]] const std::string& Message::text() const
{
    return m_text;
}

[[nodiscard]] Message::Status Message::status() const
{
    return m_status;
}

[[nodiscard]] bool Message::seen() const
{
    return m_seen;
}

[[nodiscard]] const std::chrono::high_resolution_clock::time_point Message::timestamp() const
{
    return m_timestamp;
}

[[nodiscard]] std::uint64_t Message::user_id() const
{
    return m_user_id;
}

[[nodiscard]] std::uint32_t Message::chat_id() const
{
    return m_chat_id;
}

Chat::Chat(std::uint32_t chat_id, std::string chatname)
    : m_chat_id(chat_id)
    , m_chatname(std::move(chatname))
{
}

Chat::Chat(std::string chatname)
    :m_chatname(std::move(chatname))
{
}

[[nodiscard]] std::uint32_t Chat::chat_id() const
{
    return m_chat_id;
}
[[nodiscard]] const std::string Chat::chatname() const
{
    return m_chatname;
}

}  // namespace melon::core
