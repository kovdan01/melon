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

const std::uint64_t INVALID_ID = std::uint64_t(-1);


/* Init and connect to db */

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


/* class Domain */

// Add only unique hostnames
Domain::Domain(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string hostname, bool external)
     : melon::core::Domain(domain_id, hostname, external), m_db(db)
{
    // [check] In mariadb it is bool, but it is not bool in sqlpp11 why? should change then to std::uint8_t but that strange author says that bool IS supported]
    m_db(insert_into(G_DOMAINS).set(G_DOMAINS.hostname = this->hostname(), G_DOMAINS.external = static_cast<std::uint8_t>(this->external())));
    std::uint64_t this_domain_id = get_domain_id_by_hostname(db, hostname);
    this->set_domain_id(this_domain_id);
}

// [check] That I do not delete useful information from messages - now it deletes to much maybe
void Domain::remove_domain()
{
    m_db(remove_from(G_DOMAINS).where(G_DOMAINS.domainId == this->domain_id()));
}


/* class Message */

Message::Message(sqlpp::mysql::connection& db, std::uint64_t message_id, std::uint64_t domain_id, std::uint64_t user_id,
        std::uint64_t chat_id, std::string text, Status status)
     : melon::core::Message(message_id, domain_id, user_id, chat_id, text, status), m_db(db)
{
    m_db(insert_into(G_MESSAGES).set(G_MESSAGES.text = this->text(),
                                   G_MESSAGES.timesend = this->timestamp(),
                                   G_MESSAGES.status = static_cast<std::uint8_t>(this->status()),
                                   G_MESSAGES.domainId = this->domain_id(),
                                   G_MESSAGES.userId = this->user_id(),
                                   G_MESSAGES.chatId = this->chat_id()));
    std::uint64_t this_message_id = get_message_id_by_chat_id_and_domain_id_and_user_id(m_db,
                                                                                        this->chat_id(),
                                                                                        this->domain_id(),
                                                                                        this->user_id());
    this->set_message_id(this_message_id);
}

Message::Message(sqlpp::mysql::connection& db, std::uint64_t message_id, std::uint64_t domain_id,
                 std::uint64_t user_id, std::uint64_t chat_id, std::string text, Status status,
                 const std::string& hostname, const std::string& username, const std::string& chatname)
     : melon::core::Message(message_id, domain_id, user_id, chat_id, text, status), m_db(db)
{
    std::uint64_t message_domain_id = get_domain_id_by_hostname(m_db, hostname);
    this->set_domain_id(message_domain_id);
    std::uint64_t message_user_id = get_user_id_by_username_and_domain_id(m_db, username, message_domain_id);
    this->set_user_id(message_user_id);
    std::uint64_t message_chat_id = get_chat_id_by_chatname_and_domain_id(m_db, chatname, message_domain_id);
    this->set_chat_id(message_chat_id);
    m_db(insert_into(G_MESSAGES).set(G_MESSAGES.text = this->text(),
                                   G_MESSAGES.timesend = this->timestamp(),
                                   G_MESSAGES.status = static_cast<std::uint8_t>(this->status()),
                                   G_MESSAGES.domainId = this->domain_id(),
                                   G_MESSAGES.userId = this->user_id(),
                                   G_MESSAGES.chatId = this->chat_id()));
    std::uint64_t this_message_id = get_message_id_by_chat_id_and_domain_id_and_user_id(m_db,
                                                                                        this->chat_id(),
                                                                                        this->domain_id(),
                                                                                        this->user_id());
    this->set_message_id(this_message_id);
}

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
    melon::core::Message::change_status(new_status);
    m_db(update(G_MESSAGES).set(G_MESSAGES.status = static_cast<int>(new_status)).where(G_MESSAGES.messageId == this->message_id()));
}


/* class Chat */

Chat::Chat(sqlpp::mysql::connection& db, std::uint64_t chat_id, std::uint64_t domain_id, std::string chatname)
    : melon::core::Chat(chat_id, domain_id, chatname), m_db(db)
{
    m_db(insert_into(G_CHATS).set(G_CHATS.domainId = domain_id, G_CHATS.chatname = this->chatname()));
    std::uint64_t this_chat_id = get_chat_id_by_chatname_and_domain_id(m_db, this->chatname(), this->domain_id());
    this->set_chat_id(this_chat_id);
}

Chat::Chat(sqlpp::mysql::connection& db, std::uint64_t chat_id, std::uint64_t domain_id, std::string chatname, const std::string& hostname)
    : melon::core::Chat(chat_id, domain_id, chatname), m_db(db)
{
    std::uint64_t chat_domain_id = get_domain_id_by_hostname(m_db, hostname);
    this->set_domain_id(chat_domain_id);
    m_db(insert_into(G_CHATS).set(G_CHATS.domainId = domain_id, G_CHATS.chatname = this->chatname()));
    std::uint64_t this_chat_id = get_chat_id_by_chatname_and_domain_id(m_db, this->chatname(), this->domain_id());
    this->set_chat_id(this_chat_id);
}

