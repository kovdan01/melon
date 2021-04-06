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

    [[nodiscard]] static StorageSingletone& get_instance()
    {
        static StorageSingletone instance;
        return instance;
    }

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
    ~StorageSingletone() = default;
};

class DBSingletone
{
public:
    [[nodiscard]] inline static DBSingletone& get_instance()
    {
        static DBSingletone instance;
        return instance;
    }

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

    DBSingletone();
};

class DBNameSingletone
{
public:
    [[nodiscard]] inline static DBNameSingletone& get_instance()
    {
        static DBNameSingletone instance;
        return instance;
    }

    DBNameSingletone(const DBNameSingletone&) = delete;
    DBNameSingletone& operator=(const DBNameSingletone&) = delete;
    DBNameSingletone(DBNameSingletone&&) = delete;
    DBNameSingletone& operator=(DBNameSingletone&&) = delete;

    [[nodiscard]] QString db_name();

private:
    QString m_db_name = QStringLiteral("test_db");

    DBNameSingletone() = default;
    ~DBNameSingletone() = default;
};

}  // namespace melon::client_desktop

#include "storage_singletones.ipp"

Q_DECLARE_METATYPE(melon::client_desktop::StorageSingletone::chat_handle_t)

#endif // MELON_CLIENT_DESKTOP_STORAGE_SINGLETONES_HPP_
