#include <iostream>

#include "storage.hpp"

SQLPP_ALIAS_PROVIDER(now)

int main() try
{
    namespace mss = melon::server::storage;
    namespace mc = melon::core;


    mysql::connection db(mss::config_melondb());

    mc::User user("h3ll0kitt1");
    mss::add_user(db, user);


    mc::Chat chat(1,"secret_chat");
    mss::add_chat(db, chat);


    // for GMT time; in Mysql is like SELECT UTC_TIMESTAMP();
    for (const auto& row : db(select(::sqlpp::value(std::chrono::system_clock::now()).as(now))))
    {
        std::cout << row.now << std::endl;
    }

    mc::Message message("Let's protest", mc::Message::Status::SENT, true, 1, 1);
    mss::add_message(db, message);

    mc::Message message2("or go to OVD", mc::Message::Status::RECIEVED, false, 1, 1);
    mss::add_message(db, message2);

    std::vector<mc::Message> chat_message = mss::get_messages_for_chat(db, chat);
    std::cout << "Messages of chat: " << std::endl;
    for (const auto& a: chat_message)
    {
        std:: cout << "text: " << a.text() << std::endl;
    }

    std::vector<mc::User> online_users = mss::get_online_users(db);
    std::cout << "Online users: " << std::endl;
    for (auto& a: online_users)
    {
        std:: cout << a.userid() << " : " << a.username() << std::endl;
    }

    std::cout << "Change status for online" << std::endl;
    mss::make_user_online(db, user);
    std::vector<std::string> online_users_names = mss::get_online_users_names(db);

    std::cout << "Online users: " << std::endl;
    for (const auto &a: online_users_names)
    {
        std:: cout << a << std::endl;
    }

    std::cout << "Change status for offline" << std::endl;
    mss::make_user_offline(db, user);
    online_users_names = mss::get_online_users_names(db);

    std::cout << "Online users: " << std::endl;
    for (auto& a: online_users_names)
    {
        std:: cout << a << std::endl;
    }

}
catch (const sqlpp::exception& e)
{
    std::cerr << "For testing, you'll need to create a database melon for user 'melon' on localhost with password 'melonpass'" << std::endl;
    std::cerr << e.what() << std::endl;
}
