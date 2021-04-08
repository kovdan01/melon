namespace melon::core
{

inline Domain::Domain(std::string hostname, bool external)
    : m_hostname(std::move(hostname))
    , m_external(external)
{
}

inline Domain::Domain(std::string hostname)  // -V730
    : m_hostname(std::move(hostname))
{
}

inline Domain::Domain(id_t id)
    : m_domain_id(id)
{
}

[[nodiscard]] inline id_t Domain::domain_id() const noexcept
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& Domain::hostname() const noexcept
{
    return m_hostname;
}

[[nodiscard]] inline bool Domain::external() const noexcept
{
    return m_external;
}

inline void Domain::set_domain_id(id_t domain_id) noexcept
{
    m_domain_id = domain_id;
}

inline void Domain::set_external(bool external) noexcept
{
    m_external = external;
}

inline void Domain::set_hostname(std::string hostname) noexcept
{
    m_hostname = std::move(hostname);
}


inline User::User(std::string username, id_t domain_id, Status status)
    : m_domain_id(domain_id)
    , m_username(std::move(username))
    , m_status(status)
{
}

inline User::User(std::string username, id_t domain_id)  // -V730
    : m_domain_id(domain_id)
    , m_username(std::move(username))
{
}

inline User::User(id_t user_id, id_t domain_id)
    : m_user_id(user_id)
    , m_domain_id(domain_id)
{
}

[[nodiscard]] inline User::Status User::status() const noexcept
{
    return m_status;
}

[[nodiscard]] inline id_t User::user_id() const noexcept
{
    return m_user_id;
}

[[nodiscard]] inline id_t User::domain_id() const noexcept
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& User::username() const noexcept
{
    return m_username;
}

inline void User::set_status(Status status) noexcept
{
    m_status = status;
}

inline void User::set_user_id(id_t user_id) noexcept
{
    m_user_id = user_id;
}

inline void User::set_domain_id(id_t domain_id) noexcept
{
    m_domain_id = domain_id;
}

inline void User::set_username(std::string username) noexcept
{
    m_username = std::move(username);
}

inline Chat::Chat(id_t domain_id, std::string chatname)
    : m_domain_id(domain_id)
    , m_chatname(std::move(chatname))
{
}

inline Chat::Chat(id_t chat_id, id_t domain_id)
    : m_chat_id(chat_id)
    , m_domain_id(domain_id)
{
}

[[nodiscard]] inline id_t Chat::chat_id() const noexcept
{
    return m_chat_id;
}

[[nodiscard]] inline id_t Chat::domain_id() const noexcept
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& Chat::chatname() const noexcept
{
    return m_chatname;
}

inline void Chat::set_chatname(std::string chatname)
{
    m_chatname = std::move(chatname);
}

inline void Chat::set_chat_id(id_t chat_id) noexcept
{
    m_chat_id = chat_id;
}

inline void Chat::set_domain_id(id_t domain_id) noexcept
{
    m_domain_id = domain_id;
}


inline Message::Message(id_t chat_id, id_t domain_id_chat, id_t user_id, id_t domain_id_user,
                        std::string text, Message::timestamp_t timestamp, Message::Status status)
    : m_chat_id(chat_id)
    , m_domain_id_chat(domain_id_chat)
    , m_user_id(user_id)
    , m_domain_id_user(domain_id_user)
    , m_text(std::move(text))
    , m_timestamp(timestamp)
    , m_status(status)
{
}

inline Message::Message(id_t message_id, id_t chat_id, id_t domain_id_chat)  // -V730
    : m_message_id(message_id)
    , m_chat_id(chat_id)
    , m_domain_id_chat(domain_id_chat)
{
}

[[nodiscard]] inline id_t Message::message_id() const noexcept
{
    return m_message_id;
}

[[nodiscard]] inline id_t Message::chat_id() const noexcept
{
    return m_chat_id;
}

[[nodiscard]] inline id_t Message::domain_id_chat() const noexcept
{
    return m_domain_id_chat;
}

[[nodiscard]] inline id_t Message::user_id() const noexcept
{
    return m_user_id;
}

[[nodiscard]] inline id_t Message::domain_id_user() const noexcept
{
    return m_domain_id_user;
}

[[nodiscard]] inline const std::string& Message::text() const noexcept
{
    return m_text;
}

[[nodiscard]] inline Message::Status Message::status() const noexcept
{
    return m_status;
}

[[nodiscard]] inline Message::timestamp_t Message::timestamp() const
{
    return m_timestamp;
}

inline void Message::set_text(std::string text)
{
    m_text = std::move(text);
}

inline void Message::set_status(Status status) noexcept
{
    m_status = status;
}

inline void Message::set_timestamp(Message::timestamp_t timestamp)
{
    m_timestamp = timestamp;
}

inline void Message::set_message_id(id_t message_id) noexcept
{
    m_message_id = message_id;
}

inline void Message::set_user_id(id_t user_id) noexcept
{
    m_user_id = user_id;
}

inline void Message::set_chat_id(id_t chat_id) noexcept
{
    m_chat_id = chat_id;
}

inline void Message::set_domain_id_chat(id_t domain_id_chat) noexcept
{
    m_domain_id_chat = domain_id_chat;
}

inline void Message::set_domain_id_user(id_t domain_id_user) noexcept
{
    m_domain_id_user = domain_id_user;
}


}  // namespace melon::core
