#include <storage.hpp>

#include <iostream>
#include <stdexcept>

int main() try
{
    namespace mss = melon::server::storage;
    namespace mc = melon::core;
    namespace mysql = sqlpp::mysql;

    mysql::connection db(mss::config_melondb());

    // Domains
    std::string domain_ed = "Brizil server";
    mss::Domain domain_edward(db, domain_ed, false);
    std::cout << "Domain id is " << domain_edward.domain_id() << "\n";
    std::cout << "Domain type is " << domain_edward.external() << "\n";

    mss::Domain found_domain_edward(db, domain_ed);
    std::cout << "Domain id is " << found_domain_edward.domain_id() << "\n";
    std::cout << "Domain type is " << found_domain_edward.external() << "\n";
    std::cout << "Domain hostanme is " << found_domain_edward.hostname() << "\n";

    // Users
    mss::User user1(db, "Fudge", domain_edward.domain_id(), mc::User::Status::ONLINE);
    std::cout << "Current domain_id: " << user1.domain_id() <<"\n";
    std::cout << "Current user_id: " << user1.user_id() << "\n";

    std::string anna = "anna";

    mss::User anna_user(db, anna, domain_edward.domain_id(), mc::User::Status::ONLINE);
    mss::User found_anna(db, anna_user.username(), domain_edward.domain_id());
    std::cout << "Current domain_id: " << anna_user.domain_id() <<"\n";
    std::cout << "Current user_id: " << anna_user.user_id() << "\n";
    std::cout << "Current username: " << anna_user.username() << "\n";
    user1.set_status(mc::User::Status::OFFLINE);
    user1.remove();

    std::cout << "Get online usernames\n";
    std::vector<mc::User::Ptr> online_users = mss::get_online_users(db);
    for (const auto& a : online_users)
    {
        std::cout << a->user_id() << " : " << a->username() << '\n';
    }
    anna_user.set_status(mc::User::Status::OFFLINE);

    std::cout << "Get online usernames\n";
    online_users = mss::get_online_users(db);
    for (const auto& a : online_users)
    {
        std::cout << a->user_id() << " : " << a->username() << '\n';
    }

    // Chats
    mss::Chat chat1(db, domain_edward.domain_id(), "secret_chat6");
    std::cout << "Current chat_id: " << chat1.chat_id() << '\n';

    mss::Chat secret_chat(db, chat1.chat_id(), domain_edward.domain_id());
    std::cout << secret_chat.chat_id() << '\n';
    std::cout << secret_chat.domain_id() << '\n';
    std::cout << secret_chat.chatname() << '\n';

    std::vector<mc::User::Ptr> vec_users = secret_chat.get_users();
    std::cout << "Participants: \n";
    for (const auto& a : vec_users)
    {
        std::cout << a->username() << " - ";
    }
    std::cout << "\n\n";


    std::vector<mc::Chat::Ptr> vec_chats = anna_user.get_chats();
    for (const auto& a : vec_chats)
    {
        std::cout << a->chatname() << " - ";
    }
    std::cout << "\n\n";

    // Messages
    mss::Message message1(db, chat1.chat_id(), domain_edward.domain_id(), user1.user_id(), std::chrono::system_clock::now(), ":D", mc::Message::Status::SENT);
    std::cout << "Current message_id: " << message1.message_id() << '\n';
    message1.set_status(mc::Message::Status::SEEN);
    message1.set_text("((((((");

    domain_edward.remove();
    message1.remove();

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
