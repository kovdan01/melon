#include <db_storage.hpp>
#include <main_window.hpp>

#include <QApplication>
#include <QFile>

#include <iostream>

namespace melon::client_desktop
{

QString create_connection_with_db()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));

    auto& storage = StorageSingletone::get_instance();
    QString db_name = storage.db_name();

    db.setDatabaseName(db_name);

    bool exists = QFile::exists(db_name);

    if (!db.open())
        return qApp->translate("OpenDB", "Couldn't open database! %1");

    if (exists)
        return {};

    QSqlQuery query(db);

    if (!query.exec(QStringLiteral("PRAGMA foreign_keys = ON")))
    {
        std::cout << "Error: " << query.lastError().text().toStdString() << std::endl;
        return qApp->translate("CreateDB", "Couldn't create messages table!");
    }

    if (!query.exec(QStringLiteral("CREATE TABLE [domains]"
                                  "(domain_id INT NOT NULL,"
                                  " hostname TEXT,"
                                  " external INT,"
                                  " PRIMARY KEY (domain_id))")))
    {
        std::cout << "Error: " << query.lastError().text().toStdString() << std::endl;
        return qApp->translate("CreateDB", "Couldn't create domains table");
    }

    if (!query.exec(QStringLiteral("CREATE TABLE [users]"
                                  "(user_id INT NOT NULL,"
                                  " username TEXT,"
                                  " status INT,"
                                  " domain_id INT NOT NULL,"
                                  " PRIMARY KEY (user_id, domain_id),"
                                  " FOREIGN KEY (domain_id) REFERENCES domains(domain_id) ON DELETE CASCADE)")))
    {
        std::cout << "Error: " << query.lastError().text().toStdString() << std::endl;
        return qApp->translate("CreateDB", "Couldn't create users table");
    }

    if (!query.exec(QStringLiteral("CREATE TABLE [chats]"
                                  "(chat_id INT NOT NULL,"
                                  " domain_id INT NOT NULL,"
                                  " name TEXT,"
                                  " PRIMARY KEY (chat_id, domain_id),"
                                  " FOREIGN KEY (domain_id) REFERENCES domains(domain_id) ON DELETE CASCADE)")))
    {
        std::cout << "Error: " << query.lastError().text().toStdString() << std::endl;
        return qApp->translate("CreateDB", "Couldn't create chats table");
    }

    if (!query.exec(QStringLiteral("CREATE TABLE [messages]"
                                  "(message_id int NOT NULL,"
                                  " chat_id INT NOT NULL,"
                                  " domain_id_chat INT NOT NULL,"
                                  " user_id INT,"
                                  " domain_id_user INT,"
                                  " timestamp INT,"
                                  " text TEXT,"
                                  " status INT, "
                                  " PRIMARY KEY (message_id, chat_id, domain_id_chat),"
                                  " FOREIGN KEY (chat_id, domain_id_chat) REFERENCES chats(chat_id, domain_id) ON DELETE CASCADE,"
                                  " FOREIGN KEY (user_id, domain_id_user) REFERENCES users(user_id, domain_id) ON DELETE CASCADE)")))
    {
        std::cout << "Error: " << query.lastError().text().toStdString() << std::endl;
        return qApp->translate("CreateDB", "Couldn't create messages table!");
    }

    if (!query.exec(QStringLiteral("INSERT INTO domains VALUES (1, 'melon', 0)")))
    {
        std::cout << "Error: " << query.lastError().text().toStdString() << std::endl;
        return qApp->translate("CreateDB", "Couldn't insert into domains table!");
    }

    if (!query.exec(QStringLiteral("INSERT INTO users VALUES "
                                   "(1, 'MelonUser', 0, 1),"
                                   "(2, 'SomeSender', 0, 1)")))
    {
        std::cout << "Error: " << query.lastError().text().toStdString() << std::endl;
        return qApp->translate("CreateDB", "Couldn't insert into users table!");
    }

    return {};
}
}  // namespace melon::client_desktop

int main(int argc, char* argv[]) try
{
    QApplication application(argc, argv);

    QString error = melon::client_desktop::create_connection_with_db();
    if (!error.isEmpty())
    {
        QMessageBox::critical(nullptr, QApplication::translate("CreateDB", "Critical error"), error);
        return 1;
    }

    melon::client_desktop::MainWindow window;
    window.show();
    return QApplication::exec();
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
catch (...)
{
    std::cerr << "Unknown error!" << std::endl;
}