//void Chat::update_chatname(const std::string& new_chatname)
//{
//    melon::core::Chat::update_chatname(new_chatname);
//    m_db(update(G_CHATS).set(G_CHATS.chatname = new_chatname).where(G_CHATS.chatId == this->chat_id()));
//}

//Deletes chat: deletes messages from chat, all info in Chats_Users concearnig this chat
void Chat::remove_chat()
{
    m_db(remove_from(G_CHATS).where(G_CHATS.chatId == this->chat_id()));
}


/* class User */

User::User(sqlpp::mysql::connection& db, std::uint64_t user_id, std::uint64_t domain_id, std::string username, Status status)
     : melon::core::User(user_id, domain_id, username, status), m_db(db)
{
    m_db(insert_into(G_USERS).set(G_USERS.username = this->username(), G_USERS.domainId = domain_id, G_USERS.status = static_cast<int>(this->status())));
    std::uint64_t this_user_id = get_user_id_by_username_and_domain_id(m_db, this->username(), this->domain_id());
    this->set_user_id(this_user_id);
}

User::User(sqlpp::mysql::connection& db, std::uint64_t user_id, std::uint64_t domain_id, std::string username, Status status, const std::string& hostname)
    : melon::core::User(user_id, domain_id, username, status), m_db(db)
{
    std::uint64_t user_domain_id = get_domain_id_by_hostname(m_db, hostname);
    this->set_domain_id(user_domain_id);
    m_db(insert_into(G_USERS).set(G_USERS.username = this->username(), G_USERS.domainId = user_domain_id,
                                G_USERS.status = static_cast<int>(this->status())));
    std::uint64_t this_user_id = get_user_id_by_username_and_domain_id(m_db, this->username(), this->domain_id());
    this->set_user_id(this_user_id);
}

void User::remove_user()
{
    m_db(remove_from(G_USERS).where(G_USERS.userId == this->user_id()));
}

void User::change_status(Status new_status)
{
    melon::core::User::change_status(new_status);
    m_db(update(G_USERS).set(G_USERS.status = static_cast<int>(new_status)).where(G_USERS.userId == this->user_id()));
}

//Number of recieved (this is like "not read" - maybe should change name for it) messages for specific user
std::uint64_t User::count_number_of_recieved_messages()
{
    for (const auto& row : m_db(select(count(G_MESSAGES.messageId)).from(G_MESSAGES)
                              .where(G_MESSAGES.status == static_cast<std::uint8_t>(melon::core::Message::Status::RECEIVED) and G_MESSAGES.userId == this->user_id())))
    {
        return row.count;
    }
    return INVALID_ID;
}


/* Domains */

std::uint64_t get_domain_id_by_hostname(sqlpp::mysql::connection& db, const std::string& hostname)
{
    auto result = db(select(G_DOMAINS.domainId).from(G_DOMAINS).where(G_DOMAINS.hostname == hostname));
    if (!result.empty())
    {
       const auto& row = result.front();
       return row.domainId;
    }
    return INVALID_ID;
}


/* Users */

std::uint64_t get_user_id_by_username_and_domain_id(sqlpp::mysql::connection& db, const std::string& username, std::uint64_t domain_id)
{
    auto result = db(select(G_USERS.userId).from(G_USERS).where(G_USERS.username == username and G_USERS.domainId == domain_id));
    if (!result.empty())
    {
       const auto& row = result.front();
       return row.userId;
    }
    return INVALID_ID;
}

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

std::uint64_t get_message_id_by_chat_id_and_domain_id_and_user_id(sqlpp::mysql::connection& db, std::uint64_t chat_id,
                                                                   std::uint64_t domain_id, std::uint64_t user_id)
{
    auto result = db(select(G_MESSAGES.messageId).from(G_MESSAGES).where(G_MESSAGES.chatId == chat_id and G_MESSAGES.domainId == domain_id and
                                                                         G_MESSAGES.userId == user_id));
    if (!result.empty())
    {
       const auto& row = result.front();
       return row.messageId;
    }
    return INVALID_ID;
}

/* Chats */

// So, chatname is unique then, because I use it for search
std::uint64_t get_chat_id_by_chatname_and_domain_id(sqlpp::mysql::connection& db, const std::string& chatname, std::uint64_t domain_id)
{
    auto result = db(select(G_CHATS.chatId).from(G_CHATS).where(G_CHATS.chatname == chatname and G_CHATS.domainId == domain_id));
    if (!result.empty())
    {
       const auto& row = result.front();
       return row.chatId;
    }
    return INVALID_ID;
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

}  // namespace melon::server::storage
