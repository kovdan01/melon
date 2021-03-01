#ifndef MELON_CLIENT_DESKTOP_CHAT_LIST_HPP_
#define MELON_CLIENT_DESKTOP_CHAT_LIST_HPP_

#include <QListWidget>

namespace melon::client_desktop
{

class ChatListWidget : public QListWidget
{
    Q_OBJECT

public:
    ChatListWidget(QWidget* parent = nullptr);
    ~ChatListWidget() override = default;

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_CHAT_LIST_HPP_

