#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

#include <iostream>
#include <vector>

#include "storage.hpp"
#include "melondb.h"


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


void add_user(mysql::connection& db, const User& user)
{
    db(insert_into(g_users).set(g_users.username = user.username, g_users.status = 0));
}

/* Users */

std::vector<std::string> get_online_users_names(mysql::connection& db)
{
    std::vector<std::string> online_users_names;
    for (const auto& row : db(select(g_users.username).from(g_users).where(g_users.status == 1)))
    {
        online_users_names.emplace_back(row.username);
    }
    return online_users_names;
}

std::vector<User> get_online_users(mysql::connection& db)
{
    std::vector<User> online_users;
    for (const auto& row : db(select(all_of(g_users)).from(g_users).where(g_users.status == 1)))
    {
        User user;
        user.userid = row.userId;
        user.username = row.username;
        online_users.emplace_back(std::move(user));
    }
    return online_users;
}



void make_user_online(mysql::connection& db, User& user)
{
    db(update(g_users).set(g_users.status = 1).where(g_users.username == user.username));
}

void make_user_offline(mysql::connection& db, User& user)
{
    db(update(g_users).set(g_users.status = 0).where(g_users.username == user.username));
}

/* Messages */

void add_message(mysql::connection& db, Message& message)
{
    db(insert_into(g_messages).set(g_messages.text = message.text, g_messages.timesend = message.timestamp, g_messages.status = 0,
            g_messages.seen = 0, g_messages.userId = message.user_id, g_messages.chatId = message.chat_id));
}

/* Chat */

void add_chat(mysql::connection& db, Chat chat)
{
    db(insert_into(g_chats).set(g_chats.chatname = chat.chatname));
}

std::vector<Message> get_messages_for_chat(mysql::connection& db, Chat& chat)
{
    std::vector<Message> messages_in_chat;
    for (const auto& row : db(select(all_of(g_messages)).from(g_messages).where(g_messages.chatId == chat.chatid)))
    {
        Message message;
        message.text = row.text;
        message.seen = row.seen;
        message.status = row.status;
        messages_in_chat.emplace_back(std::move(message));
    }
    return messages_in_chat;
}

}  // namespace melon::server::storage
