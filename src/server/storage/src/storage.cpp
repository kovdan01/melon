#include <melon/db_scheme.h>
#include <storage.hpp>

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/update.h>

#include <vector>


namespace melon::server::storage
{

namespace mc = melon::core;

const melon::Users G_USERS;
const melon::Messages G_MESSAGES;
const melon::Domains G_DOMAINS;
const melon::Chats G_CHATS;
const melon::ChatsUsers G_CHATSUSERS;


std::shared_ptr<sqlpp::mysql::connection_config> config_db()
{
    auto config = std::make_shared<sqlpp::mysql::connection_config>();
    config->user = "melon";
    config->database = "melon";
    config->host = "localhost";
    config->password = "melonpass";
#ifndef NDEBUG
    config->debug = true;
#else
    config->debug = false;
#endif
    return config;
}


IdNotFoundException::~IdNotFoundException() = default;


void check_if_domain_exists(sqlpp::mysql::connection& db, id_t domain_id)
{
    if (db(select(max(G_DOMAINS.domainId)).from(G_DOMAINS).where(G_DOMAINS.domainId == domain_id)).empty())
        throw IdNotFoundException("No domain with id " + std::to_string(domain_id));
}

void check_if_chat_exists(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id)
{
    if (db(select(max(G_CHATS.chatId)).from(G_CHATS).where(G_CHATS.chatId == chat_id && G_CHATS.domainId == domain_id)).empty())
        throw IdNotFoundException("No chat with id " + std::to_string(chat_id) + " on domain with id " + std::to_string(domain_id));
}


id_t max_user_id(sqlpp::mysql::connection& db, id_t domain_id)
{
    auto result = db(select(max(G_USERS.userId)).from(G_USERS).where(G_USERS.domainId == domain_id));
    if (!result.empty())
    {
        const auto& row = result.front();
        return row.max;
    }
    return mc::INVALID_ID;
}

id_t max_chat_id(sqlpp::mysql::connection& db, id_t domain_id)
{
    check_if_domain_exists(db, domain_id);

    auto result =  db(select(max(G_CHATS.chatId)).from(G_CHATS).where(G_CHATS.domainId == domain_id));
    if (!result.empty())
    {
        const auto& row = result.front();
        return row.max;
    }
    return mc::INVALID_ID;
}

id_t max_message_id(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id)
{
    check_if_chat_exists(db, chat_id, domain_id);

    auto result = db(select(max(G_MESSAGES.messageId)).from(G_MESSAGES).where(G_MESSAGES.chatId == chat_id && G_MESSAGES.domainIdChat == domain_id));
    if (!result.empty())
    {
        const auto& row = result.front();
        return row.max;
    }
    return mc::INVALID_ID;
}

/* class Domain */

// For Insert
Domain::Domain(sqlpp::mysql::connection& db, std::string hostname, bool external)
     : mc::Domain(std::move(hostname), external)
     , m_db(db)
{
    this->set_domain_id(m_db(insert_into(G_DOMAINS).set(
        G_DOMAINS.hostname = this->hostname(),
        G_DOMAINS.external = static_cast<std::uint8_t>(this->external()))));
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
        this->set_external_base(external);
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

void Domain::set_external_impl()
{
    m_db(update(G_DOMAINS).set(G_DOMAINS.external = static_cast<std::uint8_t>(this->external())).where(G_DOMAINS.domainId == this->domain_id()));
}


/* class User */

// For Insert
User::User(sqlpp::mysql::connection& db, std::string username, id_t domain_id, Status status)
     : mc::User(std::move(username), domain_id, status)
     , m_db(db)
{
    set_user_id(max_user_id(db, domain_id) + 1);
    m_db(insert_into(G_USERS).set(
        G_USERS.userId   = this->user_id(),
        G_USERS.username = this->username(),
        G_USERS.domainId = this->domain_id(),
        G_USERS.status   = static_cast<int>(this->status())));
}

// For Select
User::User(sqlpp::mysql::connection& db, std::string username, id_t domain_id)
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

std::vector<mc::Chat::Ptr> User::get_chats() const
{
    std::vector<mc::Chat::Ptr> answer;
    for (const auto& row : m_db(select(all_of(G_CHATS)).from
                                (
                                    G_CHATSUSERS.join(G_CHATS).on
                                    (
                                        G_CHATSUSERS.chatId == G_CHATS.chatId &&
                                        G_CHATSUSERS.domainIdChat == G_CHATS.domainId
                                    )
                                ).where
                                (
                                    G_CHATSUSERS.userId == this->user_id() &&
                                    G_CHATSUSERS.domainIdUser == this->domain_id()
                                ))
         )
    {
        answer.emplace_back(std::make_unique<Chat>(m_db, row.chatId, row.domainId));
    }
    return answer;
}

void User::remove()
{
    m_db(remove_from(G_USERS).where(G_USERS.userId == this->user_id() && G_USERS.domainId == this->domain_id()));
}

void User::set_status_impl()
{
    m_db(update(G_USERS).set(G_USERS.status = static_cast<int>(this->status())).where(G_USERS.userId == this->user_id() &&
                                                                                      G_USERS.domainId == this->domain_id()));
}


/* class Chat */

// For Insert
Chat::Chat(sqlpp::mysql::connection& db, id_t domain_id, std::string chatname)
    : mc::Chat(domain_id, std::move(chatname))
    , m_db(db)
{
    set_chat_id(max_chat_id(db, domain_id) + 1);
    m_db(insert_into(G_CHATS).set(
        G_CHATS.chatId   = this->chat_id(),
        G_CHATS.domainId = this->domain_id(),
        G_CHATS.chatname = this->chatname()));
}

// For Select
Chat::Chat(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id)
    : mc::Chat(chat_id, domain_id)
    , m_db(db)
{
    auto result = db(select(G_CHATS.chatname).from(G_CHATS).where(G_CHATS.chatId == this->chat_id() &&
                                                                  G_CHATS.domainId == this->domain_id()));
    if (!result.empty())
    {
        const auto& row = result.front();
        this->set_chatname_base(row.chatname);
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
                                        G_CHATSUSERS.userId == G_USERS.userId &&
                                        G_CHATSUSERS.domainIdUser == G_USERS.domainId
                                    )
                                ).where
                                (
                                    G_CHATSUSERS.chatId == this->chat_id() &&
                                    G_CHATSUSERS.domainIdChat == this->domain_id()
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
                                                                                  G_MESSAGES.domainIdChat == this->domain_id())))
    {
        answer.emplace_back(std::make_unique<Message>(m_db, row.messageId, row.chatId, row.domainIdChat));
    }
    return answer;
}

