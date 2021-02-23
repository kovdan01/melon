#ifndef MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_
#define MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_

#include <ui_main_window.h>

#include <chat_widget.hpp>

#include <QMainWindow>
#include <QSpacerItem>

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
    ChatWidget* m_chat_widget = nullptr;
    QSpacerItem* m_spacer = nullptr;

    void replace_chat_widget_with_spacer();
    void replace_spacer_with_chat_widget();
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_
