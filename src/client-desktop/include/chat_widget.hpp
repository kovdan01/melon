#ifndef MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP_
#define MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP_

#include <ram_storage.hpp>
#include <message_list_model.hpp>

#include <ui_chat_widget.h>

#include <QWidget>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui  // NOLINT (readability-identifier-naming)
{
class ChatWidget;
}  // namespace Ui
QT_END_NAMESPACE

namespace melon::client_desktop
{

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    ChatWidget(QWidget* parent = nullptr);
    ~ChatWidget() override = default;

    using chat_handle_t = RAMStorageSingletone::chat_handle_t;
    using const_chat_handle_t = RAMStorageSingletone::const_chat_handle_t;
    using message_handle_t = Chat::message_handle_t;

    void set_current_chat_it(chat_handle_t it)
    {
        m_current_chat_it = it;
    }

    template<typename ItType>
    [[nodiscard]] ItType it_by_qlistitem(QListWidgetItem* item)
    {
        return item->data(Qt::UserRole).value<ItType>();
    }

public slots:  // NOLINT (readability-redundant-access-specifiers)
    void change_chat(QListWidgetItem* current_chat, QListWidgetItem* previous_chat);
    Message capture_message_from_editor();
    void load_message_to_editor(const Message& message);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void receive_message();
    void send_message();
    void provide_message_context_menu(const QPoint& pos);
    void delete_message();
    void edit_message();

private:  // NOLINT (readability-redundant-access-specifiers)
    QMenu m_submenu_sended_messages{this};
    QMenu m_submenu_received_messages{this};
    MessageListModel* m_model_message_list = new MessageListModel{this};
    QListWidgetItem* m_current_chat_item = nullptr;
    QScopedPointer<Ui::ChatWidget> m_ui;
    chat_handle_t m_current_chat_it;
    QSet<int> m_pressed_keys;
    int m_edit_row;
    bool m_edit_mode = false;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP_
