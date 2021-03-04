#include <main_window.hpp>

#include <QApplication>

namespace mc = melon::client_desktop;

int main(int argc, char* argv[]) try
{
    QApplication application(argc, argv);
    mc::MainWindow window;
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
