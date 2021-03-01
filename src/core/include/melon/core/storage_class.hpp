#ifndef MELON_CORE_MELON_STORAGE_CLASS_HPP_
#define MELON_CORE_MELON_STORAGE_CLASS_HPP_

#include <melon/core/export.h>

#include <chrono>
#include <iostream>

namespace melon::core
{

class MELON_CORE_EXPORT User
{
public:
    enum class Status : std::uint8_t
    {
        OFFLINE,
        ONLINE,
        CHILL,
    };

    User(std::uint64_t user_id, std::string username, Status status);
    ~User() = default;

    [[nodiscard]] std::uint64_t user_id() const;
    [[nodiscard]] const std::string& username() const;
    [[nodiscard]] Status status() const;

private:
    std::uint64_t m_user_id;
    std::string m_username;
    Status m_status;
};

[[nodiscard]] inline User::Status User::status() const
{
    return m_status;
}

[[nodiscard]] inline std::uint64_t User::user_id() const
{
    return m_user_id;
}

[[nodiscard]] inline const std::string& User::username() const
{
    return m_username;
}

class MELON_CORE_EXPORT Message
{
public:
    enum class Status
    {
        FAIL,
        SENT,
        RECEIVED,
        SEEN,
    };

    Message(std::uint64_t message_id, std::uint64_t user_id, std::uint64_t chat_id, std::string text, Status status);
    ~Message() = default;

    [[nodiscard]] std::uint64_t message_id() const;
    [[nodiscard]] const std::string& text() const;
    [[nodiscard]] Status status() const;
    [[nodiscard]] std::chrono::high_resolution_clock::time_point timestamp() const;
    [[nodiscard]] std::uint64_t user_id() const;
    [[nodiscard]] std::uint64_t chat_id() const;

private:
    std::uint64_t m_message_id;
    std::uint64_t m_user_id;
    std::chrono::high_resolution_clock::time_point m_timestamp;
    std::uint64_t m_chat_id;
    std::string m_text;
    Status m_status;
};

[[nodiscard]] inline std::uint64_t Message::message_id() const
{
    return m_message_id;
}

[[nodiscard]] inline const std::string& Message::text() const
{
    return m_text;
}

[[nodiscard]] inline Message::Status Message::status() const
{
    return m_status;
}

[[nodiscard]] inline std::chrono::high_resolution_clock::time_point Message::timestamp() const
{
    return m_timestamp;
}

[[nodiscard]] inline std::uint64_t Message::user_id() const
{
    return m_user_id;
}

[[nodiscard]] inline std::uint64_t Message::chat_id() const
{
    return m_chat_id;
}

class MELON_CORE_EXPORT Chat
{
public:
    Chat(std::uint64_t chat_id, std::string chatname);
    ~Chat() = default;

    [[nodiscard]] std::uint64_t chat_id() const;
    [[nodiscard]] const std::string& chatname() const;

private:
    std::uint64_t m_chat_id;
    std::string m_chatname;
};

[[nodiscard]] inline std::uint64_t Chat::chat_id() const
{
    return m_chat_id;
}
[[nodiscard]] inline const std::string& Chat::chatname() const
{
    return m_chatname;
}


}  // namespace melon::core

#endif  // MELON_CORE_MELON_STORAGE_CLASS_HPP_
