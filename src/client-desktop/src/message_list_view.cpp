#include <message_list_view.hpp>

#include <QMouseEvent>

namespace melon::client_desktop
{

MessageListView::MessageListView(QWidget* parent)
    : QListView{parent}
{}

void MessageListView::mousePressEvent(QMouseEvent* event)
{
    if (m_multiselection_mode &&
        event->button() == Qt::MouseButton::LeftButton &&
        this->selectionModel()->isSelected(this->indexAt(event->pos())) &&
        this->selectionModel()->selectedIndexes().size() == 1)
    {
        this->selectionModel()->clearSelection();
        this->set_multiselection_mode(false);
    }
    else
    {
        QListView::mousePressEvent(event);
    }
}

void MessageListView::set_multiselection_mode(bool mode)
{
    m_multiselection_mode = mode;
    if(mode)
    {
        BOOST_LOG_TRIVIAL(info) << "Now message selection mode is MultiSelection";
        this->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Now message selection mode is NoSelection";
        this->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    }
}

}  // namespace melon::client_desktop
