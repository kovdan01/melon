namespace melon::core
{

[[nodiscard]] inline std::uint64_t Domain::domain_id() const
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& Domain::hostname() const
{
    return m_hostname;
}

[[nodiscard]] inline bool Domain::external() const
{
    return m_external;
}

inline void Domain::set_domain_id(std::uint64_t new_domain_id)
{
    m_domain_id = new_domain_id;
}

[[nodiscard]] inline User::Status User::status() const
{
    return m_status;
}

[[nodiscard]] inline std::uint64_t User::user_id() const
{
    return m_user_id;
}

[[nodiscard]] inline std::uint64_t User::domain_id() const
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& User::username() const
{
    return m_username;
}

inline void User::set_user_id(std::uint64_t new_user_id)
{
    m_user_id = new_user_id;
}

inline void User::set_domain_id(std::uint64_t new_domain_id)
{
    m_domain_id = new_domain_id;
}

[[nodiscard]] inline std::uint64_t Message::message_id() const
{
    return m_message_id;
}

[[nodiscard]] inline std::uint64_t Message::domain_id() const
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& Message::text() const
{
    return m_text;
}

[[nodiscard]] inline Message::Status Message::status() const
{
    return m_status;
}

[[nodiscard]] inline std::chrono::system_clock Message::timestamp() const
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

inline void Message::set_message_id(std::uint64_t new_message_id)
{
    m_message_id = new_message_id;
}

inline void Message::set_domain_id(std::uint64_t new_domain_id)
{
    m_domain_id = new_domain_id;
}

inline void Message::set_user_id(std::uint64_t new_user_id)
{
    m_user_id = new_user_id;
}

inline void Message::set_chat_id(std::uint64_t new_chat_id)
{
    m_chat_id = new_chat_id;
}

[[nodiscard]] inline std::uint64_t Chat::chat_id() const
{
    return m_chat_id;
}

[[nodiscard]] inline std::uint64_t Chat::domain_id() const
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& Chat::chatname() const
{
    return m_chatname;
}

inline void Chat::set_chat_id(std::uint64_t new_chat_id)
{
    m_chat_id = new_chat_id;
}

inline void Chat::set_domain_id(std::uint64_t new_domain_id)
{
    m_domain_id = new_domain_id;
}

}  // namespace melon::core
