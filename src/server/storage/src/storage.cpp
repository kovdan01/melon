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

namespace mc = melon::core;

const melon::Users G_USERS;
const melon::Messages G_MESSAGES;
const melon::Domains G_DOMAINS;
const melon::Chats G_CHATS;
const melon::ChatsUsers G_CHATSUSERS;

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
     : mc::Domain(std::move(hostname), external)
     , m_db(db)
{
    m_db(insert_into(G_DOMAINS).set(G_DOMAINS.hostname = this->hostname(),
                                    G_DOMAINS.external = static_cast<std::uint8_t>(this->external())));
    std::uint64_t this_domain_id = max_domain_id(m_db);
    assert(this_domain_id != mc::INVALID_ID);
    this->set_domain_id(this_domain_id);
}

// For Select
Domain::Domain(sqlpp::mysql::connection& db, std::string hostname)
     : mc::Domain(std::move(hostname))
     , m_db(db)
{
    auto result = db(select(G_DOMAINS.domainId, G_DOMAINS.external).from(G_DOMAINS).where(G_DOMAINS.hostname == this->hostname()));
    if (!result.empty())
    {
        const auto& row = result.front();

        bool external = row.external;

        this->set_domain_id(row.domainId);
        this->set_external(external);
    }
    else
    {
        throw IdNotFoundException("No hostname in database");
    }
}

void Domain::remove()
{
    m_db(remove_from(G_DOMAINS).where(G_DOMAINS.domainId == this->domain_id()));
}

void Domain::set_external_impl(bool external)
{
    m_db(update(G_DOMAINS).set(G_DOMAINS.external = static_cast<std::uint8_t>(external)).where(G_DOMAINS.domainId == this->domain_id()));
}

std::uint64_t max_domain_id(sqlpp::mysql::connection& db)
{
    auto result = db(select(max(G_DOMAINS.domainId)).from(G_DOMAINS).unconditionally());
    if (!result.empty())
    {
        const auto& row = result.front();
        return row.max;
    }
    return mc::INVALID_ID;
}

/* class User */

// For Insert
User::User(sqlpp::mysql::connection& db, std::string username, std::uint64_t domain_id, Status status)
     : mc::User(std::move(username), domain_id, status)
     , m_db(db)
{
    m_db(insert_into(G_USERS).set(G_USERS.username = this->username(),
                                  G_USERS.domainId = this->domain_id(),
                                  G_USERS.status = static_cast<int>(this->status())));
    std::uint64_t this_user_id = max_user_id(m_db);
    assert(this_user_id != mc::INVALID_ID);
    this->set_user_id(this_user_id);
}

