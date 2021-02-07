#include "main_window.hpp"

#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtWidgets/QMessageBox>

namespace mc = melon::client_desktop;

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    mc::MainWindow window;
    window.show();
    return QApplication::exec();
}
