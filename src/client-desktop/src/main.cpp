#include "main_window.hpp"

#include <QApplication>

namespace mc = melon::client_desktop;

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    mc::MainWindow window;
    window.show();
    return application.exec();
}
