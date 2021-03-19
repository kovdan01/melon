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
const melon::ChatsUsers G_CHATSUSERS;

const std::uint64_t INVALID_ID = std::uint64_t(-1);
const std::uint64_t INVALID = 0;

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

// For Insert
Domain::Domain(sqlpp::mysql::connection& db, std::string hostname, bool external)
     : melon::core::Domain(INVALID, std::move(hostname), external)
     , m_db(db)
{
    // [check] In mariadb it is bool, but it is not bool in sqlpp11 why? should change then to std::uint8_t but that strange author says that bool IS supported]
    m_db(insert_into(G_DOMAINS).set(G_DOMAINS.hostname = this->hostname(), G_DOMAINS.external = static_cast<std::uint8_t>(this->external())));
    std::uint64_t this_domain_id = max_domain_id(m_db);
    this->set_domain_id(this_domain_id);
}

// For Select
Domain::Domain(sqlpp::mysql::connection& db, std::string hostname)
     : melon::core::Domain(INVALID, std::move(hostname), false)
     , m_db(db)
{
    auto result = db(select(G_DOMAINS.domainId, G_DOMAINS.external).from(G_DOMAINS).where(G_DOMAINS.hostname == this->hostname()));
    if (!result.empty())
    {
        const auto& row = result.front();
        this->set_domain_id(row.domainId);
        //auto external = static_cast<bool>(row.external);
        this->set_ext(true);
        //this->set_external(row.external);
    }
    else
    {
        throw std::runtime_error("No hostname in database");
    }
}

std::uint64_t max_domain_id(sqlpp::mysql::connection& db)
{
    for (const auto& row : db(select(max(G_DOMAINS.domainId)).from(G_DOMAINS).unconditionally()))
    {
        return row.max;
    }
    return INVALID_ID;
}

// [check] That I do not delete useful information from table messages - now it deletes to much maybe
void Domain::remove()
{
    m_db(remove_from(G_DOMAINS).where(G_DOMAINS.domainId == this->domain_id()));
}


/* class Message */

// For Insert
Message::Message(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::uint64_t user_id, std::uint64_t chat_id,
                 std::string text, Status status)
     : melon::core::Message(INVALID, domain_id, user_id, chat_id, std::move(text), status)
     , m_db(db)
{
    m_db(insert_into(G_MESSAGES).set(G_MESSAGES.text = text,
                                     G_MESSAGES.timesend = std::chrono::system_clock::now(),
                                     G_MESSAGES.status = static_cast<std::uint8_t>(status),
                                     G_MESSAGES.domainId = domain_id,
                                     G_MESSAGES.userId = user_id,
                                     G_MESSAGES.chatId = chat_id));
    std::uint64_t this_message_id = max_message_id(m_db);
    this->set_message_id(this_message_id);
}

// For Select
Message::Message(sqlpp::mysql::connection& db, std::uint64_t message_id, std::uint64_t domain_id, std::uint64_t chat_id)
     : melon::core::Message(message_id, domain_id, INVALID, chat_id, std::move(""), Message::Status::FAIL)
     , m_db(db)
{
    auto result = db(select(G_MESSAGES.userId, G_MESSAGES.text, G_MESSAGES.status, G_MESSAGES.timesend)
                     .from(G_MESSAGES).where(G_MESSAGES.chatId == chat_id and G_MESSAGES.domainId == domain_id
                                             and G_MESSAGES.messageId == message_id));
    if (!result.empty())
    {
        const auto& row = result.front();
        auto status = static_cast<melon::core::Message::Status>(static_cast<int>(row.status));
        melon::core::Message::set_status(status);
        melon::core::Message::set_text(row.text);
        this->set_user_id(row.userId);
        //this->set_timestamp(row.timesend);
    }
    else
    {
        throw std::runtime_error("No message in database");
    }
}

std::uint64_t max_message_id(sqlpp::mysql::connection& db)
{
    for (const auto& row : db(select(max(G_MESSAGES.messageId)).from(G_MESSAGES).unconditionally()))
    {
        return row.max;
    }
    return INVALID_ID;
}

void Message::set_text(const std::string new_text)
{
    melon::core::Message::set_text(new_text);
    m_db(update(G_MESSAGES).set(G_MESSAGES.text = new_text).where(G_MESSAGES.messageId == this->message_id()));
}

void Message::remove()
{
    m_db(remove_from(G_MESSAGES).where(G_MESSAGES.messageId == this->message_id()));
}

void Message::set_status(Status new_status)
{
    melon::core::Message::set_status(new_status);
    m_db(update(G_MESSAGES).set(G_MESSAGES.status = static_cast<int>(new_status)).where(G_MESSAGES.messageId == this->message_id()));
}


/* class Chat */

// For Insert
Chat::Chat(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string chatname)
    : melon::core::Chat(INVALID, domain_id, std::move(chatname))
    , m_db(db)
{
    //m_db(insert_into(G_CHATS).set(G_CHATS.domainId = this->domain_id(), G_CHATS.chatname = this->chatname()));
    m_db(insert_into(G_CHATS).set(G_CHATS.domainId = domain_id, G_CHATS.chatname = chatname));
    std::uint64_t this_chat_id = max_chat_id(m_db);
    this->set_chat_id(this_chat_id);
}

// For Select
Chat::Chat(sqlpp::mysql::connection& db, std::uint64_t chat_id, std::uint64_t domain_id)
    : melon::core::Chat(chat_id, domain_id, std::move(""))
    , m_db(db)
{
    auto result = db(select(G_CHATS.chatname).from(G_CHATS).where(G_CHATS.chatId == chat_id and
                                                                  G_CHATS.domainId == domain_id));
    if (!result.empty())
    {
        const auto& row = result.front();
        melon::core::Chat::set_chatname(row.chatname);
    }
    else
    {
        throw std::runtime_error("No chat in database");
    }
}

