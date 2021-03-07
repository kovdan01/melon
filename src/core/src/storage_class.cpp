#include <melon/core/storage_class.hpp>

#include <iostream>

namespace melon::core
{

Domain::Domain(std::uint64_t domain_id, std::string hostname, bool external)
    : m_domain_id(domain_id)
    , m_hostname(std::move(hostname))
    , m_external(external)
{
}

User::User(std::uint64_t user_id, std::uint64_t domain_id, std::string username, Status status)
    : m_user_id(user_id)
    , m_domain_id(domain_id)
    , m_username(std::move(username))
    , m_status(status)
{
}

Message::Message(std::uint64_t message_id, std::uint64_t domain_id, std::uint64_t user_id,
                 std::uint64_t chat_id, std::string text, Message::Status status)
    : m_message_id(message_id)
    , m_domain_id(domain_id)
    , m_user_id(user_id)
    , m_chat_id(chat_id)
    , m_text(std::move(text))
    , m_status(status)
{
}

Chat::Chat(std::uint64_t chat_id, std::uint64_t domain_id, std::string chatname)
    : m_chat_id(chat_id)
    , m_domain_id(domain_id)
    , m_chatname(std::move(chatname))
{
}

}  // namespace melon::core
