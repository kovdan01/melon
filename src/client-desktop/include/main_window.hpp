#ifndef MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_
#define MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_

#include <ui_main_window.h>

#include <chat_item_delegate.hpp>
#include <chat_list_view.hpp>
#include <chat_list_model.hpp>
#include <chat_widget.hpp>
#include <settings_dialog.hpp>

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

class ChatNameException : public melon::Exception
{
public:
    using melon::Exception::Exception;
    ~ChatNameException() override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    using MyRoles = ChatListModel::MyRoles;
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

    using chat_handle_t = std::list<Chat>::iterator;

private slots:
    void add_chat();
    void provide_chat_context_menu(const QPoint& pos);
    void delete_chat();
    void rename_chat();
    void repaint_chat_list();
    void apply_appearance_settings();

    void change_chat(const QModelIndex& current_chat, const QModelIndex& previous_chat);

    void exec_settings();

private:  // NOLINT (readability-redundant-access-specifiers)
    QMenu m_submenu{this};
    QScopedPointer<Ui::MainWindow> m_ui;
    ChatWidget* m_chat_widget = nullptr;
    QSpacerItem* m_spacer = nullptr;
    QPoint m_requested_menu_position;
    ChatListModel* m_model_chat_list = new ChatListModel{this};
    ChatItemDelegate* m_chat_item_delegate;
    SettingsDialog* m_settings_dialog = new SettingsDialog{this};

    void replace_chat_widget_with_spacer();
    void replace_spacer_with_chat_widget();
    void load_data_from_database();
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MAIN_WINDOW_HPP_
