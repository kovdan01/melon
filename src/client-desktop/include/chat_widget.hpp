#ifndef MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP_
#define MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP_

#include <ram_storage.hpp>

#include <ui_chat_widget.h>

#include <QWidget>

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

    using chat_handle_t = RAMStorageSingletone::chat_handle_t;
    using const_chat_handle_t = RAMStorageSingletone::const_chat_handle_t;

public:
    ChatWidget(QWidget* parent = nullptr);
    ~ChatWidget() override = default;


    void set_current_chat_it(chat_handle_t it) noexcept
    {
        m_current_chat_it = it;
    }

    [[nodiscard]] chat_handle_t chat_by_qlistitem(QListWidgetItem* item)
    {
        QVariant v = item->data(Qt::UserRole);
        return v.value<chat_handle_t>();
    }

public slots:  // NOLINT (readability-redundant-access-specifiers)
    void change_chat(QListWidgetItem* current_chat, QListWidgetItem* previous_chat);
    Message capture_message_from_editor();
    void load_message_to_editor(const Message& msg);
    QListWidgetItem* load_message_into_item(const Message& msg);

private slots:
    void send_message();
    void receive_message();
    void provide_chat_context_menu(const QPoint& pos);
    void delete_msg();
    void edit_msg();

private:  // NOLINT (readability-redundant-access-specifiers)
    QScopedPointer<Ui::ChatWidget> m_ui;
    QListWidgetItem* m_current_chat_item = nullptr;
    RAMStorageSingletone::chat_handle_t m_current_chat_it;
    bool m_edit_mode = false;
    QListWidgetItem* m_edit_item = nullptr;
};

namespace rgba_receive
{
constexpr int R = 250;
constexpr int G = 224;
constexpr int B = 180;
constexpr int A = 127;
}  // namespace rgba_receive

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP_
