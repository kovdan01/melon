#ifndef MELON_CLIENT_DESKTOP_STORAGE_SINGLETONES_HPP_
#define MELON_CLIENT_DESKTOP_STORAGE_SINGLETONES_HPP_

#include <entities_db.hpp>

namespace melon::client_desktop
{

class StorageSingletone
{
public:
    using chat_handle_t = std::list<Chat>::iterator;
    using const_chat_handle_t = std::list<Chat>::const_iterator;

    [[nodiscard]] static StorageSingletone& get_instance();

    StorageSingletone(const StorageSingletone&) = delete;
    StorageSingletone& operator=(const StorageSingletone&) = delete;
    StorageSingletone(StorageSingletone&&) = delete;
    StorageSingletone& operator=(StorageSingletone&&) = delete;

    [[nodiscard]] const std::list<Chat>& chats() const;

    [[nodiscard]] std::list<Chat>& chats();

    chat_handle_t add_chat(Chat chat);
    chat_handle_t delete_chat(chat_handle_t chat_handle);

private:
    std::list<Chat> m_chats;

    StorageSingletone() = default;
};

class DBSingletone
{
public:
    [[nodiscard]] static DBSingletone& get_instance();

    DBSingletone(const DBSingletone&) = delete;
    DBSingletone& operator=(const DBSingletone&) = delete;
    DBSingletone(DBSingletone&&) = delete;
    DBSingletone& operator=(DBSingletone&&) = delete;

    [[nodiscard]] const User& me();
    [[nodiscard]] const User& another_user();
    [[nodiscard]] const Domain& my_domain();

private:
    User m_me;
    User m_another_user;
    Domain m_my_domain;

    DBSingletone();  // NOLINT (modernize-use-equals-delete)
};

class StorageNameSingletone
{
public:
    [[nodiscard]] static StorageNameSingletone& get_instance();

    StorageNameSingletone(const StorageNameSingletone&) = delete;
    StorageNameSingletone& operator=(const StorageNameSingletone&) = delete;
    StorageNameSingletone(StorageNameSingletone&&) = delete;
    StorageNameSingletone& operator=(StorageNameSingletone&&) = delete;

    [[nodiscard]] QString db_name();
    [[nodiscard]] std::string user_settings_file_name();

private:
    QString m_db_name = QStringLiteral("test_db");
    std::string m_user_settings_file_name = "user_config.yaml";

    StorageNameSingletone() = default;
};

}  // namespace melon::client_desktop

Q_DECLARE_METATYPE(melon::client_desktop::StorageSingletone::chat_handle_t)

#include "storage_singletones.ipp"

#endif  // MELON_CLIENT_DESKTOP_STORAGE_SINGLETONES_HPP_
