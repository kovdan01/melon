namespace melon::client_desktop
{

inline Message::Message(const QString& text)
{
    this->set_text(text);
}

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
    assert(!m_messages.empty());
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

}  // namespace melon::client_desktop
