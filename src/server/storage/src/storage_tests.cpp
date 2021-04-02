#include <catch2/catch.hpp>

#include <storage.hpp>

namespace mc = melon::core;
namespace mss = melon::server::storage;
namespace mysql = sqlpp::mysql;


TEST_CASE( "Test storage entities", "[somelabel]" )
{
    mysql::connection db(mss::config_db());

    SECTION( "Test domains" )
    {
        mss::Domain domain1(db, "Paul server", false);
        mss::Domain domain2(db, "Blaze server", false);

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

        SECTION( "Test users")
        {
            mss::User user1(db, "queen Ann", domain1.domain_id(), mc::User::Status::ONLINE);
            mss::User user2(db, "Erick", domain1.domain_id(), mc::User::Status::ONLINE);

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

            SECTION("Get online users")
            {
                 std::vector<mss::User> answer = mss::get_online_users(db);
                 REQUIRE(answer.size() == 2);
            }

            SECTION("Change status")
            {
                user2.set_status(mc::User::Status::OFFLINE);
                REQUIRE(user2.status() == mc::User::Status::OFFLINE);

                std::vector answer = mss::get_online_users(db);
                REQUIRE(answer.size() == 1);
            }

            domain1.remove();
            domain2.remove();
        }

    }



//    mss::Chat chat1(db, domain1.domain_id(), "secret_chat");
//    mss::Chat chat2(db, domain2.domain_id(), "On domain2");

//    SECTION( "Test Chat")
//    {
//        REQUIRE_NOTHROW(mss::check_if_chat_exists(db, chat1.chat_id(), chat1.domain_id()));
//        mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());
//        REQUIRE(chat1.chat_id() == found_chat1.chat_id());
//        REQUIRE(chat1.domain_id() == found_chat1.domain_id());
//        REQUIRE(chat1.chatname() == found_chat1.chatname());

//        SECTION("Change chatname")
//        {
//            found_chat1.set_chatname("new name");
//            REQUIRE(found_chat1.chatname() == "new name");
//        }

//        SECTION("Add users to chat")
//        {
//            found_chat1.add_user(user1);
//            found_chat1.add_user(user2);
//            std::vector<mss::User> users_in_chat = found_chat1.get_users();
//            //REQUIRE_THAT(users_in_chat, Catch::Matchers::Equals(std::vector<mss::User>{user1, user2}));
//        }
//    }

//    mss::Message message1(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(), ":D", std::chrono::system_clock::now(), mc::Message::Status::SENT);
//    mss::Message message2(db, chat2.chat_id(), chat2.domain_id(), user2.user_id(), user2.domain_id(), "Lorem ipsum", std::chrono::system_clock::now(), mc::Message::Status::SENT);
//    SECTION("Test Message")
//    {
//        mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());

//        SECTION("Change status")
//        {
//            found_message1.set_status(mc::Message::Status::SEEN);
//            REQUIRE(found_message1.status() == mc::Message::Status::SEEN);
//        }

//        SECTION("Change text")
//        {
//            found_message1.set_text("((((((");
//            REQUIRE(found_message1.text() == "((((((");
//        }
//    }


//    SECTION( "Test get_messages() for Chat")
//    {
//        mss::Chat found_chat1(db, chat1.chat_id(), chat1.domain_id());

//        mss::Message message1(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(), ":D", std::chrono::system_clock::now(), mc::Message::Status::SENT);
//        mss::Message message2(db, chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(), "Lorem ipsum", std::chrono::system_clock::now(), mc::Message::Status::SENT);
//        std::vector<mss::Message> messages_in_chat = found_chat1.get_messages();
//        //REQUIRE_THAT(messages_in_chat, Catch::Matchers::Equals(std::vector<mss::Message>{message1, message2}));
//    }

//    SECTION("Test deletions")
//    {
//        SECTION("Delete message")
//        {
//             mss::Message found_message1(db, message1.message_id(), message1.chat_id(), message1.domain_id_chat());
//             found_message1.remove();
//        }

//    }


}
