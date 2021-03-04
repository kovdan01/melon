#include <chat_list_view.hpp>

#include <QMouseEvent>

namespace melon::client_desktop
{

ChatListView::ChatListView(QWidget* parent)
    : QListView{parent}
{}

void ChatListView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        emit customContextMenuRequested(event->pos());
    }
    else
    {
        QListView::mousePressEvent(event);
    }
}

}  // namespace melon::client_desktop
