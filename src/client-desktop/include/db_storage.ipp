namespace melon::client_desktop
{

inline Message::Message(const QString& text)
{
    this->set_text(text);
}

inline Message::Message() = default;

[[nodiscard]] inline const QString& Message::from() const noexcept
{
    return m_from;
}

[[nodiscard]] inline bool Message::is_edit() const noexcept
{
    return m_is_edit;
}

[[nodiscard]] inline const std::list<Message>& Chat::messages() const noexcept
{
    return m_messages;
}

[[nodiscard]] inline std::list<Message>& Chat::messages() noexcept
{
    return m_messages;
}

inline Chat::message_handle_t Chat::add_message(Message message)
{
    m_messages.emplace_back(std::move(message));
    return std::prev(m_messages.end());
}

inline Chat::message_handle_t Chat::delete_message(message_handle_t message_handle)
{
    message_handle->remove_from_db();
    return m_messages.erase(message_handle);
}

inline void Chat::set_incomplete_message(Message incomplete_message)
{
    m_incomplete_message = std::move(incomplete_message);
}

[[nodiscard]] inline Chat::message_handle_t Chat::last_message()
{
    if (m_messages.empty())
        return m_messages.begin();
    return std::prev(m_messages.end());
}

[[nodiscard]] inline bool Chat::empty() const
{
    return m_messages.empty();
}

[[nodiscard]] inline const Message& Chat::incomplete_message() const noexcept
{
    return m_incomplete_message;
}

inline StorageSingletone::chat_handle_t StorageSingletone::add_chat(Chat chat)
{
    m_chats.emplace_back(std::move(chat));
    return std::prev(m_chats.end());
}

inline StorageSingletone::chat_handle_t StorageSingletone::delete_chat(StorageSingletone::chat_handle_t chat_handle)
{
    chat_handle->remove_from_db();
    return m_chats.erase(chat_handle);
}

[[nodiscard]] inline const std::list<Chat>& StorageSingletone::chats() const
{
    return m_chats;
}

[[nodiscard]] inline std::list<Chat>& StorageSingletone::chats()
{
    return m_chats;
}

[[nodiscard]] inline User UserDomainSingletone::me()
{
    return m_me;
}

[[nodiscard]] inline User UserDomainSingletone::another_user()
{
    return m_another_user;
}

[[nodiscard]] inline Domain UserDomainSingletone::my_domain()
{
    return m_my_domain;
}

[[nodiscard]] inline QString DBSingletone::db_name()
{
    return m_db_name;
}

}  // namespace melon::client_desktop
