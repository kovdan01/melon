#include <entities.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace mc = melon::core;
namespace mss = melon::server::storage;
namespace mysql = sqlpp::mysql;

static std::ostream& operator<<(std::ostream& o, const mc::Domain& domain)
{
    o << "Domain { "
      << "domain_id: " << domain.domain_id() << ", "
      << "hostname: \"" << domain.hostname() << "\", "
      << "external: " << domain.external() << " } ";
    return o;
}

static std::ostream& operator<<(std::ostream& o, const mc::User::Status status)
{
    switch (status)
    {
    case mc::User::Status::CHILL:
        o << "CHILL";
        break;
    case mc::User::Status::OFFLINE:
        o << "OFFLINE";
        break;
    case mc::User::Status::ONLINE:
        o << "ONLINE";
        break;
    }
    return o;
}

static std::ostream& operator<<(std::ostream& o, const mc::User& user)
{
    o << "User { "
      << "user_id: " << user.user_id() << ", "
      << "domain_id: " << user.domain_id() << ", "
      << "username: \"" << user.username() << "\", "
      << "status: " << user.status() << " } ";
    return o;
}

static std::ostream& operator<<(std::ostream& o, const mc::Chat& chat)
{
    o << "Chat { "
      << "chat_id: " << chat.chat_id() << ", "
      << "domain_id: " << chat.domain_id() << ", "
      << "chatname: \"" << chat.chatname() << "\" } ";
    return o;
}

static std::ostream& operator<<(std::ostream& o, const mc::Message::Status status)
{
    switch (status)
    {
    case mc::Message::Status::FAIL:
        o << "FAIL";
        break;
    case mc::Message::Status::SENT:
        o << "SENT";
        break;
    case mc::Message::Status::RECEIVED:
        o << "RECEIVED";
        break;
    case mc::Message::Status::SEEN:
        o << "SEEN";
        break;
    }
    return o;
}

static std::ostream& operator<<(std::ostream& o, const mc::Message& message)
{
    o << "Message { "
      << "message_id: " << message.message_id() << ", "
      << "chat_id: " << message.chat_id() << ", "
      << "domain_id_chat: " << message.domain_id_chat() << ", "
      << "user_id: " << message.user_id() << ", "
      << "domain_id_user: " << message.domain_id_user() << ", "
      << "text: \"" << message.text() << "\", "
      << "status: " << message.status() << " } ";
    return o;
}

static void get_online_users(mysql::connection& db)
{
    std::vector<mss::User> online_users = mss::get_online_users(db);
    if (online_users.empty())
    {
        std::cout << "There are no users online\n";
        return;
    }

    std::cout << "Online users:\n";
    for (const auto& online_user : online_users)
        std::cout << "    " << online_user << '\n';
}

static void get_chat_participants(const mss::Chat& chat)
{
    std::vector<mss::User> chat_participants = chat.get_users();
    if (chat_participants.empty())
    {
        std::cout << "No participants in " << chat << '\n';
        return;
    }

    std::cout << "Participants of " << chat << '\n';
    for (const auto& chat_participant : chat_participants)
        std::cout << "    " << chat_participant << '\n';
}

static void get_chats_for_user(const mss::User& user)
{
    std::vector<mss::Chat> chats = user.get_chats();
    if (chats.empty())
    {
        std::cout << "No chats for " << user << '\n';
        return;
    }

    std::cout << "Chats for " << user << '\n';
    for (const auto& chat : chats)
        std::cout << "    " << chat << '\n';
}

