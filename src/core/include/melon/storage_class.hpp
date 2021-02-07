#ifndef MELON_CORE_MELON_STORAGE_CLASS_HPP_
#define MELON_CORE_MELON_STORAGE_CLASS_HPP_

#include <iostream>
#include <chrono>

namespace melon::core
{


/* User:
*
* user_id (unsigned) - UNSIGNED INT   4 294 967 295 = uint_32  NOT NULL AUTOINCREMENT - PRIMARY KEY
* username (char[255])  - varchar(255) = std::string < 255 NOT NULL
* status (ONLINE = 1, OFFLINE = 0) - UNSIGNED TINYINT 255 = uint8_t NOT NULL DEFAULT 0 (enum is nor supported
* by sqlpp11)
*/

class User
{
public:
    User(std::uint64_t userid, std::string username, std::uint8_t status);
    User(std::string username);
    ~User(){}
    [[nodiscard]] std::uint64_t userid();
    [[nodiscard]] const std::string username() const;
    [[nodiscard]] std::uint8_t status();

private:
    std::uint64_t m_userid;
    std::string m_username;
    std::uint8_t m_status;
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

class Message
{
public:
    Message(std::uint64_t messageid, std::string text, std::uint8_t status, bool seen,
            std::uint64_t user_id, std::uint32_t chat_id);
    Message(std::string text, std::uint8_t status, bool seen, std::uint64_t user_id, std::uint32_t chat_id);
    ~Message(){}
    [[nodiscard]] std::uint64_t messageid();
    [[nodiscard]] const std::string text() const;
    [[nodiscard]] std::uint8_t status();
    [[nodiscard]] bool seen();
    [[nodiscard]] const std::chrono::high_resolution_clock::time_point timestamp() const;
    [[nodiscard]] const std::uint64_t user_id() const;
    [[nodiscard]] const std::uint32_t chat_id() const;

private:
    std::uint64_t m_messageid;
    std::string m_text;
    std::uint8_t m_status;
    bool m_seen;
    std::chrono::high_resolution_clock::time_point m_timestamp;
    std::uint64_t m_user_id;
    std::uint32_t m_chat_id;
};

/* Chat:
*
* chat_id (unsigned) - UNSIGNED INT   4 294 967 295 = uint32t  NOT NULL AUTOINCREMENT - PRIMARY KEY
* chatname (char[255])  - varchar(255) = std::string < 255 NOT NULL
*
*/

class Chat
{
public:
    Chat(std::uint32_t chatid, std::string chatname);
    Chat(std::string chatname);
    ~Chat(){}
    [[nodiscard]] const std::uint32_t chatid() const;
    [[nodiscard]] const std::string chatname() const;

private:
    std::uint32_t m_chatid;
    std::string m_chatname;
};


}  // namespace melon::core

#endif  // MELON_CORE_MELON_STORAGE_CLASS_HPP_
