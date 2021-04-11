#include <catch2/catch.hpp>

#include <algorithm>
#include <tuple>

#include <storage.hpp>

namespace mc = melon::core;
namespace mss = melon::server::storage;
namespace mysql = sqlpp::mysql;

namespace melon::server::storage
{

static bool operator==(const mss::Domain& lhs, const mss::Domain& rhs)
{
    return std::make_tuple(lhs.domain_id(), lhs.hostname(), lhs.external()) ==
           std::make_tuple(rhs.domain_id(), rhs.hostname(), rhs.external());
}

static bool operator==(const mss::User& lhs, const mss::User& rhs)
{
    return std::make_tuple(lhs.user_id(), lhs.username(), lhs.domain_id(), lhs.status()) ==
           std::make_tuple(rhs.user_id(), rhs.username(), rhs.domain_id(), rhs.status());
}

static bool operator==(const mss::Chat& lhs, const mss::Chat& rhs)
{
    return std::make_tuple(lhs.chat_id(), lhs.domain_id(), lhs.chatname()) ==
           std::make_tuple(rhs.chat_id(), rhs.domain_id(), rhs.chatname());
}

static bool operator==(const mss::Message& lhs, const mss::Message& rhs)
{
    return std::make_tuple(lhs.message_id(), lhs.chat_id(), lhs.domain_id_chat(), lhs.text(),
                           lhs.user_id(), lhs.domain_id_user(), lhs.status(),
                           std::chrono::system_clock::to_time_t(lhs.timestamp())) ==
           std::make_tuple(rhs.message_id(), rhs.chat_id(), rhs.domain_id_chat(), rhs.text(),
                           rhs.user_id(), rhs.domain_id_user(), rhs.status(),
                           std::chrono::system_clock::to_time_t(rhs.timestamp()));
}

}  // namespace melon::server::storage

TEST_CASE("Test domains", "[storage service]")
{
    mysql::connection db(mss::config_db());
    db.execute(R"(SET autocommit=0)");

    mss::Domain domain1(db, "Paul server", false);
    mss::Domain domain2(db, "Blaze server", false);

    SECTION("Select inserted domains")
    {
        REQUIRE_NOTHROW(mss::check_if_domain_exists(db, domain1.domain_id()));
        mss::Domain found_domain1(db, domain1.hostname());
        REQUIRE(domain1 == found_domain1);

        REQUIRE_NOTHROW(mss::check_if_domain_exists(db, domain2.domain_id()));
        mss::Domain found_domain2(db, domain2.hostname());
        REQUIRE(domain2 == found_domain2);
    }

    SECTION("Get hostname")
    {
        mss::Domain found_domain1(db, domain1.hostname());
        REQUIRE(found_domain1.hostname() == "Paul server");
    }

    SECTION("Get external")
    {
        mss::Domain found_domain1(db, domain1.hostname());
        REQUIRE(found_domain1.external() == false);
    }

    db.execute(R"(ROLLBACK)");
    db.execute(R"(COMMIT)");
}

