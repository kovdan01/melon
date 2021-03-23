#include <melon/core/storage_class.hpp>

#include <iostream>

namespace melon::core
{

Domain::Domain(std::string hostname, bool external)
    : m_hostname(std::move(hostname))
    , m_external(external)
{
}

Domain::Domain(std::string hostname)
    : m_hostname(std::move(hostname))
{
}

Domain::~Domain() = default;

void Domain::set_domain_id(std::uint64_t domain_id)
{
    m_domain_id = domain_id;
}

void Domain::set_external(bool external)
{
    this->set_external_base(external);
    this->set_external_impl(external);
}

void Domain::set_external_base(bool external)
{
    m_external = external;
}

void Domain::set_external_impl(bool /* external */)
{
}


User::User(std::string username, std::uint64_t domain_id, Status status)
    : m_domain_id(domain_id)
    , m_username(std::move(username))
    , m_status(status)
{
}

User::User(std::string username, std::uint64_t domain_id)
    : m_domain_id(domain_id)
    , m_username(std::move(username))
{
}

User::~User() = default;

void User::set_status(Status status)
{
    this->set_status_base(status);
    this->set_status_impl(status);
}

void User::set_status_base(Status status)
{
    m_status = status;
}

void User::set_status_impl(Status /* status */)
{
}

void User::set_user_id(std::uint64_t user_id)
{
    m_user_id = user_id;
}

void User::set_domain_id(std::uint64_t domain_id)
{
    m_domain_id = domain_id;
}

Chat::Chat(std::uint64_t domain_id, std::string chatname)
    : m_domain_id(domain_id)
    , m_chatname(std::move(chatname))
{
}

Chat::Chat(std::uint64_t chat_id, std::uint64_t domain_id)
    : m_chat_id(chat_id)
    , m_domain_id(domain_id)
{
}

Chat::~Chat() = default;

void Chat::set_chatname(std::string chatname)
{
    this->set_chatname_base(std::move(chatname));
    this->set_chatname_impl(this->chatname());
}

void Chat::set_chatname_base(std::string chatname)
{
    m_chatname = std::move(chatname);
}

void Chat::set_chatname_impl(const std::string& /* chatname */)
{
}

void Chat::set_chat_id(std::uint64_t chat_id)
{
    m_chat_id = chat_id;
}

void Chat::set_domain_id(std::uint64_t domain_id)
{
    m_domain_id = domain_id;
}

Message::Message(std::uint64_t user_id, std::uint64_t chat_id, std::uint64_t domain_id,
                 std::chrono::system_clock::time_point timestamp, std::string text, Message::Status status)
    : m_user_id(user_id)
    , m_chat_id(chat_id)
    , m_domain_id(domain_id)
    , m_timestamp(timestamp)
    , m_text(std::move(text))
    , m_status(status)
{
}

Message::Message(std::uint64_t message_id, std::uint64_t chat_id, std::uint64_t domain_id)
    : m_message_id(message_id)
    , m_chat_id(chat_id)
    , m_domain_id(domain_id)
{
}

Message::~Message() = default;

void Message::set_text(std::string text)
{
    this->set_text_base(std::move(text));
    this->set_text_impl(this->text());
}

void Message::set_text_base(std::string text)
{
    m_text = std::move(text);
}

void Message::set_text_impl(const std::string& /* text */)
{
}

void Message::set_status(Status status)
{
    this->set_status_base(status);
    this->set_status_impl(status);
}

void Message::set_status_base(Status status)
{
    m_status = status;
}

void Message::set_status_impl(Status /* status */)
{
}

void Message::set_timestamp(Message::timestamp_t timestamp)
{
    this->set_timestamp_base(timestamp);
    this->set_timestamp_impl(timestamp);
}

void Message::set_timestamp_base(Message::timestamp_t timestamp)
{
    m_timestamp = timestamp;
}

void Message::set_timestamp_impl(Message::timestamp_t /* timestamp */)
{
}

void Message::set_message_id(std::uint64_t message_id)
{
    m_message_id = message_id;
}

void Message::set_user_id(std::uint64_t user_id)
{
    m_user_id = user_id;
}

void Message::set_chat_id(std::uint64_t chat_id)
{
    m_chat_id = chat_id;
}

void Message::set_domain_id(std::uint64_t domain_id)
{
    m_domain_id = domain_id;
}

}  // namespace melon::core
