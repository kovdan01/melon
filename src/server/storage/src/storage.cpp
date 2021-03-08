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
const melon::Domains G_DOMAINS;
const melon::Chats G_CHATS;


/* class Domain */

// Add only unique hostnames
Domain::Domain(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string hostname, bool external)
     : melon::core::Domain(domain_id, hostname, external), m_db(db)
{
    // [check] In mariadb it is bool, but it is not bool in sqlpp11 why? should change then to std::uint8_t but that strange
    // author says that bool IS supported]
    m_db(insert_into(G_DOMAINS).set(G_DOMAINS.hostname = this->hostname(), G_DOMAINS.external = static_cast<std::uint8_t>(this->external())));
    std::uint64_t t_domain_id = get_domain_id_by_hostname(db, hostname);
    melon::core::Domain::set_domain_id(t_domain_id);
}

// [check] That I do not delete useful information from messages - now it deletes to much maybe
void Domain::remove_domain()
{
    m_db(remove_from(G_DOMAINS).where(G_DOMAINS.domainId == this->domain_id()));
}


/* class Message */

void Message::update_text(const std::string& new_text)
{
    melon::core::Message::update_text(new_text);
    m_db(update(G_MESSAGES).set(G_MESSAGES.text = new_text).where(G_MESSAGES.messageId == this->message_id()));
}

void Message::remove_message()
{
    m_db(remove_from(G_MESSAGES).where(G_MESSAGES.messageId == this->message_id()));
}

void Message::change_status(Status new_status)
{
    m_db(update(G_MESSAGES).set(G_MESSAGES.status = static_cast<int>(new_status)).where(G_MESSAGES.messageId == this->message_id()));
}




/* class Chat */

void Chat::update_chatname(const std::string& new_chatname)
{
    melon::core::Chat::update_chatname(new_chatname);
    m_db(update(G_CHATS).set(G_CHATS.chatname = new_chatname).where(G_CHATS.chatId == this->chat_id()));
}

//Deletes chat -> deletes messages from chat, all info in Chats_Users concearnig this chat
void Chat::remove_chat()
{
    m_db(remove_from(G_CHATS).where(G_CHATS.chatId == this->chat_id()));
}




void add_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat, const std::string& searched_hostname)
{
    std::uint64_t domain_id = find_or_insert_domain_id(db, searched_hostname);
    db(insert_into(G_CHATS).set(G_CHATS.domainId = domain_id, G_CHATS.chatname = chat.chatname()));
}






/* class User */

void User::remove_user()
{
    m_db(remove_from(G_USERS).where(G_USERS.userId == this->user_id()));
}

void User::change_status(Status new_status)
{
    m_db(update(G_USERS).set(G_USERS.status = static_cast<int>(new_status)).where(G_USERS.userId == this->user_id()));
}

void User::add_user(const std::string& hostname)
{
    std::uint64_t domain_id = find_or_insert_domain_id(m_db, hostname);
    melon::core::User::set_user_domain_id(domain_id);
    m_db(insert_into(G_USERS).set(G_USERS.username = this->username(), G_USERS.domainId = domain_id,
                                G_USERS.status = static_cast<int>(this->status())));
}

//All recieved (this is like not read - maybe should change name for it) messages for specific user
std::uint64_t User::count_number_of_recieved_messages()
{
    for (const auto& row : m_db(select(count(G_MESSAGES.messageId)).from(G_MESSAGES)
                              .where(G_MESSAGES.status == static_cast<std::uint8_t>(melon::core::Message::Status::RECEIVED) and G_MESSAGES.userId == this->user_id())))
    {
        return row.count;
    }
}


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

//Pass hostname to function as std::string, default value for external 'false'. Now this type is used in get_domain_id_by_hostname - should be changed
void add_domain(sqlpp::mysql::connection& db, const std::string& hostname)
{
    db(insert_into(G_DOMAINS).set(G_DOMAINS.hostname = hostname, G_DOMAINS.external = static_cast<std::uint8_t>(true)));
}

std::uint64_t get_domain_id_by_hostname(sqlpp::mysql::connection& db, const std::string& searched_hostname)
{
    auto result = db(select(G_DOMAINS.domainId).from(G_DOMAINS).where(G_DOMAINS.hostname == searched_hostname));
    if (!result.empty())
    {
       const auto& row = result.front();
       return row.domainId;
    }
    return std::uint64_t(-1);
}

std::uint64_t find_or_insert_domain_id(sqlpp::mysql::connection& db, const std::string& hostname)
{
    std::uint64_t domain_id = get_domain_id_by_hostname(db, hostname);
    if (domain_id != std::uint64_t(-1))
    {
        return domain_id;
    }
    add_domain(db, hostname);
    domain_id = get_domain_id_by_hostname(db, hostname);
    return domain_id;
}




/* Users */

//List of all users in database
std::vector<std::string> get_names_of_all_users(sqlpp::mysql::connection& db)
{
    std::vector<std::string> all_users_on_server;
    for (const auto& row : db(select(G_USERS.username).from(G_USERS).unconditionally()))
    {
        all_users_on_server.emplace_back(row.username);
    }
    return all_users_on_server;
}

//List of usernames of online users in database
std::vector<std::string> get_online_users_names(sqlpp::mysql::connection& db)
{
    std::vector<std::string> online_users_names;
    for (const auto& row : db(select(G_USERS.username).from(G_USERS).where(G_USERS.status == static_cast<std::uint8_t>(melon::core::User::Status::ONLINE))))
    {
        online_users_names.emplace_back(row.username);
    }
    return online_users_names;
}

//List of online mc::Users in database
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


/* Messages */

void add_message(sqlpp::mysql::connection& db, const melon::core::Message& message)
{
    db(insert_into(G_MESSAGES).set(G_MESSAGES.text = message.text(),
                                   G_MESSAGES.timesend = message.timestamp(),
                                   G_MESSAGES.status = static_cast<std::uint8_t>(message.status()),
                                   G_MESSAGES.domainId = message.domain_id(),
                                   G_MESSAGES.userId = message.user_id(),
                                   G_MESSAGES.chatId = message.chat_id()));
}

void add_message(sqlpp::mysql::connection& db, const Message& message)
{
    db(insert_into(G_MESSAGES).set(G_MESSAGES.text = message.text(),
                                   G_MESSAGES.timesend = message.timestamp(),
                                   G_MESSAGES.status = static_cast<std::uint8_t>(message.status()),
                                   G_MESSAGES.domainId = message.domain_id(),
                                   G_MESSAGES.userId = message.user_id(),
                                   G_MESSAGES.chatId = message.chat_id()));
}


/* Chats */

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

}  // namespace melon::server::storage
