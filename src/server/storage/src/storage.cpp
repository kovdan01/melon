#include <melon/db_scheme.h>
#include <storage.hpp>

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/update.h>

#include <iostream>
#include <vector>

namespace melon::server::storage
{

const melon::Users G_USERS;
const melon::Messages G_MESSAGES;
const melon::Domains G_DOMAINS;
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


/* Domains */

// hostname to function as std::string
void add_domain(sqlpp::mysql::connection& db, const std::string& searched_hostname)
{
    db(insert_into(G_DOMAINS).set(G_DOMAINS.hostname = searched_hostname));
}

std::uint64_t get_domain(sqlpp::mysql::connection& db, const std::string& searched_hostname)
{
    auto result = db(select(G_DOMAINS.domainId).from(G_DOMAINS).where(G_DOMAINS.hostname == searched_hostname));
    if(!result.empty())
    {
       const auto& row = result.front();
       return  row.domainId;
    }
    return 0;
}

std::uint64_t find_or_insert_domain_id(sqlpp::mysql::connection& db, const std::string& searched_hostname)
{
    std::uint64_t domainid = get_domain(db, searched_hostname);
    if (domainid != 0)
    {
        return domainid;
    }
    add_domain(db, searched_hostname);
    domainid = get_domain(db, searched_hostname);
    return domainid;
}




// add only unique hostnames
void add_domain(sqlpp::mysql::connection& db, const melon::core::Domain& domain)
{
    db(insert_into(G_DOMAINS).set(G_DOMAINS.hostname = domain.hostname()));
}

//now you cannot, should think what i should delete or not from db
void remove_domain(sqlpp::mysql::connection& db, const melon::core::Domain& domain)
{
    db(remove_from(G_DOMAINS).where(G_DOMAINS.hostname == domain.hostname()));
}




/* Users */


void count_users(sqlpp::mysql::connection& db)
{
    for (const auto& row : db(select(count(G_USERS.userId)).from(G_USERS).unconditionally()))
    {
       std::cout << "Number of users : " <<  row.count << std::endl;
    }
}

// List of all users in database
std::vector<std::string> get_names_of_all_users(sqlpp::mysql::connection& db)
{
    std::vector<std::string> all_users_on_server;
    for (const auto& row : db(select(G_USERS.username).from(G_USERS).unconditionally()))
    {
        all_users_on_server.emplace_back(row.username);
    }
    return all_users_on_server;
}

void add_user(sqlpp::mysql::connection& db, const melon::core::User& user, const std::string& searched_hostname)
{
    std::uint64_t domain_id = find_or_insert_domain_id(db, searched_hostname);
    db(insert_into(G_USERS).set(G_USERS.username = user.username(), G_USERS.domainId = domain_id,  G_USERS.status = static_cast<std::uint8_t>(melon::core::User::Status::OFFLINE)));
}

void remove_user(sqlpp::mysql::connection& db, const melon::core::User& user)
{
    db(remove_from(G_USERS).where(G_USERS.userId == user.user_id()));
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
        auto status = static_cast<melon::core::User::Status>(static_cast<int>(row.status));
        melon::core::User user(row.userId, row.domainId, row.username, status);
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


// all recieved messages for all users (should be changes for specific user)
void count_number_recieved_messages (sqlpp::mysql::connection& db)
{
    for (const auto& row : db(select(count(G_MESSAGES.messageId)).from(G_MESSAGES)
                              .where(G_MESSAGES.status == static_cast<std::uint8_t>(melon::core::Message::Status::RECEIVED))))
    {
       std::cout << "Number of messagers : " <<  row.count << std::endl;
    }

}

void add_message(sqlpp::mysql::connection& db, const melon::core::Message& message)
{
    db(insert_into(G_MESSAGES).set(G_MESSAGES.text = message.text(),
                                   G_MESSAGES.timesend = message.timestamp(),
                                   G_MESSAGES.status = static_cast<std::uint8_t>(message.status()),
                                   G_MESSAGES.domainId = message.domain_id(),
                                   G_MESSAGES.userId = message.user_id(),
                                   G_MESSAGES.chatId = message.chat_id()));
}

void update_text(sqlpp::mysql::connection& db, const std::string& new_text, const melon::core::Message& message)
{
    db(update(G_MESSAGES).set(G_MESSAGES.text = new_text).where(G_MESSAGES.messageId == message.message_id()));
}

void update_status_for_seen(sqlpp::mysql::connection& db, const melon::core::Message& message)
{
    db(update(G_MESSAGES).set(G_MESSAGES.status = static_cast<int>(melon::core::Message::Status::SEEN)).where(G_MESSAGES.messageId == message.message_id()));
}

void remove_message(sqlpp::mysql::connection& db, const melon::core::Message& message)
{
    db(remove_from(G_MESSAGES).where(G_MESSAGES.messageId == message.message_id()));
}

/* Chats */

void add_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat, std::string searched_hostname)
{
    std::uint64_t domain_id = find_or_insert_domain_id(db, searched_hostname);
    db(insert_into(G_CHATS).set(G_CHATS.domainId = domain_id, G_CHATS.chatname = chat.chatname()));
}

std::vector<melon::core::Message> get_messages_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat)
{
    std::vector<melon::core::Message> messages_in_chat;
    for (const auto& row : db(select(all_of(G_MESSAGES)).from(G_MESSAGES).where(G_MESSAGES.chatId == chat.chat_id())))
    {
        auto status = static_cast<melon::core::Message::Status>(static_cast<int>(row.status));
        melon::core::Message message(row.messageId, row.domainId, row.userId, row.chatId, row.text, status);
        messages_in_chat.emplace_back(std::move(message));
    }
    return messages_in_chat;
}

//deletes chat -> deletes messages from chat, all info in Chats_Users concearnig this chat
void remove_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat)
{
    db(remove_from(G_CHATS).where(G_CHATS.chatId == chat.chat_id()));
}

void update_chatname(sqlpp::mysql::connection& db, const std::string& new_chatname, const melon::core::Chat& chat)
{
    db(update(G_CHATS).set(G_CHATS.chatname = new_chatname).where(G_CHATS.chatId == chat.chat_id()));
}

}  // namespace melon::server::storage
