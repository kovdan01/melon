#ifndef MELON_CORE_STORAGE_CLASS_HPP_
#define MELON_CORE_STORAGE_CLASS_HPP_

#include <melon/core/export.h>

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace melon::core
{

inline constexpr std::uint64_t INVALID_ID = std::uint64_t(-1);

class MELON_CORE_EXPORT Domain
{
public:
    using Ptr = std::unique_ptr<Domain>;

    // After default construction the object must not be used
    // until its fields are initialized properly
    Domain() = default;

    // For Insert
    Domain(std::string hostname, bool external);
    // For Select
    Domain(std::string hostname);

    virtual ~Domain();

    [[nodiscard]] std::uint64_t domain_id() const;
    [[nodiscard]] const std::string& hostname() const;
    [[nodiscard]] bool external() const;

protected:
    void set_domain_id(std::uint64_t domain_id);

    void set_external(bool external);
    void set_external_base(bool external);
    virtual void set_external_impl(bool external);

private:
    std::uint64_t m_domain_id = INVALID_ID;
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

    // For Insert
    User(std::string username, std::uint64_t domain_id, Status status);
    // For Select
    User(std::string username, std::uint64_t domain_id);

    virtual ~User();

    [[nodiscard]] std::uint64_t user_id() const;
    [[nodiscard]] std::uint64_t domain_id() const;
    [[nodiscard]] const std::string& username() const;
    [[nodiscard]] Status status() const;

    void set_status(Status status);

    virtual std::vector<std::unique_ptr<Chat>> get_chats() const = 0;

protected:
    void set_status_base(Status status);
    virtual void set_status_impl(Status status);

    void set_user_id(std::uint64_t user_id);
    void set_domain_id(std::uint64_t domain_id);

private:
    std::uint64_t m_user_id = INVALID_ID;
    std::uint64_t m_domain_id = INVALID_ID;
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

    // For Insert
    Chat(std::uint64_t domain_id, std::string chatname);
    // For Select
    Chat(std::uint64_t chat_id, std::uint64_t domain_id);

    virtual ~Chat();

    [[nodiscard]] std::uint64_t chat_id() const;
    [[nodiscard]] std::uint64_t domain_id() const;
    [[nodiscard]] const std::string& chatname() const;

    void set_chatname(std::string chatname);

    virtual std::vector<std::unique_ptr<User>> get_users() const = 0;
    virtual std::vector<std::unique_ptr<Message>> get_messages() const = 0;

protected:
    void set_chatname_base(std::string chatname);
    virtual void set_chatname_impl(const std::string& chatname);

    void set_chat_id(std::uint64_t chat_id);
    void set_domain_id(std::uint64_t domain_id);

private:
    std::uint64_t m_chat_id = INVALID_ID;
    std::uint64_t m_domain_id = INVALID_ID;
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

    // For Insert
    Message(std::uint64_t user_id, std::uint64_t chat_id, std::uint64_t domain_id,
            timestamp_t timestamp, std::string text, Status status);
    // For Select
    Message(std::uint64_t message_id, std::uint64_t chat_id, std::uint64_t domain_id);

    virtual ~Message();

    [[nodiscard]] std::uint64_t message_id() const;
    [[nodiscard]] std::uint64_t domain_id() const;
    [[nodiscard]] const std::string& text() const;
    [[nodiscard]] Status status() const;
    [[nodiscard]] timestamp_t timestamp() const;
    [[nodiscard]] std::uint64_t user_id() const;
    [[nodiscard]] std::uint64_t chat_id() const;

    void set_text(std::string text);
    void set_status(Status status);
    void set_timestamp(timestamp_t timestamp);

protected:
    void set_text_base(std::string text);
    virtual void set_text_impl(const std::string& text);

    void set_status_base(Status status);
    virtual void set_status_impl(Status status);

    void set_timestamp_base(timestamp_t timestamp);
    virtual void set_timestamp_impl(timestamp_t timestamp);

    void set_message_id(std::uint64_t message_id);
    void set_user_id(std::uint64_t user_id);
    void set_chat_id(std::uint64_t chat_id);
    void set_domain_id(std::uint64_t domain_id);

private:
    std::uint64_t m_message_id = INVALID_ID;
    std::uint64_t m_user_id = INVALID_ID;
    std::uint64_t m_chat_id = INVALID_ID;
    std::uint64_t m_domain_id = INVALID_ID;
    timestamp_t m_timestamp;
    std::string m_text;
    Status m_status;
};

}  // namespace melon::core

#include "storage_class.ipp"

#endif  // MELON_CORE_STORAGE_CLASS_HPP_
