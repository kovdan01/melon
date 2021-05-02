#include <entities.hpp>
#include <melon/core/log_configuration.hpp>

#include <catch2/catch.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <algorithm>
#include <tuple>

#include <iostream>

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
                           lhs.user_id(), lhs.domain_id_user(), lhs.status(), lhs.timestamp()) ==
           std::make_tuple(rhs.message_id(), rhs.chat_id(), rhs.domain_id_chat(), rhs.text(),
                           rhs.user_id(), rhs.domain_id_user(), rhs.status(), rhs.timestamp());
}

}  // namespace melon::server::storage

namespace
{

class DBTestRAIIWrapper
{
public:
    DBTestRAIIWrapper()
    {
        m_conn.execute(R"(SET autocommit=0)");
    }

    ~DBTestRAIIWrapper()
    {
        try
        {
            m_conn.execute(R"(ROLLBACK)");
            m_conn.execute(R"(COMMIT)");
        }
        catch (...)
        {
            BOOST_LOG_TRIVIAL(fatal) << "Failed to ROLLBACK and COMMIT\n";
        }
    }

    mysql::connection& conn() { return m_conn; }

private:
    mysql::connection m_conn{mss::config_db()};
};

}  // namespace

TEST_CASE("Test domains", "[storage][domains]")
{
    DBTestRAIIWrapper db;
    mss::Domain domain1(db.conn(), "Paul server", false);
    mss::Domain domain2(db.conn(), "Blaze server", false);

    SECTION("Select inserted domains")
    {
        REQUIRE_THROWS_AS(mss::check_if_domain_exists(db.conn(), mc::INVALID_ID), mss::IdNotFoundException);
        REQUIRE_THROWS_AS(mss::check_if_domain_exists(db.conn(), "not_valid_hostname"), mss::IdNotFoundException);

        REQUIRE_NOTHROW(mss::check_if_domain_exists(db.conn(), domain1.hostname()));
        {
            mss::Domain found_domain1(db.conn(), domain1.hostname());
            REQUIRE(domain1 == found_domain1);
        }
        REQUIRE_NOTHROW(mss::check_if_domain_exists(db.conn(), domain1.domain_id()));
        {
            mss::Domain found_domain1(db.conn(), domain1.domain_id());
            REQUIRE(domain1 == found_domain1);
        }

        REQUIRE_NOTHROW(mss::check_if_domain_exists(db.conn(), domain2.hostname()));
        {
            mss::Domain found_domain2(db.conn(), domain2.hostname());
            REQUIRE(domain2 == found_domain2);
        }
        REQUIRE_NOTHROW(mss::check_if_domain_exists(db.conn(), domain2.domain_id()));
        {
            mss::Domain found_domain2(db.conn(), domain2.domain_id());
            REQUIRE(domain2 == found_domain2);
        }

        REQUIRE_THROWS_AS((mss::Domain{db.conn(), "not_valid_hostname"}), mss::IdNotFoundException);
        REQUIRE_THROWS_AS((mss::Domain{db.conn(), mc::INVALID_ID}), mss::IdNotFoundException);
    }

    SECTION("Getters")
    {
        REQUIRE(domain1.hostname() == "Paul server");
        REQUIRE(domain2.hostname() == "Blaze server");
        REQUIRE(domain1.external() == false);
        REQUIRE(domain2.external() == false);
    }

    SECTION("Remove")
    {
        REQUIRE_NOTHROW(mss::Domain{db.conn(), domain1.domain_id()});
        domain1.remove();
        REQUIRE_THROWS_AS((mss::Domain{db.conn(), domain1.domain_id()}), mss::IdNotFoundException);

        REQUIRE_NOTHROW(mss::Domain{db.conn(), domain2.domain_id()});
        domain2.remove();
        REQUIRE_THROWS_AS((mss::Domain{db.conn(), domain2.domain_id()}), mss::IdNotFoundException);
    }
}

