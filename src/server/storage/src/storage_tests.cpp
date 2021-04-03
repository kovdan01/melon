#include <catch2/catch.hpp>

#include <tuple>

#include <storage.hpp>

namespace mc = melon::core;
namespace mss = melon::server::storage;
namespace mysql = sqlpp::mysql;


bool operator == (const mss::User& lhs, const mss::User& rhs)
{
    return std::make_tuple(lhs.user_id(), lhs.username(), lhs.domain_id(), lhs.status()) == std::make_tuple(rhs.user_id(), rhs.username(), rhs.domain_id(), rhs.status());
}

bool operator == (const mss::Chat& lhs, const mss::Chat& rhs)
{
    return std::make_tuple(lhs.chat_id(), lhs.domain_id(), lhs.chatname()) == std::make_tuple(rhs.chat_id(), rhs.domain_id(), rhs.chatname());
}

bool operator == (const mss::Message& lhs, const mss::Message& rhs)
{
    return std::make_tuple(lhs.message_id(), lhs.chat_id(), lhs.domain_id_chat(),
                           lhs.user_id(), lhs.domain_id_user(), lhs.text(), std::chrono::system_clock::to_time_t(lhs.timestamp()), lhs.status()) ==
           std::make_tuple(rhs.message_id(), rhs.chat_id(), rhs.domain_id_chat(),
                           rhs.user_id(), rhs.domain_id_user(), rhs.text(), std::chrono::system_clock::to_time_t(rhs.timestamp()), rhs.status());
}

template <class T>
bool find(const std::vector<T>& vec, const T& obj)
{
    for (auto& a: vec)
    {
        if (a == obj)
            return true;
    }
    return false;
}

TEST_CASE( "Test storage service", "[storage]" )
{
    mysql::connection db(mss::config_db());
    db.execute(R"(SET autocommit=0)");

    SECTION( "Test domains" )
    {
        mss::Domain domain1(db, "Paul server", false);
        mss::Domain domain2(db, "Blaze server", false);

        SECTION("Select inserted domains")
        {
            REQUIRE_NOTHROW(mss::check_if_domain_exists(db, domain1.domain_id()));
            mss::Domain found_domain1(db, domain1.hostname());
            REQUIRE(domain1.domain_id() == found_domain1.domain_id());
            REQUIRE(domain1.hostname() == found_domain1.hostname());
            REQUIRE(domain1.external() == found_domain1.external());

            REQUIRE_NOTHROW(mss::check_if_domain_exists(db, domain2.domain_id()));
            mss::Domain found_domain2(db, domain2.hostname());
            REQUIRE(domain2.domain_id() == found_domain2.domain_id());
            REQUIRE(domain2.hostname() == found_domain2.hostname());
            REQUIRE(domain2.external() == found_domain2.external());
        }

        SECTION( "Test users")
        {
            mss::User user1(db, "Anna", domain1.domain_id(), mc::User::Status::ONLINE);
            mss::User user2(db, "Erick", domain1.domain_id(), mc::User::Status::ONLINE);

            SECTION("Select inserted users")
            {
                mss::User found_user1(db, user1.username(), user1.domain_id());
                REQUIRE(user1.user_id() == found_user1.user_id());
                REQUIRE(user1.username() == found_user1.username());
                REQUIRE(user1.domain_id() == found_user1.domain_id());
                REQUIRE(user1.status() == found_user1.status());

                mss::User found_user2(db, user2.username(), user2.domain_id());
                REQUIRE(user2.user_id() == found_user2.user_id());
                REQUIRE(user2.username() == found_user2.username());
                REQUIRE(user2.domain_id() == found_user2.domain_id());
                REQUIRE(user2.status() == found_user2.status());
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

            SECTION( "Test chats")
            {
                mss::Chat chat1(db, domain1.domain_id(), "secret_chat");
                mss::Chat chat2(db, domain2.domain_id(), "On domain2");

                SECTION("Select inserted chats")
                {
                    REQUIRE_NOTHROW(mss::check_if_chat_exists(db, chat1.chat_id(), chat1.domain_id()));
                    mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
                    REQUIRE(chat1.chat_id() == found_chat1.chat_id());
                    REQUIRE(chat1.domain_id() == found_chat1.domain_id());
                    REQUIRE(chat1.chatname() == found_chat1.chatname());
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
                    REQUIRE(find(users_in_chat, user1));
                    REQUIRE(find(users_in_chat, user2));
                    REQUIRE(users_in_chat.size() == 2);
                }

                SECTION("Test messages")
                {
                    mss::Message message1(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(), ":D", std::chrono::system_clock::now(), mc::Message::Status::SENT);
                    mss::Message message2(db, chat2.chat_id(), chat2.domain_id(), user2.user_id(), user2.domain_id(), "Lorem ipsum", std::chrono::system_clock::now(), mc::Message::Status::SENT);

                    SECTION("Select inserted messages")
                    {
                        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
                        REQUIRE(message1.message_id() == found_message1.message_id());
                        REQUIRE(message1.chat_id() == found_message1.chat_id());
                        REQUIRE(message1.domain_id_chat() == found_message1.domain_id_chat());
                        REQUIRE(message1.user_id() == found_message1.user_id());
                        REQUIRE(message1.domain_id_user() == found_message1.domain_id_user());
                        REQUIRE(message1.text() == found_message1.text());
                        std::time_t time1 = std::chrono::system_clock::to_time_t(message1.timestamp());
                        std::time_t time2 = std::chrono::system_clock::to_time_t(found_message1.timestamp());
                        REQUIRE(time1 == time2);
                        REQUIRE(message1.status() == found_message1.status());
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

                    SECTION( "Test get_messages() for Chat")
                    {
                        mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
                        mss::Message message1(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(), ":D", std::chrono::system_clock::now(), mc::Message::Status::SENT);
                        mss::Message message2(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(), "Lorem ipsum", std::chrono::system_clock::now(), mc::Message::Status::SENT);
                        std::vector<mss::Message> messages_in_chat = found_chat1.get_messages();
                        REQUIRE(find(messages_in_chat, message1));
                        REQUIRE(find(messages_in_chat, message2));
                    }
                }
            }
        }
        db.execute(R"(ROLLBACK)");
        db.execute(R"(COMMIT)");
    }
}
