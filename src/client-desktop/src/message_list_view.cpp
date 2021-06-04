#include <message_list_view.hpp>

#include <QMouseEvent>

namespace melon::client_desktop
{

MessageListView::MessageListView(QWidget* parent)
    : QListView{parent}
{}

void MessageListView::mousePressEvent(QMouseEvent* event)
{

    if (m_selection_mode &&
        event->button() == Qt::MouseButton::LeftButton &&
        this->selectionModel()->isSelected(this->indexAt(event->pos())) &&
        this->selectionModel()->selectedIndexes().size() == 1)
    {
        BOOST_LOG_TRIVIAL(info) << "[last] Current row is " << this->indexAt(event->pos()).row();
        this->selectionModel()->clear();
        this->disable_selection_mode();
    }
    else if (event->button() == Qt::MouseButton::RightButton)
    {
        BOOST_LOG_TRIVIAL(info) << "There was right button click";
        emit customContextMenuRequested(event->pos());
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "There was another click";
        QListView::mousePressEvent(event);
    }
}

void MessageListView::disable_selection_mode()
{
    m_selection_mode = false;
    this->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    BOOST_LOG_TRIVIAL(info) << "Now SelectionMode is NoSelection";
}

}  // namespace melon::client_desktop