void Chat::set_chatname_impl()
{
    m_db(update(G_CHATS).set(G_CHATS.chatname = this->chatname()).where(G_CHATS.chatId == this->chat_id() &&
                                                                        G_CHATS.domainId == this->domain_id()));
}

// Deletes chat: deletes messages from chat, all info in Chats_Users concearnig this chat
void Chat::remove()
{
    m_db(remove_from(G_CHATS).where(G_CHATS.chatId == this->chat_id() && G_CHATS.domainId == this->domain_id()));
}


/* class Message */

// For Insert
Message::Message(sqlpp::mysql::connection& db, id_t chat_id, id_t domain_id_chat, id_t user_id, id_t domain_id_user,
                 std::string text, std::chrono::system_clock::time_point timestamp, Status status)
     : mc::Message(chat_id, domain_id_chat, user_id, domain_id_user, std::move(text), timestamp, status)
     , m_db(db)
{
    set_message_id(max_message_id(db, chat_id, domain_id_chat) + 1);
    m_db(insert_into(G_MESSAGES).set(
        G_MESSAGES.messageId     = this->message_id(),
        G_MESSAGES.chatId        = this->chat_id(),
        G_MESSAGES.domainIdChat  = this->domain_id_chat(),
        G_MESSAGES.userId        = this->user_id(),
        G_MESSAGES.domainIdUser  = this->domain_id_user(),
        G_MESSAGES.text          = this->text(),
        G_MESSAGES.sendtime      = this->timestamp(),
        G_MESSAGES.status        = static_cast<std::uint8_t>(this->status())));
}

// For Select
Message::Message(sqlpp::mysql::connection& db, id_t message_id, id_t chat_id, id_t domain_id_chat)
     : mc::Message(message_id, chat_id, domain_id_chat)
     , m_db(db)
{
    auto result = db(select(G_MESSAGES.userId, G_MESSAGES.domainIdUser, G_MESSAGES.text, G_MESSAGES.status, G_MESSAGES.sendtime)
                     .from(G_MESSAGES).where(G_MESSAGES.messageId == this->message_id() &&
                                             G_MESSAGES.chatId == this->chat_id() &&
                                             G_MESSAGES.domainIdChat == this->domain_id_chat()));
    if (!result.empty())
    {
        const auto& row = result.front();

        auto status = static_cast<mc::Message::Status>(static_cast<int>(row.status));

        this->set_user_id(row.userId);
        this->set_domain_id_user(row.domainIdUser);
        this->set_text_base(row.text);
        this->set_timestamp_base(row.sendtime.value());
        this->set_status_base(status);
    }
    else
    {
        throw IdNotFoundException("No message in database");
    }
}

void Message::remove()
{
    m_db(remove_from(G_MESSAGES).where(G_MESSAGES.messageId == this->message_id() &&
                                       G_MESSAGES.chatId == this->chat_id() &&
                                       G_MESSAGES.domainIdChat == this->domain_id_chat()));
}

void Message::set_text_impl()
{
    m_db(update(G_MESSAGES).set(G_MESSAGES.text = this->text()).where(G_MESSAGES.messageId == this->message_id() &&
                                                                      G_MESSAGES.chatId == this->chat_id() &&
                                                                      G_MESSAGES.domainIdChat == this->domain_id_chat()));
}

void Message::set_status_impl()
{

    m_db(update(G_MESSAGES).set(G_MESSAGES.status = static_cast<std::uint8_t>(this->status())).where(G_MESSAGES.messageId == this->message_id() &&
                                                                                                     G_MESSAGES.chatId == this->chat_id() &&
                                                                                                     G_MESSAGES.domainIdChat == this->domain_id_chat()));
}

void Message::set_timestamp_impl()
{
    m_db(update(G_MESSAGES).set(G_MESSAGES.sendtime = this->timestamp()).where(G_MESSAGES.messageId == this->message_id() &&
                                                                               G_MESSAGES.chatId == this->chat_id() &&
                                                                               G_MESSAGES.domainIdChat == this->domain_id_chat()));
}


/* Users */

std::vector<std::string> get_all_usernames(sqlpp::mysql::connection& db)
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