std::uint64_t max_chat_id(sqlpp::mysql::connection& db)
{
    for (const auto& row : db(select(max(G_CHATS.chatId)).from(G_CHATS).unconditionally()))
    {
        return row.max;
    }
    return INVALID_ID;
}

void Chat::set_chatname(const std::string new_chatname)
{
    melon::core::Chat::set_chatname(new_chatname);
    m_db(update(G_CHATS).set(G_CHATS.chatname = new_chatname).where(G_CHATS.chatId == this->chat_id()));
}

// Deletes chat: deletes messages from chat, all info in Chats_Users concearnig this chat
void Chat::remove()
{
    m_db(remove_from(G_CHATS).where(G_CHATS.chatId == this->chat_id()));
}


/* class User */

// For Insert
User::User(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string username, Status status)
     : melon::core::User(INVALID, domain_id, std::move(username), status)
     , m_db(db)
{
    m_db(insert_into(G_USERS).set(G_USERS.username = this->username(), G_USERS.domainId = this->domain_id(), G_USERS.status = static_cast<int>(this->status())));
    std::uint64_t this_user_id = max_user_id(m_db);
    this->set_user_id(this_user_id);
}

// For Select
User::User(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string username)
    : melon::core::User(-1, domain_id, std::move(username), Status::OFFLINE)
    , m_db(db)
{
    auto result = db(select(G_USERS.userId, G_USERS.status).from(G_USERS).where(G_USERS.username == this->username() and G_USERS.domainId == domain_id));
    if (!result.empty())
    {
        const auto& row = result.front();
        this->set_user_id(row.userId);
        auto status = static_cast<melon::core::User::Status>(static_cast<int>(row.status));
        melon::core::User::set_status(status);
    }
    else
    {
        throw std::runtime_error("No user in database");
    }
}

std::uint64_t max_user_id(sqlpp::mysql::connection& db)
{
    auto result = db(select(max(G_USERS.userId)).from(G_USERS).unconditionally());
    if (!result.empty())
    {
        const auto& row = result.front();
        return row.max;
    }
    return INVALID_ID;
}

void User::remove()
{
    m_db(remove_from(G_USERS).where(G_USERS.userId == this->user_id()));
}

void User::set_status(Status new_status)
{
    melon::core::User::set_status(new_status);
    m_db(update(G_USERS).set(G_USERS.status = static_cast<int>(new_status)).where(G_USERS.userId == this->user_id()));
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

// Get all Chats where user participate in
std::vector<melon::core::Chat> get_chats_for_user(sqlpp::mysql::connection& db, melon::core::User user)
{
    std::vector<melon::core::Chat> chats_for_user;
    for (const auto& row : db(select(all_of(G_CHATS))
                              .from(G_CHATSUSERS.join(G_CHATS).on(G_CHATSUSERS
                              .chatId == G_CHATS.chatId
                              and G_CHATSUSERS.domainId == G_CHATS.domainId)).
                              where(G_CHATSUSERS.userId == user.user_id())))
    {
        chats_for_user.emplace_back(row.chatId, row.domainId, row.chatname);
    }
    return chats_for_user;
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

// List of usernames of online users in database
std::vector<std::string> get_online_users_names(sqlpp::mysql::connection& db)
{
    std::vector<std::string> online_users_names;
    for (const auto& row : db(select(G_USERS.username).from(G_USERS).where(G_USERS.status == static_cast<std::uint8_t>(melon::core::User::Status::ONLINE))))
    {
        online_users_names.emplace_back(row.username);
    }
    return online_users_names;
}

// List of online mc::Users in database
std::vector<melon::core::User> get_online_users(sqlpp::mysql::connection& db)
{
    std::vector<melon::core::User> online_users;
    for (const auto& row : db(select(all_of(G_USERS)).from(G_USERS).where(G_USERS.status == static_cast<std::uint8_t>(melon::core::User::Status::ONLINE))))
    {
        auto status = static_cast<melon::core::User::Status>(static_cast<int>(row.status));
        online_users.emplace_back(row.userId, row.domainId, row.username, status);
    }
    return online_users;
}


/* Chats */

std::vector<melon::core::User> get_users_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat)
{
    std::vector<melon::core::User> users_in_chat;
    for (const auto& row : db(select(all_of(G_USERS))
                                  .from(G_CHATSUSERS.join(G_USERS).on(G_CHATSUSERS
                                  .userId == G_USERS.userId))
                                  .where(G_CHATSUSERS.chatId == chat.chat_id() and G_CHATSUSERS.domainId == chat.domain_id())))
    {
        auto status = static_cast<melon::core::User::Status>(static_cast<int>(row.status));
        users_in_chat.emplace_back(row.userId, row.domainId, row.username, status);
    }
    return users_in_chat;
}

std::vector<melon::core::Message> get_messages_for_chat(sqlpp::mysql::connection& db, const melon::core::Chat& chat)
{
    std::vector<melon::core::Message> messages_in_chat;
    for (const auto& row : db(select(all_of(G_MESSAGES)).from(G_MESSAGES).where(G_MESSAGES.chatId == chat.chat_id())))
    {
        auto status = static_cast<melon::core::Message::Status>(static_cast<int>(row.status));
        messages_in_chat.emplace_back(row.messageId, row.domainId, row.userId, row.chatId, row.text, status);
    }
    return messages_in_chat;
}

}  // namespace melon::server::storage