// For Select
User::User(sqlpp::mysql::connection& db, std::string username, std::uint64_t domain_id)
    : mc::User(std::move(username), domain_id)
    , m_db(db)
{
    auto result = db(select(G_USERS.userId, G_USERS.status)
                     .from(G_USERS).where(G_USERS.username == this->username() &&
                                          G_USERS.domainId == this->domain_id()));
    if (!result.empty())
    {
        const auto& row = result.front();

        auto status = static_cast<mc::User::Status>(static_cast<int>(row.status));

        this->set_user_id(row.userId);
        this->set_status_base(status);
    }
    else
    {
        throw IdNotFoundException("No user in database");
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
    return mc::INVALID_ID;
}

std::vector<mc::Chat::Ptr> User::get_chats() const
{
    std::vector<mc::Chat::Ptr> answer;
    for (const auto& row : m_db(select(all_of(G_CHATS)).from
                                (
                                    G_CHATSUSERS.join(G_CHATS).on
                                    (
                                        G_CHATSUSERS.chatId == G_CHATS.chatId &&
                                        G_CHATSUSERS.domainId == G_CHATS.domainId
                                    )
                                ).where(G_CHATSUSERS.userId == this->user_id()))
         )
    {
        answer.emplace_back(std::make_unique<Chat>(m_db, row.chatId, row.domainId));
    }
    return answer;
}

void User::remove()
{
    m_db(remove_from(G_USERS).where(G_USERS.userId == this->user_id()));
}

void User::set_status_impl(Status status)
{
    m_db(update(G_USERS).set(G_USERS.status = static_cast<int>(status)).where(G_USERS.userId == this->user_id() &&
                                                                              G_USERS.domainId == this->domain_id()));
}


/* class Chat */

// For Insert
Chat::Chat(sqlpp::mysql::connection& db, std::uint64_t domain_id, std::string chatname)
    : mc::Chat(domain_id, std::move(chatname))
    , m_db(db)
{
    m_db(insert_into(G_CHATS).set(G_CHATS.domainId = this->domain_id(),
                                  G_CHATS.chatname = this->chatname()));
    std::uint64_t this_chat_id = max_chat_id(m_db);
    assert(this_chat_id != mc::INVALID_ID);
    this->set_chat_id(this_chat_id);
}

// For Select
Chat::Chat(sqlpp::mysql::connection& db, std::uint64_t chat_id, std::uint64_t domain_id)
    : mc::Chat(chat_id, domain_id)
    , m_db(db)
{
    auto result = db(select(G_CHATS.chatname).from(G_CHATS).where(G_CHATS.chatId == this->chat_id() &&
                                                                  G_CHATS.domainId == this->domain_id()));
    if (!result.empty())
    {
        const auto& row = result.front();
        mc::Chat::set_chatname(row.chatname);
    }
    else
    {
        throw IdNotFoundException("No chat in database");
    }
}

std::vector<mc::User::Ptr> Chat::get_users() const
{
    std::vector<mc::User::Ptr> answer;
    for (const auto& row : m_db(select(all_of(G_USERS)).from
                                (
                                    G_CHATSUSERS.join(G_USERS).on
                                    (
                                        G_CHATSUSERS.userId == G_USERS.userId
                                    )
                                ).where
                                (
                                    G_CHATSUSERS.chatId == this->chat_id() &&
                                    G_CHATSUSERS.domainId == this->domain_id()
                                ))
         )
    {
        answer.emplace_back(std::make_unique<User>(m_db, row.username, row.domainId));
    }
    return answer;
}

std::vector<mc::Message::Ptr> Chat::get_messages() const
{
    std::vector<mc::Message::Ptr> answer;
    for (const auto& row : m_db(select(all_of(G_MESSAGES)).from(G_MESSAGES).where(G_MESSAGES.chatId == this->chat_id() &&
                                                                                  G_MESSAGES.domainId == this->domain_id())))
    {
        answer.emplace_back(std::make_unique<Message>(m_db, row.messageId, row.chatId, row.domainId));
    }
    return answer;
}

void Chat::set_chatname_impl(const std::string& chatname)
{
    m_db(update(G_CHATS).set(G_CHATS.chatname = chatname).where(G_CHATS.chatId == this->chat_id() &&
                                                                G_CHATS.domainId == this->domain_id()));
}

// Deletes chat: deletes messages from chat, all info in Chats_Users concearnig this chat
void Chat::remove()
{
    m_db(remove_from(G_CHATS).where(G_CHATS.chatId == this->chat_id()));
}

std::uint64_t max_chat_id(sqlpp::mysql::connection& db)
{
    auto result =  db(select(max(G_CHATS.chatId)).from(G_CHATS).unconditionally());
    if (!result.empty())
    {
        const auto& row = result.front();
        return row.max;
    }
    return mc::INVALID_ID;
}


/* class Message */

// For Insert
Message::Message(sqlpp::mysql::connection& db, std::uint64_t user_id, std::uint64_t chat_id, std::uint64_t domain_id,
                 std::chrono::system_clock::time_point timestamp, std::string text, Status status)
     : mc::Message(user_id, chat_id, domain_id, timestamp, std::move(text), status)
     , m_db(db)
{
    m_db(insert_into(G_MESSAGES).set(G_MESSAGES.text = this->text(),
                                     G_MESSAGES.timesend = timestamp,
                                     G_MESSAGES.status = static_cast<std::uint8_t>(this->status()),
                                     G_MESSAGES.domainId = this->domain_id(),
                                     G_MESSAGES.userId = this->user_id(),
                                     G_MESSAGES.chatId = this->chat_id()));
    std::uint64_t this_message_id = max_message_id(m_db);
    assert(this_message_id != mc::INVALID_ID);
    this->set_message_id(this_message_id);
}

// For Select
Message::Message(sqlpp::mysql::connection& db, std::uint64_t message_id, std::uint64_t chat_id, std::uint64_t domain_id)
     : mc::Message(message_id, chat_id, domain_id)
     , m_db(db)
{
    auto result = db(select(G_MESSAGES.userId, G_MESSAGES.text, G_MESSAGES.status, G_MESSAGES.timesend)
                     .from(G_MESSAGES).where(G_MESSAGES.chatId == this->chat_id() &&
                                             G_MESSAGES.domainId == this->domain_id() &&
                                             G_MESSAGES.messageId == this->message_id()));
    if (!result.empty())
    {
        const auto& row = result.front();

        auto status = static_cast<mc::Message::Status>(static_cast<int>(row.status));

        this->set_user_id(row.userId);
        this->set_text(row.text);
        this->set_timestamp(row.timesend.value());
        this->set_status(status);
    }
    else
    {
        throw IdNotFoundException("No message in database");
    }
}

std::uint64_t max_message_id(sqlpp::mysql::connection& db)
{
    auto result = db(select(max(G_MESSAGES.messageId)).from(G_MESSAGES).unconditionally());
    if (!result.empty())
    {
        const auto& row = result.front();
        return row.max;
    }
    return mc::INVALID_ID;
}

void Message::remove()
{
    m_db(remove_from(G_MESSAGES).where(G_MESSAGES.messageId == this->message_id()));
}

void Message::set_text_impl(const std::string& text)
{
    m_db(update(G_MESSAGES).set(G_MESSAGES.text = text).where(G_MESSAGES.messageId == this->message_id() &&
                                                              G_MESSAGES.chatId == this->chat_id() &&
                                                              G_MESSAGES.domainId == this->domain_id()));
}

void Message::set_status_impl(Status status)
{
    m_db(update(G_MESSAGES).set(G_MESSAGES.status = static_cast<std::uint8_t>(status)).where(G_MESSAGES.messageId == this->message_id() &&
                                                                                             G_MESSAGES.chatId == this->chat_id() &&
                                                                                             G_MESSAGES.domainId == this->domain_id()));
}

void Message::set_timestamp_impl(timestamp_t timestamp)
{
    m_db(update(G_MESSAGES).set(G_MESSAGES.timesend = timestamp).where(G_MESSAGES.messageId == this->message_id() &&
                                                                       G_MESSAGES.chatId == this->chat_id() &&
                                                                       G_MESSAGES.domainId == this->domain_id()));

}


/* Users */

std::vector<std::string> get_names_of_all_users(sqlpp::mysql::connection& db)
{
    std::vector<std::string> answer;
    for (const auto& row : db(select(G_USERS.username).from(G_USERS).unconditionally()))
    {
        answer.emplace_back(row.username);
    }
    return answer;
}

std::vector<mc::User::Ptr> get_online_users(sqlpp::mysql::connection& db)
{
    std::vector<mc::User::Ptr> answer;
    for (const auto& row : db(select(all_of(G_USERS)).from(G_USERS).where(G_USERS.status == static_cast<std::uint8_t>(mc::User::Status::ONLINE))))
    {
        answer.emplace_back(std::make_unique<User>(db, row.username, row.domainId));
    }
    return answer;
}

}  // namespace melon::server::storage
