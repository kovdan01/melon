#include <storage.hpp>

#include <iostream>
#include <stdexcept>

int main() try
{
    namespace mss = melon::server::storage;
    namespace mc = melon::core;
    namespace mysql = sqlpp::mysql;

    mysql::connection db(mss::config_melondb());



    std::string domain1 = "Anna7 server";
 //   std::string domain2 = "Clyde server";

//    /* Domains */

//    //same hostnames are not allowed:
    mss::Domain domain(db, 0, domain1, true);
    std::cout << "Domain id is " << domain.domain_id() << "\n";


//    mss::User user(db, 0, 0, "kitt_l0v3__", mc::User::Status::ONLINE, domain1);
//    // or
//    mss::User user2(db, 0, domain.domain_id(), "kitt1_r0ar", mc::User::Status::ONLINE);
//    std::cout << "Current domain_id: " << user.domain_id() <<"\n";
//    std::cout << "Current user_id: " << user.user_id() << "\n";
//    user.change_status(mc::User::Status::OFFLINE);

//    /* Users */

//    mc::User user(1, 0, "h3ll0kitt1", mc::User::Status::ONLINE);
//    mss::add_user(db, user, domain1);

//    mc::User user2(2, 0, "Bey Hakim", mc::User::Status::OFFLINE);
//    mss::add_user(db, user2, domain2);

//    //same domain_id + username are not allowed
//    //mss::add_user(db, user, domain1);

//    mc::User user3(3, 0, "Anna Maria", mc::User::Status::OFFLINE);
//    mss::add_user(db, user3, domain1);


//    std::cout << "Get all usernames\n";
//    std::vector<std::string> all_usernames = mss::get_names_of_all_users(db);
//    for (const auto& a : all_usernames)
//    {
//        std::cout << a << '\n';
//    }

////    mss::remove_user(db, user2);
////    mss::remove_user(db, user);
//    std::cout << "Get all usernames\n";
//    for (const auto& a : all_usernames)
//    {
//        std::cout << a << '\n';
//    }

//    /* Chats */

//    mc::Chat chat(1, 0, "secret_chat");
//    mss::add_chat(db, chat, domain1);
////    mss::remove_chat(db, chat);

//    mc::Chat chat2(2, 0, "secret_chat");
//    mss::update_chatname(db, "party time", chat2);

//    /* Messages */

//    mc::Message message(0, 1, 1, 2, "Let's protest", mc::Message::Status::SENT);

//    mss::Message message(db, 1, 1, 1, 1, "Let's protest", mc::Message::Status::SENT);
//    //mss::add_message(db, message);
//    message.update_text("This is smth new");

//    mss::count_number_recieved_messages(db);

//    mc::Message message2(2, 2, 2, 1, "or go to OVD", mc::Message::Status::RECEIVED);
////    mss::add_message(db, message2);
//    //mss::update_text(db, "You better watch yourself", message2);

//    mss::count_number_recieved_messages(db);

//    //Should I delete all chats if hostname is deleted ??
//    mss::remove_chat(db, chat2);





//    std::vector<mc::Message> chat_message = mss::get_messages_for_chat(db, chat);
//    std::cout << "Messages of chat:\n";
//    for (const auto& a : chat_message)
//    {
//        std::cout << "text: " << a.text() << '\n';
//    }

//    std::vector<mc::User> online_users = mss::get_online_users(db);
//    std::cout << "Online users:\n";
//    for (const auto& a : online_users)
//    {
//        std::cout << a.user_id() << " : " << a.username() << '\n';
//    }

//    std::cout << "Change status for online\n";
//    mss::make_user_online(db, user);
//    std::vector<std::string> online_users_names = mss::get_online_users_names(db);

//    std::cout << "Online users:\n";
//    for (const auto& a : online_users_names)
//    {
//        std::cout << a << '\n';
//    }

//    std::cout << "Change status for offline\n";
//    mss::make_user_offline(db, user);
//    online_users_names = mss::get_online_users_names(db);

//    std::cout << "Online users:\n";
//    for (const auto& a : online_users_names)
//    {
//        std::cout << a << '\n';
//    }
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
