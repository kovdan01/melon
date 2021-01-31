#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

#include <iostream>
#include <array>

#include "melondb.h"

namespace mysql = sqlpp::mysql;

struct User
{
    std::string username;
    long userid;
    bool status;
};

const auto users = melon::Users{};

static auto config_melondb()
{
    auto config = std::make_shared<mysql::connection_config>();
    config->user = "melon";
    config->database = "melon";
    config->host = "localhost";
    config->password = "melonpass";
    return config;
}

static std::vector<std::string> OnlineUsersNamesGet(mysql::connection& db)
{
    std::vector<std::string> onlineUsersNames;
    User user;
    for (const auto& row : db(select(users.username).from(users).where(users.status == 1)))
    {
        onlineUsersNames.push_back(row.username);
    }
    return onlineUsersNames;
}

static std::vector<User> OnlineUsersGet(mysql::connection& db)
{
    std::vector<User> onlineUsers;
    User user;
    for (const auto& row : db(select(all_of(users)).from(users).where(users.status == 1)))
    {
        user.userid = row.userId;
        user.username = row.username;
        user.status = row.status;
        onlineUsers.push_back(user);
    }
    return onlineUsers;
}

static void AddUser(mysql::connection& db, User user)
{
    db(insert_into(users).set(users.username = user.username, users.status = 0));
}

static void MakeUserOnline(mysql::connection& db, User user)
{
    db(update(users).set(users.status = 1).where(users.username == user.username));
}

static void MakeUserOffline(mysql::connection& db, User user)
{
    db(update(users).set(users.status = 0).where(users.username == user.username));
}



int main()
{

    try
    {
        mysql::connection db(config_melondb());
        db.execute(R"(DROP TABLE IF EXISTS users)");
        db.execute(R"(CREATE TABLE users (
        user_id INT NOT NULL AUTO_INCREMENT,
            username varchar(100) not null,
            status boolean not null default 0,
            PRIMARY KEY (user_id)
            ))");

        User user;
        user.username = "h3ll0kitt1";
        AddUser(db, user);

        std::vector<User> onlineUsers = OnlineUsersGet(db);

        std::cout << "Online users: " << std::endl;
        for (auto& a: onlineUsers)
        {
            std:: cout << a.userid << " : " << a.username << std::endl;
        }

        std::cout << "Change status for online" << std::endl;
        MakeUserOnline(db, user);
        std::vector<std::string> onlineUsersNames = OnlineUsersNamesGet(db);

        std::cout << "Online users: " << std::endl;
        for (const auto &a: onlineUsersNames)
        {
            std:: cout << a << std::endl;
        }

        std::cout << "Change status for offline" << std::endl;
        MakeUserOffline(db, user);
        onlineUsersNames = OnlineUsersNamesGet(db);

        std::cout << "Online users: " << std::endl;
        for (auto& a: onlineUsersNames)
        {
            std:: cout << a << std::endl;
        }

    }
    catch (const sqlpp::exception& e)
    {
        std::cerr << "For testing, you'll need to create a database melon for user melon on localhost with password melonpass" << std::endl;
        std::cerr << e.what() << std::endl;
    }

}
