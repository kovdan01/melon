#include <iostream>

#include "storage.hpp"


int main() try
{
    namespace mss = melon::server::storage;

    mysql::connection db(mss::config_melondb());
    db.execute(R"(DROP TABLE IF EXISTS messages)");
    db.execute(R"(DROP TABLE IF EXISTS users)");
    db.execute(R"(DROP TABLE IF EXISTS chats)");

    db.execute(R"(CREATE TABLE users (
        user_id  BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
        username varchar(255) NOT NULL,
        status TINYINT UNSIGNED NOT NULL DEFAULT 0,
        PRIMARY KEY (user_id)
        ))");

    db.execute(R"(CREATE TABLE chats (
        chat_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
        chatname varchar(255) NOT NULL,
        PRIMARY KEY (chat_id)
        ))");

    db.execute(R"(CREATE TABLE messages (
        message_id BIGINT UNSIGNED  NOT NULL AUTO_INCREMENT,
        text varchar(1024) NOT NULL,
        status TINYINT UNSIGNED  NOT NULL DEFAULT 0,
        seen BOOLEAN NOT NULL DEFAULT 0,
        timesend DATETIME NOT NULL,
        user_id BIGINT UNSIGNED  NOT NULL,
        chat_id INT UNSIGNED  NOT NULL,
        FOREIGN KEY (chat_id)  REFERENCES chats (chat_id),
        FOREIGN KEY (user_id)  REFERENCES users (user_id),
        PRIMARY KEY (message_id)
        ))");

    //check that time in mariadb

    User user;
    user.username = "h3ll0kitt1";
    mss::add_user(db, user);

    Chat chat;
    chat.chatid = 1;
    chat.chatname = "secret_chat";
    mss::add_chat(db, chat);

    Message message;
    message.text = "Let's protest";
    message.seen = 1;
    message.status = 1;
    message.timestamp = std::chrono::system_clock::now();
    message.user_id = 1;
    message.chat_id = 1;
    mss::add_message(db, message);

    message.text = "or go to OVD";
    message.seen = 0;
    message.status = 0;
    message.timestamp = std::chrono::system_clock::now();
    message.user_id = 1;
    message.chat_id = 1;
    mss::add_message(db, message);

    std::vector<Message> chat_message = mss::get_messages_for_chat(db, chat);
    std::cout << "Messages of chat: " << std::endl;
    for (const auto& a: chat_message)
    {
        std:: cout << "message id: " << a.messageid << "\ntext: " << a.text << std::endl;
    }

    std::vector<User> online_users = mss::get_online_users(db);
    std::cout << "Online users: " << std::endl;
    for (auto& a: online_users)
    {
        std:: cout << a.userid << " : " << a.username << std::endl;
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
    std::cerr << "For testing, you'll need to create a database melon for user melon on localhost with password melonpass" << std::endl;
    std::cerr << e.what() << std::endl;
}
