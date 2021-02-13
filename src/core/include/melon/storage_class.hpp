#ifndef MELON_CORE_MELON_STORAGE_CLASS_HPP_
#define MELON_CORE_MELON_STORAGE_CLASS_HPP_

#include <melon/core/export.h>

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

class MELON_CORE_EXPORT User
{
public:
    enum class Status: std::uint8_t
    {
        OFFLINE, ONLINE, CHILL
    };
    User(std::uint64_t userid, std::string username, Status status);
    User(std::string username);
    ~User() = default;
    [[nodiscard]] std::uint64_t userid() const;
    [[nodiscard]] const std::string username() const;
    [[nodiscard]] Status status() const;


private:
    std::uint64_t m_userid;
    std::string m_username;
    Status m_status;
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

class MELON_CORE_EXPORT Message
{
public:
    enum class Status: std::uint8_t
    {
        FAIL, SENT, RECIEVED
    };
    Message(std::uint64_t messageid, std::string text, Status status, bool seen,
            std::uint64_t user_id, std::uint32_t chat_id);
    Message(std::string text, Status status, bool seen, std::uint64_t user_id, std::uint32_t chat_id);
    ~Message() = default;
    [[nodiscard]] std::uint64_t messageid() const;
    [[nodiscard]] const std::string text() const;
    [[nodiscard]] Status status() const;
    [[nodiscard]] bool seen() const;
    [[nodiscard]] const std::chrono::high_resolution_clock::time_point timestamp() const;
    [[nodiscard]] std::uint64_t user_id() const;
    [[nodiscard]] std::uint32_t chat_id() const;

private:
    std::uint64_t m_messageid;
    std::string m_text;
    Status m_status;
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

class MELON_CORE_EXPORT Chat
{
public:
    Chat(std::uint32_t chatid, std::string chatname);
    Chat(std::string chatname);
    ~Chat() = default;
    [[nodiscard]] std::uint32_t chatid() const;
    [[nodiscard]] const std::string chatname() const;

private:
    std::uint32_t m_chatid;
    std::string m_chatname;
};


}  // namespace melon::core

#endif  // MELON_CORE_MELON_STORAGE_CLASS_HPP_
