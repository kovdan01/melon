#ifndef MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP
#define MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP

#include "ui_main_window.h"
#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

namespace melon::client_desktop
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void add_chat();

//    void eraseItem();
//    void on_MenuList_itemDoubleClicked(QListWidgetItem *item);

private:
    QScopedPointer<Ui::MainWindow> m_ui;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP
