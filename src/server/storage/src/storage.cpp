#include <melon/db_scheme.h>
#include <storage.hpp>

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/update.h>

#include <iostream>
#include <vector>

namespace melon::server::storage
{

const melon::Users G_USERS;
const melon::Messages G_MESSAGES;
const melon::Chats G_CHATS;

std::shared_ptr<sqlpp::mysql::connection_config> config_melondb()
{
    auto config = std::make_shared<sqlpp::mysql::connection_config>();
    config->user = "melon";
    config->database = "melon";
    config->host = "localhost";
    config->password = "melonpass";
    config->debug = true;
    return config;
}

/* Users */

void add_user(sqlpp::mysql::connection& db, const melon::core::User& user)
{
    db(insert_into(G_USERS).set(G_USERS.username = user.username(), G_USERS.status = static_cast<std::uint8_t>(melon::core::User::Status::OFFLINE)));
}

std::vector<std::string> get_online_users_names(sqlpp::mysql::connection& db)
{
    std::vector<std::string> online_users_names;
    for (const auto& row : db(select(G_USERS.username).from(G_USERS).where(G_USERS.status == static_cast<std::uint8_t>(melon::core::User::Status::ONLINE))))
    {
        online_users_names.emplace_back(row.username);
    }
    return online_users_names;
}

std::vector<melon::core::User> get_online_users(sqlpp::mysql::connection& db)
{
    std::vector<melon::core::User> online_users;
    for (const auto& row : db(select(all_of(G_USERS)).from(G_USERS).where(G_USERS.status == static_cast<std::uint8_t>(melon::core::User::Status::ONLINE))))
    {
        unsigned long tmp = row.status;  // NOLINT (google-runtime-int)
        auto status = static_cast<melon::core::User::Status>(tmp);
        melon::core::User user(row.userId, row.username, status);
        online_users.emplace_back(std::move(user));
    }
    return online_users;
}

void make_user_online(sqlpp::mysql::connection& db, const melon::core::User& user)
{
    db(update(G_USERS).set(G_USERS.status = static_cast<int>(melon::core::User::Status::ONLINE)).where(G_USERS.username == user.username()));
}

void make_user_offline(sqlpp::mysql::connection& db, const melon::core::User& user)
{
    db(update(G_USERS).set(G_USERS.status = static_cast<int>(melon::core::User::Status::OFFLINE)).where(G_USERS.username == user.username()));
}

/* Messages */

void add_message(sqlpp::mysql::connection& db, const melon::core::Message& message)
{
    db(insert_into(G_MESSAGES).set(G_MESSAGES.text = message.text(),
                                   G_MESSAGES.timesend = message.timestamp(),
                                   G_MESSAGES.status = static_cast<std::uint8_t>(melon::core::Message::Status::SENT),
                                   G_MESSAGES.userId = message.user_id(),
                                   G_MESSAGES.chatId = message.chat_id()));
}

/* Chats */

void add_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat)
{
    db(insert_into(G_CHATS).set(G_CHATS.chatname = chat.chatname()));
}

std::vector<melon::core::Message> get_messages_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat)
{
    std::vector<melon::core::Message> messages_in_chat;
    for (const auto& row : db(select(all_of(G_MESSAGES)).from(G_MESSAGES).where(G_MESSAGES.chatId == chat.chat_id())))
    {
        unsigned long tmp = row.status;  // NOLINT (google-runtime-int)
        auto status = static_cast<melon::core::Message::Status>(tmp);
        melon::core::Message message(row.messageId, row.userId, row.chatId, row.text, status);
        messages_in_chat.emplace_back(std::move(message));
    }
    return messages_in_chat;
}

}  // namespace melon::server::storage
