#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>

namespace mysql = sqlpp::mysql;
int main()
{
    auto config = std::make_shared<mysql::connection_config>();
    config->user = "root";
    config->database = "melon";
    config->debug = true;
    mysql::connection db(config);
    return 0;
}
