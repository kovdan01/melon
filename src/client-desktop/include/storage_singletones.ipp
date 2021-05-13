namespace melon::client_desktop
{

[[nodiscard]] inline StorageSingletone& StorageSingletone::get_instance()
{
    static StorageSingletone instance;
    return instance;
}

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

[[nodiscard]] inline DBSingletone& DBSingletone::get_instance()
{
    static DBSingletone instance;
    return instance;
}

[[nodiscard]] inline const User& DBSingletone::me()
{
    return m_me;
}

[[nodiscard]] inline const User& DBSingletone::another_user()
{
    return m_another_user;
}

[[nodiscard]] inline const Domain& DBSingletone::my_domain()
{
    return m_my_domain;
}

[[nodiscard]] inline StorageNameSingletone& StorageNameSingletone::get_instance()
{
    static StorageNameSingletone instance;
    return instance;
}

[[nodiscard]] inline const QString& StorageNameSingletone::db_name()
{
    return m_db_name;
}

[[nodiscard]] inline const std::string& StorageNameSingletone::user_settings_file_name()
{
    return m_user_settings_file_name;
}

}  // namespace melon::client_desktop