int main() try
{
    mysql::connection db(mss::config_db());

    // Domains
    mss::Domain domain1(db, "Brazil server", false);
    std::cout << "Created domain1: " << domain1 << '\n';

    mss::Domain found_domain1(db, domain1.hostname());
    std::cout << "Found domain1: " << found_domain1 << '\n';

    mss::Domain domain2(db, "melon.org", false);
    std::cout << "Created domain2: " << domain2 << '\n';

    // Users
    mss::User user1(db, "Fudge", domain1.domain_id(), mc::User::Status::ONLINE);
    std::cout << "Created user1: " << user1 << '\n';

    mss::User found_user1(db, user1.username(), user1.domain_id());
    std::cout << "Found user1:   " << found_user1 << '\n';

    mss::User user2(db, "Anna", domain1.domain_id(), mc::User::Status::ONLINE);
    std::cout << "Created user2: " << user2 << '\n';

    mss::User found_user2(db, user2.username(), user2.domain_id());
    std::cout << "Found user2:   " << found_user2 << '\n';

    user1.set_status(mc::User::Status::OFFLINE);
    std::cout << "Set status of user1 to offline\n";

    mss::User found_user1_offline = mss::User(db, user1.username(), user1.domain_id());
    std::cout << "Found user1:   " << found_user1_offline << '\n';

    std::cout << "Remove user1\n";
    user1.remove();
    try
    {
        [[maybe_unused]] mss::User found_user1_deleted = mss::User(db, user1.username(), user1.domain_id());
        assert(false);
    }
    catch (const mss::IdNotFoundException& e)
    {
        std::cout << "Not found user1\n";
    }

    mss::User user3(db, "Bob", domain2.domain_id(), mc::User::Status::OFFLINE);
    std::cout << "Created user3: " << user3 << '\n';

    get_online_users(db);

    std::cout << "Set status of user2 to offline\n";
    user2.set_status(mc::User::Status::OFFLINE);

    get_online_users(db);

    // Chats
    mss::Chat chat1(db, domain1.domain_id(), "secret_chat");
    std::cout << "Created chat1: " << chat1 << '\n';

    mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
    std::cout << "Found chat1:   " << found_chat1 << '\n';

    found_chat1.add_user(user1);
    found_chat1.add_user(user2);

    get_chat_participants(chat1);

    get_chats_for_user(user2);

    mss::Chat chat2(db, domain2.domain_id(), "On domain2");
    std::cout << "Created chat2: " << chat2 << '\n';

    // Messages
    mss::Message message1(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(), ":D", std::chrono::system_clock::now(), mc::Message::Status::SENT);
    std::cout << "Created message1: " << message1 << '\n';

    message1.set_status(mc::Message::Status::SEEN);
    std::cout << "Set message1 status to SEEN\n";

    message1.set_text("((((((");
    std::cout << "Set message1 text to \"((((((\"\n";

    mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
    std::cout << "Found message1:   " << found_message1 << '\n';

    mss::Message message2(db, chat2.chat_id(), chat2.domain_id(), user2.user_id(), user2.domain_id(), "Lorem ipsum", std::chrono::system_clock::now(), mc::Message::Status::SENT);
    std::cout << "Created message2: " << message2 << '\n';

    std::cout << "Remove domain1";
    domain1.remove();
    try
    {
        [[maybe_unused]] mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
        assert(false);
    }
    catch (const mss::IdNotFoundException& e)
    {
        std::cout << "Not found chat1\n";
    }

    try
    {
        [[maybe_unused]] mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
        assert(false);
    }
    catch (const mss::IdNotFoundException& e)
    {
        std::cout << "Not found message1\n";
    }

    std::cout << "Remove user2\n";
    user2.remove();
    try
    {
        [[maybe_unused]] mss::Message found_message2(db, message2.message_id(), message2.chat_id(), message2.domain_id_chat());
        assert(false);
    }
    catch (const mss::IdNotFoundException& e)
    {
        std::cout << "Not found message2\n";
    }
}
catch (const sqlpp::exception& e)
{
    std::cerr << "For testing, you'll need to create a database melon for user 'melon' on localhost with password 'melonpass'" << std::endl;
    std::cerr << e.what() << std::endl;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
catch (...)
{
    std::cerr << "An unknown error occurred\n";
}
