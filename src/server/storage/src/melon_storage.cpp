#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

#include <iostream>
#include <vector>

#include "storage.hpp"

namespace mysql = sqlpp::mysql;


namespace melon::server::storage
{

std::shared_ptr<mysql::connection_config> config_melondb()
{
    auto config = std::make_shared<mysql::connection_config>();
    config->user = "melon";
    config->database = "melon";
    config->host = "localhost";
    config->password = "melonpass";
    //config->debug = true;
    return config;
}


void add_user(mysql::connection& db, const mc::User& user)
{
    db(insert_into(G_USERS).set(G_USERS.username = user.username(), G_USERS.status = static_cast<int>(mc::User::Status::OFFLINE)));
}

/* Users */

std::vector<std::string> get_online_users_names(mysql::connection& db)
{
    std::vector<std::string> online_users_names;
    for (const auto& row : db(select(G_USERS.username).from(G_USERS).where(G_USERS.status == static_cast<std::uint8_t>(mc::User::Status::ONLINE))))
    {
        online_users_names.emplace_back(row.username);
    }
    return online_users_names;
}



std::vector<mc::User> get_online_users(mysql::connection& db)
{
    std::vector<mc::User> online_users;
    for (const auto& row : db(select(all_of(G_USERS)).from(G_USERS).where(G_USERS.status == static_cast<std::uint8_t>(mc::User::Status::ONLINE))))
    {
        std::uint8_t tmp = row.status;
        mc::User::Status status = static_cast<mc::User::Status>(tmp);
        mc::User user(row.userId, row.username, status);
        online_users.emplace_back(std::move(user));

    }
    return online_users;
}



void make_user_online(mysql::connection& db, const mc::User& user)
{
    db(update(G_USERS).set(G_USERS.status = static_cast<int>(mc::User::Status::ONLINE)).where(G_USERS.username == user.username()));
}

void make_user_offline(mysql::connection& db, const mc::User& user)
{
    db(update(G_USERS).set(G_USERS.status = static_cast<int>(mc::User::Status::OFFLINE)).where(G_USERS.username == user.username()));
}

/* Messages */

void add_message(mysql::connection& db, const mc::Message& message)
{
    db(insert_into(G_MESSAGES).set(G_MESSAGES.text = message.text(), G_MESSAGES.timesend = message.timestamp(), G_MESSAGES.status = static_cast<std::uint8_t>(mc::Message::Status::SENT),
            G_MESSAGES.seen = 0, G_MESSAGES.userId = message.user_id(), G_MESSAGES.chatId = message.chat_id()));
}

/* Chat */

void add_chat(mysql::connection& db, const mc::Chat& chat)
{
    db(insert_into(G_CHATS).set(G_CHATS.chatname = chat.chatname()));
}

std::vector<mc::Message> get_messages_for_chat(mysql::connection& db, const mc::Chat& chat)
{
    std::vector<mc::Message> messages_in_chat;
    for (const auto& row : db(select(all_of(G_MESSAGES)).from(G_MESSAGES).where(G_MESSAGES.chatId == chat.chatid())))
    {
        std::uint8_t tmp = row.status;
        mc::Message::Status status = static_cast<mc::Message::Status>(tmp);
        mc::Message message(row.messageId, row.text, status, row.seen, row.userId, row.chatId);
        messages_in_chat.emplace_back(std::move(message));
    }
    return messages_in_chat;
}

}  // namespace melon::server::storage
