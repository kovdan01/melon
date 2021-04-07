#include <catch2/catch.hpp>

#include <algorithm>
#include <iostream>
#include <tuple>

#include <storage.hpp>

namespace mc = melon::core;
namespace mss = melon::server::storage;
namespace mysql = sqlpp::mysql;

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

static std::ostream& operator<<(std::ostream& o, const mc::Message::Status status)
{
    switch (status)
    {
    case mc::Message::Status::FAIL:
        o << "FAIL";
        break;
    case mc::Message::Status::SENT:
        o << "SENT";
        break;
    case mc::Message::Status::RECEIVED:
        o << "RECEIVED";
        break;
    case mc::Message::Status::SEEN:
        o << "SEEN";
        break;
    }
    return o;
}

static std::ostream& operator<<(std::ostream& o, const mc::Message& message)
{
    o << "Message { "
      << "message_id: " << message.message_id() << ", "
      << "chat_id: " << message.chat_id() << ", "
      << "domain_id_chat: " << message.domain_id_chat() << ", "
      << "user_id: " << message.user_id() << ", "
      << "domain_id_user: " << message.domain_id_user() << ", "
      << "text: \"" << message.text() << "\", "
      << "status: " << message.status() << " } ";
    return o;
}

static bool operator==(const mss::Message& lhs, const mss::Message& rhs)
{
    std::cerr << "\nM1: " << lhs << std::endl << "M2: " << rhs << std::endl << std::endl;
    return std::make_tuple(lhs.message_id(), lhs.chat_id(), lhs.domain_id_chat(), lhs.text(),
                           lhs.user_id(), lhs.domain_id_user(), lhs.status(), lhs.timestamp()) ==
           std::make_tuple(rhs.message_id(), rhs.chat_id(), rhs.domain_id_chat(), rhs.text(),
                           rhs.user_id(), rhs.domain_id_user(), rhs.status(), rhs.timestamp());
}

TEST_CASE("Test storage service", "[storage]")
{
    mysql::connection db(mss::config_db());
    db.execute(R"(SET autocommit=0)");

    SECTION("Test domains")
    {
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

        SECTION("Test users")
        {
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

            SECTION("Change status")
            {
                user2.set_status(mc::User::Status::OFFLINE);
                REQUIRE(user2.status() == mc::User::Status::OFFLINE);

                std::vector answer = mss::get_online_users(db);
                REQUIRE_FALSE(answer.empty());
            }

            SECTION("Test chats")
            {
                mss::Chat chat1(db, domain1.domain_id(), "secret_chat");
                mss::Chat chat2(db, domain2.domain_id(), "On domain2");

                SECTION("Select inserted chats")
                {
                    REQUIRE_NOTHROW(mss::check_if_chat_exists(db, chat1.chat_id(), chat1.domain_id()));
                    mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
                    REQUIRE(chat1 == found_chat1);
                }

                SECTION("Change chatname")
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

                SECTION("Test messages")
                {
                    mss::Message message1(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(),
                                          ":D", std::chrono::system_clock::now(), mc::Message::Status::SENT);
                    mss::Message message2(db, chat2.chat_id(), chat2.domain_id(), user2.user_id(), user2.domain_id(),
                                          "Lorem ipsum", std::chrono::system_clock::now(), mc::Message::Status::SENT);

                    SECTION("Select inserted messages")
                    {
                        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
                        REQUIRE(message1 == found_message1);
                    }

                    SECTION("Change status")
                    {
                        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
                        found_message1.set_status(mc::Message::Status::SEEN);
                        REQUIRE(found_message1.status() == mc::Message::Status::SEEN);
                    }

                    SECTION("Change text")
                    {
                        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
                        found_message1.set_text("((((((");
                        REQUIRE(found_message1.text() == "((((((");
                    }

                    SECTION("Test get_messages() for Chat")
                    {
                        mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
                        mss::Message message1(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(),
                                              ":D", std::chrono::system_clock::now(), mc::Message::Status::SENT);
                        mss::Message message2(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(),
                                              "Lorem ipsum", std::chrono::system_clock::now(), mc::Message::Status::SENT);
                        std::vector<mss::Message> messages_in_chat = found_chat1.get_messages();
                        REQUIRE(std::find_if(messages_in_chat.begin(), messages_in_chat.end(), [&message1](const mss::Message& m){ return m == message1; }) != messages_in_chat.end());
                        REQUIRE(std::find_if(messages_in_chat.begin(), messages_in_chat.end(), [&message2](const mss::Message& m){ return m == message2; }) != messages_in_chat.end());
                        REQUIRE(messages_in_chat.size() == 2);
                    }
                }
            }
        }
        db.execute(R"(ROLLBACK)");
        db.execute(R"(COMMIT)");
    }
}
