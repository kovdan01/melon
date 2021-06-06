#include <ui_chat_widget.h>

#include <chat_widget.hpp>
#include <entities_db.hpp>
#include <message_list_model.hpp>

#include <QMainWindow>
#include <QMenuBar>
#include <QMouseEvent>
#include <QScrollBar>

#include <chrono>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <config.hpp>

namespace melon::client_desktop
{

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget{parent}
    , m_ui{new Ui::ChatWidget}
{
    m_ui->setupUi(this);
    connect(m_ui->SendButton, &QPushButton::clicked, this, &ChatWidget::send_message);
    connect(m_ui->ReceiveButton, &QPushButton::clicked, this, &ChatWidget::receive_message);

    m_ui->MsgList->setModel(m_model_message_list);

    m_message_item_delegate = new MessageItemDelegate{m_ui->MsgList};
    m_ui->MsgList->setItemDelegate(m_message_item_delegate);

    connect(m_ui->MsgList,
            &QWidget::customContextMenuRequested,
            this,
            &ChatWidget::provide_message_context_menu);

    m_submenu_sended_message.addAction(tr("Edit"), this, SLOT(edit_message()));
    m_submenu_sended_message.addAction(tr("Delete"), this, SLOT(delete_message()));
    m_submenu_sended_message.addAction(tr("Select"), this, SLOT(select_message()));

    m_submenu_received_message.addAction(tr("Delete"), this, SLOT(delete_message()));
    m_submenu_received_message.addAction(tr("Select"), this, SLOT(select_message()));

    m_submenu_multiselection.addAction(tr("Delete"), this, SLOT(delete_message()));

    m_ui->MsgEdit->installEventFilter(this);
}


bool ChatWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto* event_key = static_cast<QKeyEvent*>(event);
        int key = event_key->key();
        m_pressed_keys += key;

        auto& config = UserConfigSingletone::get_instance();
        if ( (m_pressed_keys.contains(Qt::Key_Enter) || m_pressed_keys.contains(Qt::Key_Return))
              && m_pressed_keys.contains(Qt::Key_Shift)
              && config.behaviour().send_message_by_enter())
        {
            QTextCursor cursor = m_ui->MsgEdit->textCursor();
            cursor.insertText(QStringLiteral("\n"));
            return true;
        }

        if ( (m_pressed_keys.contains(Qt::Key_Enter) || m_pressed_keys.contains(Qt::Key_Return))
             && config.behaviour().send_message_by_enter())
        {
            ChatWidget::send_message();
            return true;
        }
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        auto* event_key = static_cast<QKeyEvent*>(event);
        int key = event_key->key();
        m_pressed_keys -= key;
        return true;
    }

    return object->eventFilter(object, event);
}

void ChatWidget::send_message()
{
    QString message_text = m_ui->MsgEdit->toPlainText();

    if (message_text.isEmpty())
        return;

    auto& config = UserConfigSingletone::get_instance();

    if (config.behaviour().remove_whitespaces_around())
        message_text = message_text.trimmed();

    if (config.behaviour().replace_hyphens())
    {
        static const QString two_hyphens = QStringLiteral("--");
        static const QString dash = QStringLiteral("–");
        message_text.replace(two_hyphens, dash);
    }

    if (m_edit_mode)
    {
        if (message_text != m_pre_edit_message)
        {
            QModelIndex index = m_model_message_list->index(m_edit_row);

            m_model_message_list->setData(index, message_text, Qt::DisplayRole);
            m_model_message_list->setData(index, true, MyRoles::RepaintRole);
            m_pre_edit_message.clear();
        }

        m_edit_mode = false;
        m_ui->MsgEdit->clear();
        m_ui->ReceiveButton->setVisible(true);
        m_ui->SendButton->setText(QStringLiteral("Send"));

        m_ui->MsgEdit->setText(m_incomplete_message);
        m_incomplete_message.clear();

        if (m_edit_row == m_model_message_list->rowCount(QModelIndex()) - 1)
            emit this->last_message_changed();
        m_ui->MsgList->selectionModel()->clearSelection();
        return;
    }


    auto& storage = DBSingletone::get_instance();

    Message new_message(m_current_chat_it->chat_id(), m_current_chat_it->domain_id(),
                        storage.me().user_id(), storage.me().domain_id(),
                        message_text,
                        std::chrono::system_clock::now(),
                        Message::Status::SENT);

    m_ui->MsgEdit->clear();
    m_model_message_list->add_message(m_current_chat_it, new_message);
    m_ui->MsgList->scrollToBottom();

    m_ui->MsgEdit->setFocus();

    emit this->last_message_changed();
}

void ChatWidget::receive_message()
{
    auto& storage = DBSingletone::get_instance();
    Message new_message(m_current_chat_it->chat_id(), m_current_chat_it->domain_id(),
                        storage.another_user().user_id(), storage.another_user().domain_id(),
                        QStringLiteral("I wish I could hear you."),
                        std::chrono::system_clock::now(),
                        Message::Status::RECEIVED);

    m_model_message_list->add_message(m_current_chat_it, new_message);

    m_ui->MsgList->scrollToBottom();

    m_ui->MsgEdit->setFocus();
    emit this->last_message_changed();
}

