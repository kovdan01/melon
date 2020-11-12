#include "main_window.hpp"

#include <QApplication>

namespace mc = melon::client_desktop;

#ifdef __MINGW64__

#include <windows.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc = 0;
    QApplication app(argc, nullptr);
    mc::MainWindow window;
    window.show();
    return QApplication::exec();
}

#else

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    mc::MainWindow window;
    window.show();
    return QApplication::exec();
}

#endif
