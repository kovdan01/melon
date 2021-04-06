namespace melon::client_desktop
{

[[nodiscard]] inline const std::list<Chat>& StorageSingletone::chats() const
{
    return m_chats;
}

[[nodiscard]] inline std::list<Chat>& StorageSingletone::chats()
{
    return m_chats;
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

[[nodiscard]] inline User DBSingletone::me()
{
    return m_me;
}

[[nodiscard]] inline User DBSingletone::another_user()
{
    return m_another_user;
}

[[nodiscard]] inline Domain DBSingletone::my_domain()
{
    return m_my_domain;
}

[[nodiscard]] inline QString DBNameSingletone::db_name()
{
    return m_db_name;
}

}  // namespace melon::client_desktop