TEST_CASE("Test users", "[storage][users]")
{
    DBTestRAIIWrapper db;
    mss::Domain domain1(db.conn(), "Paul server", false);
    mss::Domain domain2(db.conn(), "Blaze server", false);

    mss::User user1(db.conn(), "Anna", domain2.domain_id(), mc::User::Status::ONLINE);
    mss::User user2(db.conn(), "Erick", domain1.domain_id(), mc::User::Status::OFFLINE);

    SECTION("Select inserted users")
    {
        {
            mss::User found_user1(db.conn(), user1.username(), user1.domain_id());
            REQUIRE(user1 == found_user1);
        }
        {
            mss::User found_user1(db.conn(), user1.user_id(), user1.domain_id());
            REQUIRE(user1 == found_user1);
        }
        {
            mss::User found_user2(db.conn(), user2.username(), user2.domain_id());
            REQUIRE(user2 == found_user2);
        }
        {
            mss::User found_user2(db.conn(), user2.user_id(), user2.domain_id());
            REQUIRE(user2 == found_user2);
        }

        REQUIRE_THROWS_AS((mss::User{db.conn(), "Not_valid_username", user1.domain_id()}), mss::IdNotFoundException);
        REQUIRE_THROWS_AS((mss::User{db.conn(), "Anna", mc::INVALID_ID}), mss::IdNotFoundException);
        REQUIRE_THROWS_AS((mss::User{db.conn(), "Not_valid_username", mc::INVALID_ID}), mss::IdNotFoundException);
    }

    SECTION("Getters")
    {
        REQUIRE(user1.user_id() == 1);
        REQUIRE(user2.user_id() == 1);
        REQUIRE(user1.domain_id() == domain2.domain_id());
        REQUIRE(user2.domain_id() == domain1.domain_id());
        REQUIRE(user1.username() == "Anna");
        REQUIRE(user2.username() == "Erick");
        REQUIRE(user1.status() == mc::User::Status::ONLINE);
        REQUIRE(user2.status() == mc::User::Status::OFFLINE);

        std::vector<mss::User> online_users = mss::get_online_users(db.conn());
        REQUIRE(online_users.size() == 1);
        REQUIRE(online_users[0] == user1);
    }

    SECTION("Setters")
    {
        user1.set_status(mc::User::Status::OFFLINE);
        REQUIRE(user1.status() == mc::User::Status::OFFLINE);

        std::vector online_users = mss::get_online_users(db.conn());
        REQUIRE(online_users.empty());
    }

    SECTION("Remove")
    {
        REQUIRE_NOTHROW(mss::User{db.conn(), user1.user_id(), user1.domain_id()});
        user1.remove();
        REQUIRE_THROWS_AS((mss::User{db.conn(), user1.user_id(), user1.domain_id()}), mss::IdNotFoundException);

        REQUIRE_NOTHROW(mss::User{db.conn(), user2.user_id(), user2.domain_id()});
        user2.remove();
        REQUIRE_THROWS_AS((mss::User{db.conn(), user2.user_id(), user2.domain_id()}), mss::IdNotFoundException);
    }
}

