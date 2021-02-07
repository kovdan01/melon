#include "main_window.hpp"

#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtWidgets/QMessageBox>

namespace mc = melon::client_desktop;

namespace
{
    QString createDatabase()
    {
        auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
        db.setDatabaseName(QStringLiteral(":memory:"));
        auto error = [&](QString message){
            return message.arg(db.lastError().text());
        };
        if (!db.open())
            return error(qApp->translate("CreateDB","Couldn't create in-memory database: %1"));
        QSqlQuery query;
        if (!query.exec(QStringLiteral(
                "CREATE TABLE messages (ID INTEGER PRIMARY KEY, text TEXT, chat_ind INTEGER, is_my INTEGER)")))
            return error(qApp->translate("CreateDB","Couldn't create table: %1"));
        if (!query.prepare(QStringLiteral("INSERT INTO messages (text, chat_ind, is_my) VALUES (?,?,?)")))
            return error(qApp->translate("CreateDB","Couldn't prepare insert query"));

        query.addBindValue(QStringLiteral("Here's your Mind Palace"));
        query.addBindValue(1);
        query.addBindValue(1);
        if (!query.exec())
            return error(qApp->translate("CreateDB","Failed to insert row: %1"));
        return{};
    }
}

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QString error = createDatabase();
    if (!error.isEmpty())
    {
        QMessageBox::critical(nullptr,application.translate("CreatingDB","Error"),error);
        return 1;
    }
    mc::MainWindow window;
    window.show();
    return QApplication::exec();
}
