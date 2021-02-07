#ifndef MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_
#define MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_

#include "ui_main_window.h"

#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui  // NOLINT (readability-identifier-naming)
{
    class MainWindow;
}  // namespace Ui
QT_END_NAMESPACE

namespace melon::client_desktop
{

class ChatWidgetModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void add_chat();
    void provide_chat_context_menu(const QPoint& pos);
    void delete_chat();
    void rename_chat();

private:  // NOLINT (readability-redundant-access-specifiers)
    QScopedPointer<Ui::MainWindow> m_ui;
    ChatWidgetModel* chat_model;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_