TEST_CASE("Test users", "[storage service]")
{
    mysql::connection db(mss::config_db());
    db.execute(R"(SET autocommit=0)");

    mss::Domain domain1(db, "Paul server", false);
    mss::Domain domain2(db, "Blaze server", false);

    mss::User user1(db, "Anna", domain1.domain_id(), mc::User::Status::ONLINE);
    mss::User user2(db, "Erick", domain1.domain_id(), mc::User::Status::ONLINE);

    SECTION("Select inserted users")
    {
        mss::User found_user1(db, user1.username(), user1.domain_id());
        REQUIRE(user1 == found_user1);

        mss::User found_user2(db, user2.username(), user2.domain_id());
        REQUIRE(user2 == found_user2);
    }

    SECTION("Get online users")
    {
         std::vector<mss::User> answer = mss::get_online_users(db);
         REQUIRE_FALSE(answer.size() < 2);
    }

    SECTION("Get user_id")
    {
        REQUIRE(user1.user_id() == 1);
        REQUIRE(user2.user_id() == 2);
    }

    SECTION("Get username")
    {
        mss::User found_user1(db, user1.username(), user1.domain_id());
        REQUIRE(found_user1.username() == "Anna");

        mss::User found_user2(db, user2.username(), user2.domain_id());
        REQUIRE(found_user2.username() == "Erick");
    }

    SECTION("Get domain_id")
    {
        REQUIRE(user1.domain_id() == domain1.domain_id());
        REQUIRE(user2.domain_id() == domain1.domain_id());
    }

    SECTION("Get status")
    {
        mss::User found_user1(db, user1.username(), user1.domain_id());
        REQUIRE(found_user1.status() == mc::User::Status::ONLINE);

        mss::User found_user2(db, user2.username(), user2.domain_id());
        REQUIRE(found_user2.status() == mc::User::Status::ONLINE);
    }

    SECTION("Set status")
    {
        user2.set_status(mc::User::Status::OFFLINE);
        REQUIRE(user2.status() == mc::User::Status::OFFLINE);

        std::vector answer = mss::get_online_users(db);
        REQUIRE_FALSE(answer.empty());
    }

    db.execute(R"(ROLLBACK)");
    db.execute(R"(COMMIT)");
}

TEST_CASE("Test chats", "[storage service]")
{
    mysql::connection db(mss::config_db());
    db.execute(R"(SET autocommit=0)");

    mss::Domain domain1(db, "Paul server", false);
    mss::Domain domain2(db, "Blaze server", false);

    mss::User user1(db, "Anna", domain1.domain_id(), mc::User::Status::ONLINE);
    mss::User user2(db, "Erick", domain1.domain_id(), mc::User::Status::ONLINE);

    mss::Chat chat1(db, domain1.domain_id(), "secret_chat");
    mss::Chat chat2(db, domain2.domain_id(), "On domain2");

    SECTION("Select inserted chats")
    {
        REQUIRE_NOTHROW(mss::check_if_chat_exists(db, chat1.chat_id(), chat1.domain_id()));
        mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
        REQUIRE(chat1 == found_chat1);

        REQUIRE_NOTHROW(mss::check_if_chat_exists(db, chat2.chat_id(), chat2.domain_id()));
        mss::Chat found_chat2(db, chat2.chat_id(), chat2.domain_id());
        REQUIRE(chat2 == found_chat2);
    }

    SECTION("Get chat_id")
    {
        REQUIRE(chat1.chat_id() == 1);
        REQUIRE(chat2.chat_id() == 1);
    }

    SECTION("Get domain_id")
    {
        REQUIRE(chat1.domain_id() == domain1.domain_id());
        REQUIRE(chat2.domain_id() == domain2.domain_id());
    }

    SECTION("Get chatname")
    {
        mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
        REQUIRE(found_chat1.chatname() == "secret_chat");

        mss::Chat found_chat2(db, chat2.chat_id(), chat2.domain_id());
        REQUIRE(found_chat2.chatname() == "On domain2");
    }

    SECTION("Set chatname")
    {
        mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
        found_chat1.set_chatname("new name");
        REQUIRE(found_chat1.chatname() == "new name");
    }

    SECTION("Add users to chat")
    {
        mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
        found_chat1.add_user(user1);
        found_chat1.add_user(user2);
        std::vector<mss::User> users_in_chat = found_chat1.get_users();
        REQUIRE(std::find_if(users_in_chat.begin(), users_in_chat.end(), [&user1](const mss::User& u){ return u == user1; }) != users_in_chat.end());
        REQUIRE(std::find_if(users_in_chat.begin(), users_in_chat.end(), [&user2](const mss::User& u){ return u == user2; }) != users_in_chat.end());
        REQUIRE(users_in_chat.size() == 2);
    }

    db.execute(R"(ROLLBACK)");
    db.execute(R"(COMMIT)");
}

