#include <entities_db.hpp>
#include <main_window.hpp>
#include <storage_singletones.hpp>

#include <QApplication>
#include <QFile>

#include <iostream>

int main(int argc, char* argv[]) try
{
    QApplication application(argc, argv);

    //melon::client_desktop::create_connection_with_db();

    [[maybe_unused]] const auto& storage = melon::client_desktop::DBSingletone::get_instance();

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
