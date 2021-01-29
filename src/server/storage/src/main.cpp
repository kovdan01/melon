#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>

#include <iostream>

#include "melondb.h"

namespace mysql = sqlpp::mysql;

int main()
{
    auto config = std::make_shared<mysql::connection_config>();
    config->user = "melon";
    config->database = "melon";
    config->host = "localhost";
    config->password = "melonpass";
    //config->debug = true;
    try
    {
        mysql::connection db(config);
    }
    catch (const sqlpp::exception& e)
    {
        std::cerr << "For testing, you'll need to create a database melon for user melon on localhost with password" << std::endl;
        std::cerr << e.what() << std::endl;
        return 1;
    }
    try
    {
        mysql::connection db(config);
        const auto users = melon::Users{};
        db(insert_into(users).set(users.username = "Pablo Escobar", users.status = 0));
        db(update(users).set(users.status = 1).where(users.username == "Pablo Escobar"));
        auto preparedSelectAll = db.prepare(sqlpp::select(all_of(users)).from(users).unconditionally());
        for (const auto& row : db(preparedSelectAll))
        {
            std::string username = row.username;
            bool userstatus = row.status;
            std::cout << ">>> username: " << username << ", status: " << userstatus << std::endl;
        }
        db(remove_from(users).where(users.username == "Pablo Escobar"));
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
