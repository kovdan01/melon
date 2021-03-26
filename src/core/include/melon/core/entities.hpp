#ifndef MELON_CORE_ENTITIES_HPP_
#define MELON_CORE_ENTITIES_HPP_

#include <melon/core/export.h>

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace melon::core
{

using id_t = std::uint64_t;
inline constexpr id_t INVALID_ID = 0;

class MELON_CORE_EXPORT Domain
{
public:
    using Ptr = std::unique_ptr<Domain>;

    // After default construction the object must not be used
    // until its fields are initialized properly
    Domain() = default;

    Domain(const Domain&) = default;
    Domain& operator=(const Domain&) = default;
    Domain(Domain&&) = default;
    Domain& operator=(Domain&&) = default;

    virtual ~Domain();

    [[nodiscard]] id_t domain_id() const;
    [[nodiscard]] const std::string& hostname() const;
    [[nodiscard]] bool external() const;

protected:
    // Corresponding constructors in derived classes must
    // initialize all members that are not set here

    // For Insert
    Domain(std::string hostname, bool external);
    // For Select
    Domain(std::string hostname);

    void set_domain_id(id_t domain_id);

    void set_external(bool external);
    void set_external_base(bool external);
    virtual void set_external_impl();

private:
    id_t m_domain_id = INVALID_ID;
    std::string m_hostname;
    bool m_external;
};

class Chat;

class MELON_CORE_EXPORT User
{
public:
    using Ptr = std::unique_ptr<User>;

    enum class Status
    {
        OFFLINE,
        ONLINE,
        CHILL,
    };

    // After default construction the object must not be used
    // until its fields are initialized properly
    User() = default;

    User(const User&) = default;
    User& operator=(const User&) = default;
    User(User&&) = default;
    User& operator=(User&&) = default;

    virtual ~User();

    [[nodiscard]] id_t user_id() const;
    [[nodiscard]] id_t domain_id() const;
    [[nodiscard]] const std::string& username() const;
    [[nodiscard]] Status status() const;

    void set_status(Status status);

    [[nodiscard]] virtual std::vector<std::unique_ptr<Chat>> get_chats() const = 0;

protected:
    // Corresponding constructors in derived classes must
    // initialize all members that are not set here

    // For Insert
    User(std::string username, id_t domain_id, Status status);
    // For Select
    User(std::string username, id_t domain_id);

    void set_status_base(Status status);
    virtual void set_status_impl();

    void set_user_id(id_t user_id);
    void set_domain_id(id_t domain_id);

private:
    id_t m_user_id = INVALID_ID;
    id_t m_domain_id = INVALID_ID;
    std::string m_username;
    Status m_status;
};

class Message;

class MELON_CORE_EXPORT Chat
{
public:
    using Ptr = std::unique_ptr<Chat>;

    // After default construction the object must not be used
    // until its fields are initialized properly
    Chat() = default;

    Chat(const Chat&) = default;
    Chat& operator=(const Chat&) = default;
    Chat(Chat&&) = default;
    Chat& operator=(Chat&&) = default;

    virtual ~Chat();

    [[nodiscard]] id_t chat_id() const;
    [[nodiscard]] id_t domain_id() const;
    [[nodiscard]] const std::string& chatname() const;

    void set_chatname(std::string chatname);

    [[nodiscard]] virtual std::vector<std::unique_ptr<User>> get_users() const = 0;
    [[nodiscard]] virtual std::vector<std::unique_ptr<Message>> get_messages() const = 0;

protected:
    // Corresponding constructors in derived classes must
    // initialize all members that are not set here

    // For Insert
    Chat(id_t domain_id, std::string chatname);
    // For Select
    Chat(id_t chat_id, id_t domain_id);

    virtual void set_chatname_base(std::string chatname);
    virtual void set_chatname_impl();

    void set_chat_id(id_t chat_id);
    void set_domain_id(id_t domain_id);

private:
    id_t m_chat_id = INVALID_ID;
    id_t m_domain_id = INVALID_ID;
    std::string m_chatname;
};

class MELON_CORE_EXPORT Message
{
public:
    using Ptr = std::unique_ptr<Message>;

    enum class Status
    {
        FAIL,
        SENT,
        RECEIVED,
        SEEN,
    };

    using timestamp_t = std::chrono::system_clock::time_point;

    // After default construction the object must not be used
    // until its fields are initialized properly
    Message() = default;

    Message(const Message&) = default;
    Message& operator=(const Message&) = default;
    Message(Message&&) = default;
    Message& operator=(Message&&) = default;

    virtual ~Message();

    [[nodiscard]] id_t message_id() const;
    [[nodiscard]] id_t domain_id() const;
    [[nodiscard]] const std::string& text() const;
    [[nodiscard]] Status status() const;
    [[nodiscard]] timestamp_t timestamp() const;
    [[nodiscard]] id_t user_id() const;
    [[nodiscard]] id_t chat_id() const;

    void set_text(std::string text);
    void set_status(Status status);
    void set_timestamp(timestamp_t timestamp);

protected:
    // Corresponding constructors in derived classes must
    // initialize all members that are not set here

    // For Insert
    Message(id_t chat_id, id_t domain_id, id_t user_id,
            std::string text, timestamp_t timestamp, Status status);
    // For Select
    Message(id_t message_id, id_t chat_id, id_t domain_id);

    virtual void set_text_base(std::string text);
    virtual void set_text_impl();

    void set_status_base(Status status);
    virtual void set_status_impl();

    void set_timestamp_base(timestamp_t timestamp);
    virtual void set_timestamp_impl();

    void set_message_id(id_t message_id);
    void set_user_id(id_t user_id);
    void set_chat_id(id_t chat_id);
    void set_domain_id(id_t domain_id);

private:
    id_t m_message_id = INVALID_ID;
    id_t m_chat_id = INVALID_ID;
    id_t m_domain_id = INVALID_ID;
    id_t m_user_id = INVALID_ID;
    std::string m_text;
    timestamp_t m_timestamp;
    Status m_status;
};

}  // namespace melon::core

#include "entities.ipp"

#endif  // MELON_CORE_ENTITIES_HPP_
