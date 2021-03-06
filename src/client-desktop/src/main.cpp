#include <entities_db.hpp>
#include <main_window.hpp>
#include <melon/core/log_configuration.hpp>
#include <storage_singletones.hpp>

#include <QApplication>

#include <iostream>

int main(int argc, char* argv[]) try
{
    QApplication application(argc, argv);

    [[maybe_unused]] const auto& storage = melon::client_desktop::DBSingletone::get_instance();

    melon::core::log::setup();

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
