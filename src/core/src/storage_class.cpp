#include <melon/storage_class.hpp>

#include <iostream>

namespace melon::core
{


User::User(std::uint64_t userid, std::string username, std::uint8_t status)
    :m_userid(std::move(userid)), m_username(std::move(username)), m_status(std::move(status))
{
    std::cout << m_userid << ":" << m_username << ":" << m_status <<std::endl;
}

User::User(std::string username)
    :m_username(std::move(username))
{
}

[[nodiscard]] std::uint8_t User::status()
{
    return m_status;
}

[[nodiscard]] std::uint64_t User::userid()
{
    return m_userid;
}

[[nodiscard]] const std::string User::username() const
{
    return m_username;
}

Message::Message(std::uint64_t messageid, std::string text, std::uint8_t status, bool seen,
                 std::uint64_t user_id, std::uint32_t chat_id)
    :m_messageid(std::move(messageid)), m_text(std::move(text)), m_status(std::move(status)), m_seen(std::move(seen)),
      m_user_id(std::move(user_id)), m_chat_id(std::move(chat_id))
{
}

Message::Message(std::string text, std::uint8_t status, bool seen,
                 std::uint64_t user_id, std::uint32_t chat_id)
    : m_text(std::move(text)), m_status(std::move(status)), m_seen(std::move(seen)),
      m_user_id(std::move(user_id)), m_chat_id(std::move(chat_id))
{
}

[[nodiscard]] std::uint64_t Message::messageid()
{
    return m_messageid;
}

[[nodiscard]] const std::string Message::text() const
{
    return m_text;
}

[[nodiscard]] std::uint8_t Message::status()
{
    return m_status;
}

[[nodiscard]] bool Message::seen()
{
    return m_seen;
}

[[nodiscard]] const std::chrono::high_resolution_clock::time_point Message::timestamp() const
{
    return m_timestamp;
}

[[nodiscard]] const std::uint64_t Message::user_id() const
{
    return m_user_id;
}

[[nodiscard]] const std::uint32_t Message::chat_id() const
{
    return m_chat_id;
}

Chat::Chat(std::uint32_t chatid, std::string chatname)
    :m_chatid((std::move(chatid))), m_chatname((std::move(chatname)))
{
}

Chat::Chat(std::string chatname)
    :m_chatname(std::move(chatname))
{
}

[[nodiscard]] const std::uint32_t Chat::chatid() const
{
    return m_chatid;
}
[[nodiscard]] const std::string Chat::chatname() const
{
    return m_chatname;
}

}  // namespace melon::core
