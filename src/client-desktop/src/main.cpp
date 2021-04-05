#include <db_storage.hpp>
#include <main_window.hpp>

#include <QApplication>
#include <QFile>

#include <iostream>

namespace melon::client_desktop
{

void create_connection_with_db()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));

    auto& storage = DBSingletone::get_instance();
    QString db_name = storage.db_name();

    db.setDatabaseName(db_name);

    bool exists = QFile::exists(db_name);

    if (!db.open())
        throw QtSqlException("Fail opening database");

    QSqlQuery query(db);

    exec_and_check_qtsql_query(query, QStringLiteral("PRAGMA foreign_keys=ON"), "Foreign keys ON");

    if (exists)
        return;

    QString qry_string = QStringLiteral("CREATE TABLE [domains]"
                                        "(domain_id INT NOT NULL,"
                                        " hostname TEXT,"
                                        " external INT,"
                                        " PRIMARY KEY (domain_id))");
    exec_and_check_qtsql_query(query, qry_string, "Creating [domains] table");

    qry_string = QStringLiteral("CREATE TABLE [users]"
                                "(user_id INT NOT NULL,"
                                " username TEXT,"
                                " status INT,"
                                " domain_id INT NOT NULL,"
                                " PRIMARY KEY (user_id, domain_id),"
                                " FOREIGN KEY (domain_id) REFERENCES domains(domain_id) ON DELETE CASCADE)");
    exec_and_check_qtsql_query(query, qry_string, "Creating [users] table");

    qry_string = QStringLiteral("CREATE TABLE [chats]"
                                "(chat_id INT NOT NULL,"
                                " domain_id INT NOT NULL,"
                                " name TEXT,"
                                " PRIMARY KEY (chat_id, domain_id),"
                                " FOREIGN KEY (domain_id) REFERENCES domains(domain_id) ON DELETE CASCADE)");
    exec_and_check_qtsql_query(query, qry_string, "Creating [chats] table");


    qry_string = QStringLiteral("CREATE TABLE [messages]"
                                "(message_id int NOT NULL,"
                                " chat_id INT NOT NULL,"
                                " domain_id_chat INT NOT NULL,"
                                " user_id INT,"
                                " domain_id_user INT,"
                                " timestamp INT,"
                                " text TEXT,"
                                " status INT, "
                                " edit INT, "
                                " PRIMARY KEY (message_id, chat_id, domain_id_chat),"
                                " FOREIGN KEY (chat_id, domain_id_chat) REFERENCES chats(chat_id, domain_id) ON DELETE CASCADE,"
                                " FOREIGN KEY (user_id, domain_id_user) REFERENCES users(user_id, domain_id) ON DELETE CASCADE)");
    exec_and_check_qtsql_query(query, qry_string, "Creating [messages] table");

    exec_and_check_qtsql_query(query, QStringLiteral("INSERT INTO domains VALUES (1, 'melon', 0)"), "Insert basic domain");

    qry_string = QStringLiteral("INSERT INTO users VALUES "
                                   "(1, 'MelonUser', 0, 1),"
                                   "(2, 'SomeSender', 0, 1)");
    exec_and_check_qtsql_query(query, qry_string, "Insert two basic users");
}

}  // namespace melon::client_desktop

int main(int argc, char* argv[]) try
{
    QApplication application(argc, argv);

    melon::client_desktop::create_connection_with_db();

    melon::client_desktop::MainWindow window;
    window.show();
    return QApplication::exec();
}
catch (const melon::client_desktop::QtSqlException& e)
{
    std::cerr << "Error with DB! " << e.what() << std::endl;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
catch (...)
{
    std::cerr << "Unknown error!" << std::endl;
}
