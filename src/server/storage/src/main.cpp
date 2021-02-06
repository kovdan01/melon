#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

#include <iostream>
#include <array>

#include "melondb.h"

namespace mysql = sqlpp::mysql;


/* User:
*
* user_id (unsigned) - UNSIGNED INT   4 294 967 295 = uint_32  NOT NULL AUTOINCREMENT - PRIMARY KEY
* username (char[255])  - varchar(255) = std::string < 255 NOT NULL
* enum status (ONLINE = 1, OFFLINE = 0) - UNSIGNED TINYINT 255 = uint8_t NOT NULL DEFAULT 0
*/

struct User
{
    uint32_t userid;
    std::string username;
    enum status
    {
        OFFLINE,
        ONLINE,
    } stat;
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
    uint64_t messageid;
    std::string text;
    enum status
    {
        FAIL,
        SUCCES
    } stat;
    bool seen;
    std::chrono::high_resolution_clock::time_point timestamp;
    uint32_t user_id;
    uint32_t chat_id;
};

/* Chat:
*
* chat_id (unsigned) - UNSIGNED INT   4 294 967 295 = uint_32  NOT NULL AUTOINCREMENT - PRIMARY KEY
* chatname (char[255])  - varchar(255) = std::string < 255 NOT NULL
*/

struct Chat
{
    uint32_t chatid;
    std::string chatname;
};

const auto users = melon::Users{};
const auto messages = melon::Messages{};
const auto chats = melon::Chats{};

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
    for (const auto& row : db(select(users.username).from(users).where(users.status == 1)))
    {
        online_users_names.emplace_back(std::move(row.username));
    }
    return online_users_names;
}

static std::vector<User> get_online_users(mysql::connection& db)
{
    std::vector<User> online_users;
    for (const auto& row : db(select(all_of(users)).from(users).where(users.status == 1)))
    {
        User user;
        user.userid = row.userId;
        user.username = row.username;
        //user.status = row.status;
        online_users.emplace_back(std::move(user));
    }
    return online_users;
}

static void add_user(mysql::connection& db, User user)
{
    db(insert_into(users).set(users.username = user.username, users.status = 0));
}

static void make_user_online(mysql::connection& db, User user)
{
    db(update(users).set(users.status = 1).where(users.username == user.username));
}

static void make_user_offline(mysql::connection& db, User user)
{
    db(update(users).set(users.status = 0).where(users.username == user.username));
}

/* Messages */

static void add_message(mysql::connection& db, Message message)
{
    db(insert_into(messages).set(messages.text = message.text, messages.timesend = message.timestamp, messages.status = 0,
            messages.seen = 0, messages.userId = message.user_id, messages.chatId = message.chat_id));
}

/* Chat */

static void add_chat(mysql::connection& db, Chat chat)
{
    db(insert_into(chats).set(chats.chatname = chat.chatname));
}

static std::vector<Message> get_messages_for_chat(mysql::connection& db, Chat chat)
{
    std::vector<Message> messages_in_chat;
    for (const auto& row : db(select(all_of(messages)).from(messages).where(messages.chatId == chat.chatid)))
    {
        Message message;
        message.text = row.text;
        message.seen = row.seen;
        //user.status = row.status;
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



        User user;
        user.username = "h3ll0kitt1";
        add_user(db, user);

        Chat chat;
        chat.chatname = "secret_chat";
        add_chat(db, chat);

        Message message;
        message.text = "Let's protest";
        message.seen = 1;
        message.timestamp = std::chrono::system_clock::now();
        message.user_id = 1;
        message.chat_id = 1;
        add_message(db, message);


        std::vector<User> online_users = get_online_users(db);

        std::cout << "Online users: " << std::endl;
        for (auto& a: online_users)
        {
            std:: cout << a.userid << " : " << a.username << std::endl;
        }

        std::cout << "Change status for online" << std::endl;
        make_user_online(db, user);
        std::vector<std::string> onlineUsersNames = get_online_users_names(db);

        std::cout << "Online users: " << std::endl;
        for (const auto &a: onlineUsersNames)
        {
            std:: cout << a << std::endl;
        }

        std::cout << "Change status for offline" << std::endl;
        make_user_offline(db, user);
        onlineUsersNames = get_online_users_names(db);

        std::cout << "Online users: " << std::endl;
        for (auto& a: onlineUsersNames)
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
