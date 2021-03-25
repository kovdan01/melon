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
    db.setDatabaseName(DB_NAME);

    auto error = [&](QString message)
    {
        return message.arg(db.lastError().text());
    };

    bool is_exist = QFile::exists(DB_NAME);

    if(!db.open())
        return error(qApp->translate("OpenDB", "Couldn't open database! %1"));

    if (!is_exist)
    {
        QSqlQuery query(db);

        std::cout << "File does not exist!" << std::endl;
        if(!query.exec(QStringLiteral("CREATE TABLE messages(message_id int NOT NULL,"
                                      " user_id int NOT NULL,"
                                      " chat_id int NOT NULL,"
                                      " domain_id int,"
                                      " timestamp int,"
                                      " text text,"
                                      " status int, "
                                      " PRIMARY KEY (message_id, user_id, chat_id))")))
        {
            std::cout << "Error: " << query.lastError().text().toStdString() << std::endl;
            return error(qApp->translate("CreateDB", "Couldn't create messages table: %1"));
        }

        if(!query.exec(QStringLiteral("CREATE TABLE chats (chat_id int NOT NULL,"
                                      " domain_id int NOT NULL,"
                                      " name text,"
                                      " PRIMARY KEY (chat_id, domain_id))")))
        {
            return error(qApp->translate("CreateDB", "Couldn't create chats table"));
        }
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
        QMessageBox::critical(nullptr, application.translate("CreateDB", "Critical error"), error);
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
