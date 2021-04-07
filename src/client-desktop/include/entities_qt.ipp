namespace melon::client_desktop
{

using id_t = melon::core::id_t;

namespace detail
{

inline Domain::Domain (const QString& hostname, bool external)
    : melon::core::Domain(hostname.toStdString(), external)
{
}

inline Domain::Domain (const QString& hostname)
    : melon::core::Domain(hostname.toStdString())
{
}

[[nodiscard]] inline QString Domain::hostname() const
{
    return QString::fromStdString(melon::core::Domain::hostname());
}

inline User::User (const QString& username, id_t domain_id, User::Status status)
    : melon::core::User(username.toStdString(), domain_id, status)
{
}

inline User::User (const QString& username, id_t domain_id)
    : melon::core::User(username.toStdString(), domain_id)
{
}

[[nodiscard]] inline QString User::username() const
{
    return QString::fromStdString(melon::core::User::username());
}

inline Message::Message(id_t chat_id, id_t domain_id_chat, id_t user_id, id_t domain_id_user,
                        const QString& text, timestamp_t timestamp, Message::Status status)
    : melon::core::Message(chat_id, domain_id_chat, user_id, domain_id_user, text.toStdString(), timestamp, status)
{
}

inline Message::Message(id_t message_id, id_t chat_id, id_t domain_id_chat)
    : melon::core::Message(message_id, chat_id, domain_id_chat)
{
}

inline Message::Message() = default;

[[nodiscard]] inline QString Message::text() const
{
    return QString::fromStdString(melon::core::Message::text());
}

inline void Message::set_text(const QString& text)
{
    melon::core::Message::set_text(text.toStdString());
}

inline Chat::Chat(id_t domain_name, const QString& name)
    : melon::core::Chat(domain_name, name.toStdString())
{
}

inline Chat::Chat(id_t chat_id, id_t domain_id)
    : melon::core::Chat(chat_id, domain_id)
{
}

[[nodiscard]] inline QString Chat::chatname() const noexcept
{
    return QString::fromStdString(melon::core::Chat::chatname());
}

inline void Chat::set_chatname(const QString& chatname)
{
    melon::core::Chat::set_chatname(chatname.toStdString());
}

[[nodiscard]] inline int Chat::scrolling_position() const noexcept
{
    return m_scrolling_position;
}

inline void Chat::set_scrolling_position(int scrollbar) noexcept
{
    m_scrolling_position = scrollbar;
}

[[nodiscard]] inline bool Chat::is_read() const noexcept
{
    return true;
}

}  // namespace detail

}  // namespace melon::client_desktop
