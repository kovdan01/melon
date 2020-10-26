#ifndef MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP
#define MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP

#include "ui_main_window.h"

#include <QMainWindow>

#include <memory>

namespace melon::client_desktop
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    std::unique_ptr<Ui::MainWindow> m_ui;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP
