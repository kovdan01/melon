#include <storage.hpp>

#include <iostream>
#include <stdexcept>

int main() try
{
    namespace mss = melon::server::storage;
    namespace mc = melon::core;
    namespace mysql = sqlpp::mysql;

    mysql::connection db(mss::config_melondb());

    std::string domain1 = "Bonnie server";
    std::string domain2 = "Clyde server";

    /* Domains */

    mc::Domain domain(0, domain1, 0);
    //same hostnames are not allowed:
    //mss::add_domain(db, domain);
    //mss::add_domain(db, domain);


//    mss::remove_domain(db, domain);

    //add find hostname, if not exists add
    mss::find_domain_id(db, domain1);

    /* Users */

    mc::User user(3, 0, "h3ll0kitt1", mc::User::Status::ONLINE);
    mss::add_user(db, user, domain1);

    mc::User user2(2, 0, "Bey Hakim", mc::User::Status::OFFLINE);
    mss::add_user(db, user2, domain2);

    //same domain_id + username are not allowed
    //mss::add_user(db, user, domain1);



    std::cout << "Get all usernames\n";
    std::vector<std::string> all_usernames = mss::get_names_of_all_users(db);
    for (const auto& a : all_usernames)
    {
        std::cout << a << '\n';
    }

    mss::count_users(db);

    mss::remove_user(db, user2);
    mss::remove_user(db, user);
    std::cout << "Get all usernames\n";
    for (const auto& a : all_usernames)
    {
        std::cout << a << '\n';
    }

    /* Chats */

    mc::Chat chat(2, 0, "secret_chat");
    mss::add_chat(db, chat, domain1);
        //should add extra details to FK to dump to update or delete
    mss::update_chatname(db, "FBIchat", chat);
//    mss::remove_chat(db, chat);


    /* Messages */

    // at what moment determine chat_id ?

    mc::Message message(0, 8, 1, 1, "Let's protest", mc::Message::Status::SENT);
    mss::add_message(db, message);

    mss::count_number_recieved_messages(db);

    mc::Message message2(14, 8, 1, 1, "or go to OVD", mc::Message::Status::RECEIVED);
    mss::add_message(db, message2);
    mss::update_text(db, "You better watch yourself", message2);

    mss::count_number_recieved_messages(db);




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
