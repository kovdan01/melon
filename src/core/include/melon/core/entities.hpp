#ifndef MELON_CORE_ENTITIES_HPP_
#define MELON_CORE_ENTITIES_HPP_

#include <melon/core/export.h>

#include <boost/functional/hash.hpp>

#include <chrono>
#include <cstdint>
#include <functional>
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
    Domain(Domain&&) noexcept = default;
    Domain& operator=(Domain&&) noexcept = default;

    [[nodiscard]] id_t domain_id() const noexcept;
    [[nodiscard]] const std::string& hostname() const noexcept;
    [[nodiscard]] bool external() const noexcept;

protected:
    // Corresponding constructors in derived classes must
    // initialize all members that are not set here

    // For Insert
    Domain(std::string hostname, bool external);
    // For Select by hostname
    Domain(std::string hostname);
    // For Select by id
    Domain(id_t domain_id);

    void set_domain_id(id_t domain_id) noexcept;
    void set_external(bool external) noexcept;
    void set_hostname(std::string hostname) noexcept;

private:
    id_t m_domain_id = INVALID_ID;
    std::string m_hostname;
    bool m_external;
};

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
    User(User&&) noexcept = default;
    User& operator=(User&&) noexcept = default;

    [[nodiscard]] id_t user_id() const noexcept;
    [[nodiscard]] id_t domain_id() const noexcept;
    [[nodiscard]] const std::string& username() const noexcept;
    [[nodiscard]] Status status() const noexcept;

    void set_status(Status status) noexcept;

    // Corresponding constructors in derived classes must
    // initialize all members that are not set here

    // For Insert
    User(std::string username, id_t domain_id, Status status);
    // For Select by username and domain_id
    User(std::string username, id_t domain_id);
    // For Select by user_id and domain_id
    User(id_t user_id, id_t domain_id);

    bool friend operator==(User const&  lhs, User const& rhs) noexcept
    {
        return std::tuple(lhs.user_id(), lhs.domain_id(), lhs.username(), lhs.status()) == std::tuple(rhs.user_id(), rhs.domain_id(), rhs.username(), rhs.status());
    }

    void set_user_id(id_t user_id) noexcept;
    void set_domain_id(id_t domain_id) noexcept;
    void set_username(std::string username) noexcept;

private:
    id_t m_user_id = INVALID_ID;
    id_t m_domain_id = INVALID_ID;
    std::string m_username;
    Status m_status;
};

class MELON_CORE_EXPORT Chat
{
public:
    using Ptr = std::unique_ptr<Chat>;

    // After default construction the object must not be used
    // until its fields are initialized properly
    Chat() = default;

    Chat(const Chat&) = default;
    Chat& operator=(const Chat&) = default;
    Chat(Chat&&) noexcept = default;
    Chat& operator=(Chat&&) noexcept = default;

    [[nodiscard]] id_t chat_id() const noexcept;
    [[nodiscard]] id_t domain_id() const noexcept;
    [[nodiscard]] const std::string& chatname() const noexcept;

    void set_chatname(std::string chatname);

protected:
    // Corresponding constructors in derived classes must
    // initialize all members that are not set here

    // For Insert
    Chat(id_t domain_id, std::string chatname);
    // For Select
    Chat(id_t chat_id, id_t domain_id);

    void set_chat_id(id_t chat_id) noexcept;
    void set_domain_id(id_t domain_id) noexcept;

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
    Message(Message&&) noexcept = default;
    Message& operator=(Message&&) noexcept = default;

    [[nodiscard]] id_t message_id() const noexcept;
    [[nodiscard]] id_t chat_id() const noexcept;
    [[nodiscard]] id_t domain_id_chat() const noexcept;
    [[nodiscard]] id_t user_id() const noexcept;
    [[nodiscard]] id_t domain_id_user() const noexcept;
    [[nodiscard]] const std::string& text() const noexcept;
    [[nodiscard]] Status status() const noexcept;
    [[nodiscard]] timestamp_t timestamp() const;

    void set_text(std::string text);
    void set_status(Status status) noexcept;
    void set_timestamp(timestamp_t timestamp);

protected:
    // Corresponding constructors in derived classes must
    // initialize all members that are not set here

    // For Insert
    Message(id_t chat_id, id_t domain_id_chat, id_t user_id, id_t domain_id_user,
            std::string text, timestamp_t timestamp, Status status);
    // For Select
    Message(id_t message_id, id_t chat_id, id_t domain_id_chat);

    void set_message_id(id_t message_id) noexcept;
    void set_user_id(id_t user_id) noexcept;
    void set_chat_id(id_t chat_id) noexcept;
    void set_domain_id_chat(id_t domain_id_chat) noexcept;
    void set_domain_id_user(id_t domain_id_user) noexcept;

private:
    id_t m_message_id = INVALID_ID;
    id_t m_chat_id = INVALID_ID;
    id_t m_domain_id_chat = INVALID_ID;
    id_t m_user_id = INVALID_ID;
    id_t m_domain_id_user = INVALID_ID;
    std::string m_text;
    timestamp_t m_timestamp;
    Status m_status;
};

}  // namespace melon::core

template<> struct std::hash<melon::core::User>
{
    std::size_t operator()(melon::core::User const& u) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, u.username());
        boost::hash_combine(seed, u.user_id());
        return seed;
    }
};

#include "entities.ipp"

#endif  // MELON_CORE_ENTITIES_HPP_