void ChatWidget::change_chat(chat_handle_t current_it)
{
    this->set_current_chat_it(current_it);

    m_model_message_list->clear();

    std::list<Message>& messages = m_current_chat_it->messages();

    for (auto it_message = messages.begin(); it_message != messages.end(); ++it_message)
    {
        m_model_message_list->load_message(it_message);
    }

    this->load_message_to_editor(m_current_chat_it->incomplete_message());

    int my_scroll_pos = m_current_chat_it->scrolling_position();
    m_ui->MsgList->verticalScrollBar()->setMaximum(my_scroll_pos);
    m_ui->MsgList->verticalScrollBar()->setValue(my_scroll_pos);

    m_ui->MsgEdit->setFocus();
    QTextCursor cursor = m_ui->MsgEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_ui->MsgEdit->setTextCursor(cursor);

//    m_ui->MsgList->clearSelection();
    m_ui->MsgList->scrollToBottom();
}

void ChatWidget::change_chat(chat_handle_t current_it, chat_handle_t previous_it)
{
    m_ui->MsgList->set_multiselection_mode(false);
    m_ui->MsgList->clearSelection();
    previous_it->set_incomplete_message(capture_message_from_editor());
    previous_it->set_scrolling_position(m_ui->MsgList->verticalScrollBar()->value());

    this->change_chat(current_it);
}

Message ChatWidget::capture_message_from_editor()
{
    return m_ui->MsgEdit->toPlainText();
}

void ChatWidget::load_message_to_editor(const Message& message)
{
    m_ui->MsgEdit->setText(message.text());
}

void ChatWidget::provide_message_context_menu(const QPoint& pos)
{
    m_pos_menu = pos;
    QModelIndexList selected_indexes = m_ui->MsgList->selectionModel()->selectedIndexes();
//    m_ui->MsgList->selectionModel()->select(m_ui->MsgList->indexAt(pos), QItemSelectionModel::Toggle);
    BOOST_LOG_TRIVIAL(info) << "Count of selected items: " << selected_indexes.size();

    // for single selection
    // when popup menu triggered and we are not in multiselection mode,
    // NO items are selected!
    if (selected_indexes.count() == 1 || !m_ui->MsgList->multiselection_mode())
    {
        BOOST_LOG_TRIVIAL(info) << "In single selection context menu";
        QModelIndex cur_index = m_ui->MsgList->selectionModel()->currentIndex();

        auto it_message = this->m_model_message_list->data(cur_index, Qt::DisplayRole).value<message_handle_t>();

        if (it_message->status() == Message::Status::SENT)
            m_submenu_sended_message.popup(m_ui->MsgList->mapToGlobal(pos));
        else
            m_submenu_received_message.popup(m_ui->MsgList->mapToGlobal(pos));
    }
    else  // for multiselection
    {
        m_submenu_multiselection.popup(m_ui->MsgList->mapToGlobal(pos));
    }
}

void ChatWidget::select_message()
{
    m_ui->MsgList->selectionModel()->select(m_ui->MsgList->indexAt(m_pos_menu), QItemSelectionModel::Toggle);
    m_ui->MsgList->set_multiselection_mode(true);
}

void ChatWidget::delete_message()
{
    QModelIndexList selected_indexes = m_ui->MsgList->selectionModel()->selectedIndexes();
    QModelIndex cur_index = m_ui->MsgList->selectionModel()->currentIndex();
    int row = cur_index.row();

    // If we want to delete just one message in NoSelection mode, we should set it selected
    if (selected_indexes.empty())
        m_ui->MsgList->selectionModel()->select(cur_index, QItemSelectionModel::Toggle);

    BOOST_LOG_TRIVIAL(info) << "[delete msgs] Count of items to delete: " << m_ui->MsgList->selectionModel()->selectedIndexes().size();
    while (!(selected_indexes = m_ui->MsgList->selectionModel()->selectedIndexes()).empty())
    {
        cur_index = selected_indexes.first();
        m_model_message_list->delete_message(m_current_chat_it, cur_index);
        row = cur_index.row();
    }

    if (row == m_model_message_list->rowCount(QModelIndex()))
        emit this->last_message_changed();
    m_ui->MsgList->set_multiselection_mode(false);
}

void ChatWidget::edit_message()
{
    m_ui->MsgList->selectionModel()->select(m_ui->MsgList->selectionModel()->currentIndex(),
                                            QItemSelectionModel::Toggle);
    QModelIndex index = m_ui->MsgList->selectionModel()->currentIndex();

    m_edit_mode = true;
    m_edit_row = index.row();

    auto message_text = m_model_message_list->data(index, MyRoles::MessageTextRole).toString();
    m_pre_edit_message = message_text;

    m_incomplete_message = m_ui->MsgEdit->toPlainText();
    m_ui->MsgEdit->setText(message_text);
    m_ui->MsgEdit->setFocus();
    QTextCursor cursor = m_ui->MsgEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_ui->MsgEdit->setTextCursor(cursor);

    m_ui->ReceiveButton->setVisible(false);
    m_ui->SendButton->setText(QStringLiteral("Done"));
}

void ChatWidget::apply_appearance_settings()
{
    BOOST_LOG_TRIVIAL(info) << "Applying appearance settings in ChatWidget";
    m_message_item_delegate->update_settings();
    this->repaint_message_list();
}

void ChatWidget::repaint_message_list()
{
    BOOST_LOG_TRIVIAL(info) << "Repainting message list";
    QModelIndex cur_index = m_model_message_list->index(m_model_message_list->rowCount(QModelIndex()) - 1);
    m_model_message_list->setData(cur_index, QVariant(), MyRoles::RepaintRole);
}

}  // namespace melon::client_desktop
