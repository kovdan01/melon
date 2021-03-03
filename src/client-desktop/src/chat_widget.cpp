#include <ram_storage.hpp>

#include <ui_chat_widget.h>

#include <chat_widget.hpp>
#include <message_list_model.hpp>

#include <QScrollBar>
#include<QMenuBar>
#include <QMouseEvent>

#include <chrono>
#include <iostream>

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

    connect(m_ui->MsgList,
            &QWidget::customContextMenuRequested,
            this,
            &ChatWidget::provide_message_context_menu);

    m_submenu_sended_messages.addAction(tr("Edit"), this, SLOT(edit_message()));
    m_submenu_sended_messages.addAction(tr("Delete"), this, SLOT(delete_message()));
    m_submenu_received_messages.addAction(tr("Delete"), this, SLOT(delete_message()));

    m_ui->MsgEdit->installEventFilter(this);
}


bool ChatWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object != m_ui->MsgEdit)
        return object->eventFilter(object, event);

    if (event->type() == QEvent::KeyPress)
    {
        auto* event_key = static_cast<QKeyEvent*>(event);
        int key = event_key->key();
        m_pressed_keys += key;

        if ( (m_pressed_keys.contains(Qt::Key_Enter) || m_pressed_keys.contains(Qt::Key_Return))
              && m_pressed_keys.contains(Qt::Key_Shift) )
        {
            m_ui->MsgEdit->append(QString());
            return true;
        }

        if (m_pressed_keys.contains(Qt::Key_Enter) || m_pressed_keys.contains(Qt::Key_Return))
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
    message_text = message_text.trimmed();

    if (message_text.isEmpty())
        return;

    if (m_edit_mode)
    {
        auto index = m_model_message_list->index(m_edit_row);
        m_model_message_list->setData(index, message_text, Qt::DisplayRole);
        m_model_message_list->set_external_message(index, message_text);

        m_edit_mode = false;
        m_ui->MsgEdit->clear();
        m_ui->ReceiveButton->setVisible(true);
        m_ui->SendButton->setText(QStringLiteral("Send"));
        return;
    }

    Message new_message(QLatin1String("Me"),
                        message_text,
                        {},
                        std::chrono::high_resolution_clock::now());
    auto it_message = m_current_chat_it->add_message(new_message);

    m_ui->MsgEdit->clear();
    m_model_message_list->add_message(it_message);
    m_ui->MsgList->scrollToBottom();
}

void ChatWidget::receive_message()
{
    Message msg(QLatin1String("Some Sender"),
                QStringLiteral("I wish I could hear you."),
                {},
                std::chrono::high_resolution_clock::now());

    auto it_message = m_current_chat_it->add_message(msg);

    m_model_message_list->add_message(it_message);

    m_ui->MsgList->scrollToBottom();
}

void ChatWidget::change_chat(QListWidgetItem* current_chat, QListWidgetItem* previous_chat)
{
    m_current_chat_item = current_chat;
    if (m_current_chat_item == nullptr)
        return;

    set_current_chat_it(it_by_qlistitem<chat_handle_t>(m_current_chat_item));

    if (previous_chat != nullptr)
    {
        auto it_previous = it_by_qlistitem<chat_handle_t>(previous_chat);
        it_previous->set_incomplete_message(capture_message_from_editor());
        it_previous->set_scrolling_position(m_ui->MsgList->verticalScrollBar()->value());
    }

    m_model_message_list->clear();

    std::list<Message>& messages = m_current_chat_it->messages();

    for (auto it_message = messages.begin(); it_message != messages.end(); ++it_message)
    {
        m_model_message_list->add_message(it_message);
    }

    this->load_message_to_editor(m_current_chat_it->incomplete_message());
    int my_scroll_pos = m_current_chat_it->scrolling_position();
    m_ui->MsgList->verticalScrollBar()->setMaximum(my_scroll_pos);
    m_ui->MsgList->verticalScrollBar()->setValue(my_scroll_pos);
}

Message ChatWidget::capture_message_from_editor()
{
    QString message_text = m_ui->MsgEdit->toPlainText();
    if (message_text.isEmpty())
        return Message(QLatin1String("Me"), QLatin1String(""), {}, std::chrono::high_resolution_clock::now());

    return Message(QLatin1String("Me"), message_text, {}, std::chrono::high_resolution_clock::now());
}

void ChatWidget::load_message_to_editor(const Message& message)
{
    m_ui->MsgEdit->setText(message.text());
}

void ChatWidget::provide_message_context_menu(const QPoint& pos)
{
    auto index = m_ui->MsgList->selectionModel()->currentIndex();
    if (!index.isValid())
        return;

    if (this->m_model_message_list->data(index, Qt::DisplayRole) != QStringLiteral("I wish I could hear you."))
    {
        m_submenu_sended_messages.popup(m_ui->MsgList->mapToGlobal(pos));
    }
    else
    {
        m_submenu_received_messages.popup(m_ui->MsgList->mapToGlobal(pos));
    }
}

void ChatWidget::delete_message()
{
    auto index = m_ui->MsgList->selectionModel()->currentIndex();

    m_model_message_list->delete_message(m_current_chat_it, index);
}

void ChatWidget::edit_message()
{
    auto index = m_ui->MsgList->selectionModel()->currentIndex();

    m_edit_mode = true;
    m_edit_row = index.row();

    auto message_text = m_model_message_list->data(index, Qt::DisplayRole);

    m_ui->MsgEdit->setText(message_text.toString());

    m_ui->ReceiveButton->setVisible(false);
    m_ui->SendButton->setText(QStringLiteral("Done"));
}

}  // namespace melon::client_desktop
