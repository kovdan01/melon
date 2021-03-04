#ifndef MELON_CLIENT_DESKTOP_CHAT_LIST_HPP_
#define MELON_CLIENT_DESKTOP_CHAT_LIST_HPP_

#include <QListView>

namespace melon::client_desktop
{

class ChatListView : public QListView
{
    Q_OBJECT

public:
    ChatListView(QWidget* parent = nullptr);
    ~ChatListView() override = default;

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_CHAT_LIST_HPP_

