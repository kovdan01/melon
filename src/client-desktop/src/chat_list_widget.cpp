#include <chat_list_widget.hpp>

#include <QMouseEvent>

namespace melon::client_desktop
{

ChatListWidget::ChatListWidget(QWidget* parent)
    : QListWidget{parent}
{}

void ChatListWidget::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        emit customContextMenuRequested(event->pos());
    }
    else
    {
        QListView::mousePressEvent(event);
    }
}

}  // namespace melon::client_desktop
