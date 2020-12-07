#ifndef MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP
#define MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP

#include "ui_chat_widget.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui  // NOLINT (readability-identifier-naming)
{
class ChatWidget;
} // namespace Ui
QT_END_NAMESPACE

namespace melon::client_desktop
{

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget() override = default;

private slots:
    void send_message();
    void receive_message();

private:  // NOLINT (readability-redundant-access-specifiers)
    QScopedPointer<Ui::ChatWidget> m_ui;
};

}  // namespace melon::client_desktop

#endif // MELON_CLIENT_DESKTOP_CHAT_WIDGET_HPP