TEST_CASE("Test chats", "[storage][chats]")
{
    DBTestRAIIWrapper db;
    mss::Domain domain1(db.conn(), "Paul server", false);
    mss::Domain domain2(db.conn(), "Blaze server", false);

    mss::User user1(db.conn(), "Anna", domain2.domain_id(), mc::User::Status::ONLINE);
    mss::User user2(db.conn(), "Erick", domain1.domain_id(), mc::User::Status::OFFLINE);

    mss::Chat chat1(db.conn(), domain1.domain_id(), "secret_chat");
    mss::Chat chat2(db.conn(), domain2.domain_id(), "On domain2");

    SECTION("Select inserted chats")
    {
        REQUIRE_THROWS_AS(mss::check_if_chat_exists(db.conn(), mc::INVALID_ID, chat1.domain_id()), mss::IdNotFoundException);
        REQUIRE_THROWS_AS(mss::check_if_chat_exists(db.conn(), chat1.chat_id(), mc::INVALID_ID), mss::IdNotFoundException);
        REQUIRE_THROWS_AS(mss::check_if_chat_exists(db.conn(), mc::INVALID_ID, mc::INVALID_ID), mss::IdNotFoundException);

        REQUIRE_THROWS_AS((mss::Chat{db.conn(), mc::INVALID_ID, chat1.domain_id()}), mss::IdNotFoundException);
        REQUIRE_THROWS_AS((mss::Chat{db.conn(), chat1.chat_id(), mc::INVALID_ID}), mss::IdNotFoundException);
        REQUIRE_THROWS_AS((mss::Chat{db.conn(), mc::INVALID_ID, mc::INVALID_ID}), mss::IdNotFoundException);

        REQUIRE_NOTHROW(mss::check_if_chat_exists(db.conn(), chat1.chat_id(), chat1.domain_id()));
        mss::Chat found_chat1(db.conn(), chat1.chat_id(), chat1.domain_id());
        REQUIRE(chat1 == found_chat1);

        REQUIRE_NOTHROW(mss::check_if_chat_exists(db.conn(), chat2.chat_id(), chat2.domain_id()));
        mss::Chat found_chat2(db.conn(), chat2.chat_id(), chat2.domain_id());
        REQUIRE(chat2 == found_chat2);
    }

    SECTION("Getters")
    {
        REQUIRE(chat1.chat_id() == 1);
        REQUIRE(chat2.chat_id() == 1);
        REQUIRE(chat1.domain_id() == domain1.domain_id());
        REQUIRE(chat2.domain_id() == domain2.domain_id());
        REQUIRE(chat1.chatname() == "secret_chat");
        REQUIRE(chat2.chatname() == "On domain2");
    }

    SECTION("Setters")
    {
        chat1.set_chatname("new name");
        REQUIRE(chat1.chatname() == "new name");
    }

    SECTION("Add users to chat")
    {
        chat1.add_user(user1);
        chat1.add_user(user2);
        std::vector<mss::User> users_in_chat = chat1.get_users();
        REQUIRE(std::find_if(users_in_chat.begin(), users_in_chat.end(), [&user1](const mss::User& u){ return u == user1; }) != users_in_chat.end());
        REQUIRE(std::find_if(users_in_chat.begin(), users_in_chat.end(), [&user2](const mss::User& u){ return u == user2; }) != users_in_chat.end());
        REQUIRE(users_in_chat.size() == 2);
    }

    SECTION("Remove")
    {
        REQUIRE_NOTHROW(mss::Chat{db.conn(), chat1.chat_id(), chat1.domain_id()});
        chat1.remove();
        REQUIRE_THROWS_AS((mss::Chat{db.conn(), chat1.chat_id(), chat1.domain_id()}), mss::IdNotFoundException);

        REQUIRE_NOTHROW(mss::Chat{db.conn(), chat2.chat_id(), chat2.domain_id()});
        chat2.remove();
        REQUIRE_THROWS_AS((mss::Chat{db.conn(), chat2.chat_id(), chat2.domain_id()}), mss::IdNotFoundException);
    }
}

