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
    std::string domain_ed = "Edward3";
    mss::Domain domain_edward(db, 0, domain_ed, true);
    std::cout << "Domain id is " << domain_edward.domain_id() << "\n";

    // Users
    // 1. Constructor
    mss::User user1(db, 0, domain_edward.domain_id(), "kitt12", mc::User::Status::ONLINE);
    std::cout << "Current domain_id: " << user1.domain_id() <<"\n";
    std::cout << "Current user_id: " << user1.user_id() << "\n";
    user1.change_status(mc::User::Status::OFFLINE);
    user1.remove_user();

    // 2. Constructor [fails to create user2 if there is no domain in list]
//    mss::User user2(db, 0, 0, "kitt1", mc::User::Status::ONLINE, "not_in_list_domain");
//    std::cout << "Current domain_id: " << user1.domain_id() <<"\n";
//    std::cout << "Current user_id: " << user1.user_id() << "\n";

    mss::User user3(db, 0, domain_edward.domain_id(), "anna", mc::User::Status::ONLINE);
    mss::User user4(db, 0, domain_edward.domain_id(), "erick", mc::User::Status::ONLINE);
    mss::User user5(db, 0, domain_edward.domain_id(), "silvia", mc::User::Status::ONLINE);
    mss::User user6(db, 0, domain_edward.domain_id(), "bunny", mc::User::Status::ONLINE);

    std::cout << "Get online usernames\n";
    std::vector<mc::User> online_users = mss::get_online_users(db);
    for (const auto& a : online_users)
    {
        std::cout << a.user_id() << " : " << a.username() << '\n';
    }
    user6.change_status(mc::User::Status::OFFLINE);

    std::cout << "Get online usernames\n";
    online_users = mss::get_online_users(db);
    for (const auto& a : online_users)
    {
        std::cout << a.user_id() << " : " << a.username() << '\n';
    }

    // Chats
    // 1. Constructor
    mss::Chat chat1(db, 0, domain_edward.domain_id(), "secret_chat");
    std::cout << "Current chat_id: " << chat1.chat_id() << '\n';
    // 2. Constructor [fails to create user2 if there is no domain in list]
//    mss::Chat chat2(db, 0, 0, "secret_chat", "not_in_the_list_domain");

    // Messages
    mss::Message message1(db, 0, domain_edward.domain_id(), user1.user_id(), chat1.chat_id(), ":D", mc::Message::Status::SENT);
    std::cout << "Current message_id: " << message1.message_id() << '\n';
    message1.change_status(mc::Message::Status::SEEN);
    message1.update_text("((((((");
    message1.remove_message();

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
