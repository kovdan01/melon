#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

#include <iostream>

#include "melondb.h"

namespace mysql = sqlpp::mysql;


namespace melon::core{

/* User:
*
* user_id (unsigned) - UNSIGNED INT   4 294 967 295 = uint_32  NOT NULL AUTOINCREMENT - PRIMARY KEY
* username (char[255])  - varchar(255) = std::string < 255 NOT NULL
* status (ONLINE = 1, OFFLINE = 0) - UNSIGNED TINYINT 255 = uint8_t NOT NULL DEFAULT 0 (enum is nor supported
* by sqlpp11)
*/

struct User
{
    std::uint32_t userid;
    std::string username;
    std::uint8_t status;
};

/* Message:
*
* message_id (unsigned) - UNSIGNED BIGINT  18 446 744 073 709 551 615 = uint64_t NOT NULL AUTOINCREMENT - PRIMARY KEY
* text (char[1024]) - varchar(1024) = std::string < 1024 NOT BULL
* status (SEND = 1, FAIL = 0) - UNSIGNED TINYINT 255 = unint8_t NOT NULL DEFAULT 0
* seen (SEEN = 1, NOTSEEN = 0) - BOOLEAN  NOT NULL DEFAULT 0
* time TIMESTAMP(14) NOT NULL
* user_id (unsigned) - UNSIGNED INT   4 294 967 295 = uint_32  NOT NULL
* chat_id (unsigned) - UNSIGNED INT   4 294 967 295 = uint_32  NOT NULL
* FOREIGHN KEYS:
* User (user_id)
* Chat (chat_id)
*/

struct Message
{
    std::uint64_t messageid;
    std::string text;
    std::uint8_t status;
    bool seen;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::uint32_t user_id;
    std::uint32_t chat_id;
};

/* Chat:
*
* chat_id (unsigned) - UNSIGNED INT   4 294 967 295 = uint_32  NOT NULL AUTOINCREMENT - PRIMARY KEY
* chatname (char[255])  - varchar(255) = std::string < 255 NOT NULL
*
*/

struct Chat
{
    std::uint32_t chatid;
    std::string chatname;
};

}

const melon::Users Users;
const melon::Messages Messages;
const melon::Chats chats;

static auto config_melondb()
{
    auto config = std::make_shared<mysql::connection_config>();
    config->user = "melon";
    config->database = "melon";
    config->host = "localhost";
    config->password = "melonpass";
    //config->debug = true;
    return config;
}

/* Users */

static std::vector<std::string> get_online_users_names(mysql::connection& db)
{
    std::vector<std::string> online_users_names;
    for (const auto& row : db(select(Users.username).from(Users).where(Users.status == 1)))
    {
        online_users_names.emplace_back(std::move(row.username));
    }
    return online_users_names;
}

static std::vector<melon::core::User> get_online_users(mysql::connection& db)
{
    std::vector<melon::core::User> online_users;
    for (const auto& row : db(select(all_of(Users)).from(Users).where(Users.status == 1)))
    {
        melon::core::User user;
        user.userid = row.userId;
        user.username = row.username;
        //user.status = row.status;
        online_users.emplace_back(std::move(user));
    }
    return online_users;
}

static void add_user(mysql::connection& db, melon::core::User user)
{
    db(insert_into(Users).set(Users.username = user.username, Users.status = 0));
}

static void make_user_online(mysql::connection& db, melon::core::User user)
{
    db(update(Users).set(Users.status = 1).where(Users.username == user.username));
}

static void make_user_offline(mysql::connection& db, melon::core::User user)
{
    db(update(Users).set(Users.status = 0).where(Users.username == user.username));
}

/* Messages */

static void add_message(mysql::connection& db, melon::core::Message message)
{
    db(insert_into(Messages).set(Messages.text = message.text, Messages.timesend = message.timestamp, Messages.status = 0,
            Messages.seen = 0, Messages.userId = message.user_id, Messages.chatId = message.chat_id));
}

/* Chat */

static void add_chat(mysql::connection& db, melon::core::Chat chat)
{
    db(insert_into(chats).set(chats.chatname = chat.chatname));
}

static std::vector<melon::core::Message> get_messages_for_chat(mysql::connection& db, melon::core::Chat chat)
{
    std::vector<melon::core::Message> messages_in_chat;
    for (const auto& row : db(select(all_of(Messages)).from(Messages).where(Messages.chatId == chat.chatid)))
    {
        melon::core::Message message;
        message.text = row.text;
        message.seen = row.seen;
        message.status = row.status;
        messages_in_chat.emplace_back(std::move(message));
    }
    return messages_in_chat;
}


int main()
{

    try
    {
        mysql::connection db(config_melondb());
        db.execute(R"(DROP TABLE IF EXISTS messages)");
        db.execute(R"(DROP TABLE IF EXISTS users)");
        db.execute(R"(DROP TABLE IF EXISTS chats)");

        db.execute(R"(CREATE TABLE users (
        user_id  INT UNSIGNED NOT NULL AUTO_INCREMENT,
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
            user_id INT UNSIGNED  NOT NULL,
            chat_id INT UNSIGNED  NOT NULL,
            FOREIGN KEY (chat_id)  REFERENCES chats (chat_id),
            FOREIGN KEY (user_id)  REFERENCES users (user_id),
            PRIMARY KEY (message_id)
            ))");


        //check that time in mariadb

        melon::core::User user;
        user.username = "h3ll0kitt1";
        add_user(db, user);

        melon::core::Chat chat;
        chat.chatid = 1;
        chat.chatname = "secret_chat";
        add_chat(db, chat);

        melon::core::Message message;
        message.text = "Let's protest";
        message.seen = 1;
        message.status = 1;
        message.timestamp = std::chrono::system_clock::now();
        message.user_id = 1;
        message.chat_id = 1;
        add_message(db, message);

        message.text = "or go to OVD";
        message.seen = 0;
        message.status = 0;
        message.timestamp = std::chrono::system_clock::now();
        message.user_id = 1;
        message.chat_id = 1;
        add_message(db, message);

        std::vector<melon::core::Message> chat_message = get_messages_for_chat(db, chat);

        std::cout << "Messages of chat: " << std::endl;
        for (const auto& a: chat_message)
        {
            std:: cout << "message id: " << a.messageid << "\ntext: " << a.text << std::endl;
        }


        std::vector<melon::core::User> online_users = get_online_users(db);

        std::cout << "Online users: " << std::endl;
        for (auto& a: online_users)
        {
            std:: cout << a.userid << " : " << a.username << std::endl;
        }

        std::cout << "Change status for online" << std::endl;
        make_user_online(db, user);
        std::vector<std::string> online_users_names = get_online_users_names(db);

        std::cout << "Online users: " << std::endl;
        for (const auto &a: online_users_names)
        {
            std:: cout << a << std::endl;
        }

        std::cout << "Change status for offline" << std::endl;
        make_user_offline(db, user);
        online_users_names = get_online_users_names(db);

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

}
