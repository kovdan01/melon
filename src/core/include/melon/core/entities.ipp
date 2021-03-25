namespace melon::core
{

[[nodiscard]] inline id_t Domain::domain_id() const
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


[[nodiscard]] inline User::Status User::status() const
{
    return m_status;
}

[[nodiscard]] inline id_t User::user_id() const
{
    return m_user_id;
}

[[nodiscard]] inline id_t User::domain_id() const
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& User::username() const
{
    return m_username;
}


[[nodiscard]] inline id_t Chat::chat_id() const
{
    return m_chat_id;
}

[[nodiscard]] inline id_t Chat::domain_id() const
{
    return m_domain_id;
}

[[nodiscard]] inline const std::string& Chat::chatname() const
{
    return m_chatname;
}


[[nodiscard]] inline id_t Message::message_id() const
{
    return m_message_id;
}

[[nodiscard]] inline id_t Message::domain_id() const
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

[[nodiscard]] inline std::chrono::system_clock::time_point Message::timestamp() const
{
    return m_timestamp;
}

[[nodiscard]] inline id_t Message::user_id() const
{
    return m_user_id;
}

[[nodiscard]] inline id_t Message::chat_id() const
{
    return m_chat_id;
}

}  // namespace melon::core