TEST_CASE("Test messages", "[storage][messages]")
{
    DBTestRAIIWrapper db;
    mss::Domain domain1(db.conn(), "Paul server", false);
    mss::Domain domain2(db.conn(), "Blaze server", false);

    mss::User user1(db.conn(), "Anna", domain2.domain_id(), mc::User::Status::ONLINE);
    mss::User user2(db.conn(), "Erick", domain1.domain_id(), mc::User::Status::OFFLINE);

    mss::Chat chat1(db.conn(), domain1.domain_id(), "secret_chat");
    mss::Chat chat2(db.conn(), domain2.domain_id(), "On domain2");

    std::chrono::time_point now = std::chrono::system_clock::now();

    mss::Message message1(db.conn(), chat1.chat_id(), chat1.domain_id(), user2.user_id(), user2.domain_id(),
                          ":D", now, mc::Message::Status::SENT);
    mss::Message message2(db.conn(), chat2.chat_id(), chat2.domain_id(), user2.user_id(), user2.domain_id(),
                          "Lorem ipsum", now, mc::Message::Status::SENT);

    SECTION("Select inserted messages")
    {
        mss::Message found_message1(db.conn(), message1.message_id(), message1.chat_id(), message1.domain_id_chat());
        REQUIRE(message1 == found_message1);

        REQUIRE_THROWS_AS((mss::Message{db.conn(), mc::INVALID_ID, message1.chat_id(), message1.domain_id_chat()}), mss::IdNotFoundException);
        REQUIRE_THROWS_AS((mss::Message{db.conn(), message1.message_id(), mc::INVALID_ID, message1.domain_id_chat()}), mss::IdNotFoundException);
        REQUIRE_THROWS_AS((mss::Message{db.conn(), message1.message_id(), message1.chat_id(), mc::INVALID_ID}), mss::IdNotFoundException);
        REQUIRE_THROWS_AS((mss::Message{db.conn(), mc::INVALID_ID, mc::INVALID_ID, mc::INVALID_ID}), mss::IdNotFoundException);
    }

    SECTION("Getters")
    {
        REQUIRE(message1.message_id() == 1);
        REQUIRE(message2.message_id() == 1);
        REQUIRE(message1.chat_id() == chat1.chat_id());
        REQUIRE(message2.chat_id() == chat2.chat_id());
        REQUIRE(message1.domain_id_chat() == chat1.domain_id());
        REQUIRE(message2.domain_id_chat() == chat2.domain_id());
        REQUIRE(message1.user_id() == user2.user_id());
        REQUIRE(message2.user_id() == user2.user_id());
        REQUIRE(message1.domain_id_user() == user2.domain_id());
        REQUIRE(message2.domain_id_user() == user2.domain_id());
        REQUIRE(message1.status() == mc::Message::Status::SENT);
        REQUIRE(message2.status() == mc::Message::Status::SENT);
        REQUIRE(message1.text() == ":D");
        REQUIRE(message2.text() == "Lorem ipsum");
        REQUIRE(message1.timestamp() == std::chrono::floor<std::chrono::seconds>(now));
        REQUIRE(message2.timestamp() == std::chrono::floor<std::chrono::seconds>(now));
    }

    SECTION("Setters")
    {
        message1.set_status(mc::Message::Status::SEEN);
        REQUIRE(message1.status() == mc::Message::Status::SEEN);

        message1.set_text("((((((");
        REQUIRE(message1.text() == "((((((");

        std::chrono::time_point test_time_point = std::chrono::system_clock::now();
        message1.set_timestamp(test_time_point);
        REQUIRE(message1.timestamp() == std::chrono::floor<std::chrono::seconds>(test_time_point));
    }

    SECTION("Test get_messages() for Chat")
    {
        mss::Message message4(db.conn(), chat1.chat_id(), chat1.domain_id(), user1.user_id(), user1.domain_id(),
                              "Sample text", std::chrono::system_clock::now(), mc::Message::Status::SENT);
        std::vector<mss::Message> messages_in_chat = chat1.get_messages();
        REQUIRE(std::find_if(messages_in_chat.begin(), messages_in_chat.end(), [&message1](const mss::Message& m){ return m == message1; }) != messages_in_chat.end());
        REQUIRE(std::find_if(messages_in_chat.begin(), messages_in_chat.end(), [&message4](const mss::Message& m){ return m == message4; }) != messages_in_chat.end());
        REQUIRE(messages_in_chat.size() == 2);
    }

    SECTION("Remove")
    {
        REQUIRE_NOTHROW(mss::Message{db.conn(), message1.message_id(), message1.chat_id(), message1.domain_id_chat()});
        message1.remove();
        REQUIRE_THROWS_AS((mss::Message{db.conn(), message1.message_id(), message1.chat_id(), message1.domain_id_chat()}), mss::IdNotFoundException);

        REQUIRE_NOTHROW(mss::Message{db.conn(), message2.message_id(), message2.chat_id(), message2.domain_id_chat()});
        message2.remove();
        REQUIRE_THROWS_AS((mss::Message{db.conn(), message2.message_id(), message2.chat_id(), message2.domain_id_chat()}), mss::IdNotFoundException);
    }
}
