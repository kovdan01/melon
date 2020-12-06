#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include "ui_chat_widget.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ChatWidget;
}
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

private:
    QScopedPointer<Ui::ChatWidget> m_ui;
};

}

#endif // CHAT_WIDGET_H
