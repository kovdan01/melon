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
//    std::string domain_ed = "Edwar8 server";
//    mss::Domain domain_edward(db, domain_ed, true);
//    std::cout << "Domain id is " << domain_edward.domain_id() << "\n";

//    mss::Domain found_domain_edward(db, domain_ed);
//    std::cout << found_domain_edward.domain_id() << " : " << found_domain_edward.external() << " : " << found_domain_edward.domain_id() << "\n";

//    // Users
//    mss::User user1(db, domain_edward.domain_id(), "Don Juan", mc::User::Status::ONLINE);
//    std::cout << "Current domain_id: " << user1.domain_id() <<"\n";
//    std::cout << "Current user_id: " << user1.user_id() << "\n";

    mss::User anna_user(db, 1, "anna");
    std::cout << "Current domain_id: " << anna_user.domain_id() <<"\n";
    std::cout << "Current user_id: " << anna_user.user_id() << "\n";
    std::cout << "Current username: " << anna_user.username() << "\n";


//    user1.set_status(mc::User::Status::OFFLINE);
//    user1.remove();

//    mss::User user3(db, domain_edward.domain_id(), "anna", mc::User::Status::ONLINE);
//    mss::User user4(db, domain_edward.domain_id(), "erick", mc::User::Status::ONLINE);
//    mss::User user5(db, domain_edward.domain_id(), "silvia", mc::User::Status::ONLINE);
//    mss::User user6(db, domain_edward.domain_id(), "bunny", mc::User::Status::ONLINE);

//    std::cout << "Get online usernames\n";
//    std::vector<mc::User> online_users = mss::get_online_users(db);
//    for (const auto& a : online_users)
//    {
//        std::cout << a.user_id() << " : " << a.username() << '\n';
//    }
//    user6.set_status(mc::User::Status::OFFLINE);

//    std::cout << "Get online usernames\n";
//    online_users = mss::get_online_users(db);
//    for (const auto& a : online_users)
//    {
//        std::cout << a.user_id() << " : " << a.username() << '\n';
//    }

    // Chats
//    mss::Chat chat1(db, domain_edward.domain_id(), "secret_chat6");
//    std::cout << "Current chat_id: " << chat1.chat_id() << '\n';

    mss::Chat secret_chat(db, 1, 1);
    std::cout << secret_chat.chat_id() << '\n';
    std::cout << secret_chat.domain_id() << '\n';
    std::cout << secret_chat.chatname() << '\n';

    std::vector<mc::User> vec_users = mss::get_users_for_chat(db, secret_chat);
    std::cout << "Participants: \n";
    for (const auto& a : vec_users)
    {
        std::cout << a.username() << " - ";
    }
    std::cout << "\n\n";


    std::vector<mc::Chat> vec_chats = mss::get_chats_for_user(db, anna_user);
    for (const auto& a : vec_chats)
    {
        std::cout << a.chatname() << " - ";
    }
    std::cout << "\n\n";

//    // Messages
//    mss::Message message1(db, 0, domain_edward.domain_id(), user3.user_id(), chat1.chat_id(), ":D", mc::Message::Status::SENT);
//    std::cout << "Current message_id: " << message1.message_id() << '\n';
//    message1.set_status(mc::Message::Status::SEEN);
//    message1.set_text("((((((");

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
