#include "main_window.hpp"

#include <QApplication>

namespace mc = melon::client_desktop;

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    mc::MainWindow w;
    w.show();
    return a.exec();
}
