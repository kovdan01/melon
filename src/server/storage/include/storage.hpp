#ifndef MELON_SERVER_STORAGE_HPP_
#define MELON_SERVER_STORAGE_HPP_

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

#include "melondb.h"

namespace mysql = sqlpp::mysql;

const melon::Users g_Users;
const melon::Messages g_Messages;
const melon::Chats g_Chats;

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


namespace melon::server::storage
{

/* Init and connect to db */

std::shared_ptr<mysql::connection_config> config_melondb();

/* Users */

void add_user(mysql::connection& db, const User& user);
std::vector<std::string> get_online_users_names(mysql::connection& db);
std::vector<User> get_online_users(mysql::connection& db);
void make_user_online(mysql::connection& db, User& user);
void make_user_offline(mysql::connection& db, User& user);

/* Messages */

void add_message(mysql::connection& db, Message& message);

/* Chat */

void add_chat(mysql::connection& db, Chat chat);
std::vector<Message> get_messages_for_chat(mysql::connection& db, Chat& chat);


void hello();

}  // namespace melon::server::storage

#endif  // MELON_SERVER_STORAGE_HPP_