TEST_CASE("Test messages", "[storage service]")
{
    mysql::connection db(mss::config_db());
    db.execute(R"(SET autocommit=0)");

    mss::Domain domain1(db, "Paul server", false);
    mss::Domain domain2(db, "Blaze server", false);

    mss::User user1(db, "Anna", domain1.domain_id(), mc::User::Status::ONLINE);
    mss::User user2(db, "Erick", domain1.domain_id(), mc::User::Status::ONLINE);

    mss::Chat chat1(db, domain1.domain_id(), "secret_chat");
    mss::Chat chat2(db, domain2.domain_id(), "On domain2");

    mss::Message message1(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(),
                          ":D", std::chrono::system_clock::now(), mc::Message::Status::SENT);
    mss::Message message2(db, chat2.chat_id(), chat2.domain_id(), user2.user_id(), user2.domain_id(),
                          "Lorem ipsum", std::chrono::system_clock::now(), mc::Message::Status::SENT);

    SECTION("Select inserted messages")
    {
        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
        REQUIRE(message1 == found_message1);
    }

    SECTION("Get message_id")
    {
        REQUIRE(message1.message_id() == 1);
        REQUIRE(message2.message_id() == 1);
    }

    SECTION("Get chat_id")
    {
        REQUIRE(message1.chat_id() == chat1.chat_id());
        REQUIRE(message2.chat_id() == chat2.chat_id());
    }

    SECTION("Get domain_id_chat")
    {
        REQUIRE(message1.domain_id_chat() == chat1.domain_id());
        REQUIRE(message2.domain_id_chat() == chat2.domain_id());
    }

    SECTION("Get user_id")
    {
        REQUIRE(message1.user_id() == user2.user_id());
        REQUIRE(message2.user_id() == user2.user_id());
    }

    SECTION("Get domain_id_user")
    {
        REQUIRE(message1.domain_id_user() == user2.domain_id());
        REQUIRE(message2.domain_id_user() == user2.domain_id());
    }

    SECTION("Get status")
    {
        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
        REQUIRE(found_message1.status() == mc::Message::Status::SENT);
    }

    SECTION("Get text")
    {
        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
        REQUIRE(found_message1.text() == ":D");
    }

    SECTION("Get timestamp")
    {
        std::chrono::system_clock::time_point test_time_point = std::chrono::system_clock::now();
        mss::Message message3(db, chat1.chat_id(), chat1.domain_id(), user1.user_id(), user1.domain_id(),
                              "hello", test_time_point, mc::Message::Status::SENT);
        mss::Message found_message3(db, message3.message_id(), message3.chat_id(), message3.domain_id_chat());
        REQUIRE(std::chrono::system_clock::to_time_t(found_message3.timestamp()) ==
                std::chrono::system_clock::to_time_t(test_time_point));
    }

    SECTION("Set status")
    {
        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
        found_message1.set_status(mc::Message::Status::SEEN);
        REQUIRE(found_message1.status() == mc::Message::Status::SEEN);
    }

    SECTION("Set text")
    {
        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
        found_message1.set_text("((((((");
        REQUIRE(found_message1.text() == "((((((");
    }

    SECTION("Set timestamp")
    {
        std::chrono::system_clock::time_point test_time_point = std::chrono::system_clock::now();
        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
        found_message1.set_timestamp(test_time_point);
        REQUIRE(std::chrono::system_clock::to_time_t(found_message1.timestamp()) ==
                std::chrono::system_clock::to_time_t(test_time_point));
    }

    SECTION("Test get_messages() for Chat")
    {
        mss::Message message4(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(),
                              "Sample text", std::chrono::system_clock::now(), mc::Message::Status::SENT);
        std::vector<mss::Message> messages_in_chat = chat1.get_messages();
        REQUIRE(std::find_if(messages_in_chat.begin(), messages_in_chat.end(), [&message1](const mss::Message& m){ return m == message1; }) != messages_in_chat.end());
        REQUIRE(std::find_if(messages_in_chat.begin(), messages_in_chat.end(), [&message4](const mss::Message& m){ return m == message4; }) != messages_in_chat.end());
        REQUIRE(messages_in_chat.size() == 2);
    }

    db.execute(R"(ROLLBACK)");
    db.execute(R"(COMMIT)");
}
