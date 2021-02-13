#include <ram_storage.hpp>

#include <ui_chat_widget.h>

#include <chat_widget.hpp>

#include <QScrollBar>
#include<QMenuBar>

#include <chrono>

namespace melon::client_desktop
{

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget{parent}
    , m_ui{new Ui::ChatWidget}
{
    m_ui->setupUi(this);
    connect(m_ui->SendButton, &QPushButton::clicked, this, &ChatWidget::send_message);
    connect(m_ui->ReceiveButton, &QPushButton::clicked, this, &ChatWidget::receive_message);

    m_ui->MsgList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->MsgList,
            &QWidget::customContextMenuRequested,
            this,
            &ChatWidget::provide_chat_context_menu);
}

void ChatWidget::send_message()
{

    QString msg_text = m_ui->MsgEdit->toPlainText();
    if (msg_text.isEmpty())
        return;

    if (m_edit_mode)
    {
        m_edit_item->setText(msg_text);
        auto it_msg = m_current_chat_it->msg_by_qlistitem(m_edit_item);
        it_msg->set_text(msg_text);
        m_edit_mode = false;
        m_edit_item = nullptr;
        m_ui->MsgEdit->clear();
        return;
    }

    Chat::message_handle_t it_msg = m_current_chat_it->add_message(Message(QLatin1String("Me"),
                                                                           msg_text,
                                                                           {},
                                                                           std::chrono::high_resolution_clock::now()));

    auto* msg_item = new QListWidgetItem();
    msg_item->setText(msg_text);

    QVariant pointer_to_msg;
    pointer_to_msg.setValue(it_msg);
    msg_item->setData(Qt::UserRole, pointer_to_msg);

    m_ui->MsgEdit->clear();
    m_ui->MsgList->addItem(msg_item);
    m_ui->MsgList->scrollToBottom();
}

void ChatWidget::receive_message()
{
    QString msg_text = QStringLiteral("I wish I could hear you.");

    auto it_msg = m_current_chat_it->add_message(Message(QLatin1String("Some Sender"),
                                                                           msg_text,
                                                                           {},
                                                                           std::chrono::high_resolution_clock::now()));

    auto* msg_item = new QListWidgetItem();
    msg_item->setText(msg_text);
    msg_item->setTextAlignment(Qt::AlignLeft);
    QColor background_color(rgba_receive::R, rgba_receive::G,
                            rgba_receive::B, rgba_receive::A);
    msg_item->setBackground(background_color);

    QVariant pointer_to_msg;
    pointer_to_msg.setValue(it_msg);
    msg_item->setData(Qt::UserRole, pointer_to_msg);

    m_ui->MsgList->addItem(msg_item);
    m_ui->MsgList->scrollToBottom();
}

void ChatWidget::change_chat(QListWidgetItem* current_chat, QListWidgetItem* previous_chat)
{
    m_current_chat_item = current_chat;
    if (m_current_chat_item == nullptr)
        return;

    set_current_chat_it(chat_by_qlistitem(m_current_chat_item));

    if (previous_chat != nullptr)
    {
        auto it_previous = chat_by_qlistitem(previous_chat);
        it_previous->set_incomplete_message(capture_message_from_editor());
        it_previous->set_scrolling_position(m_ui->MsgList->verticalScrollBar()->value());
    }

    m_ui->MsgList->clear();

    std::list<Message>& messages = m_current_chat_it->messages();

    for (auto it_msg = messages.begin(); it_msg != messages.end(); ++it_msg)
    {
        QListWidgetItem* new_msg_item = load_message_into_item(*it_msg);

        m_ui->MsgList->addItem(new_msg_item);

        QVariant pointer_to_msg;
        pointer_to_msg.setValue(it_msg);
        new_msg_item->setData(Qt::UserRole, pointer_to_msg);
    }

    load_message_to_editor(m_current_chat_it->incomplete_message());
    int my_scroll_pos = m_current_chat_it->scrolling_position();
    m_ui->MsgList->verticalScrollBar()->setMaximum(my_scroll_pos);
    m_ui->MsgList->verticalScrollBar()->setValue(my_scroll_pos);
}

Message ChatWidget::capture_message_from_editor()
{
    QString msg_text = m_ui->MsgEdit->toPlainText();
    if (msg_text.isEmpty())
        return Message(QLatin1String("Me"), QLatin1String(""), {}, std::chrono::high_resolution_clock::now());

    return Message(QLatin1String("Me"), msg_text, {}, std::chrono::high_resolution_clock::now());
}

void ChatWidget::load_message_to_editor(const Message& msg)
{
    m_ui->MsgEdit->setText(msg.text());
}

QListWidgetItem* ChatWidget::load_message_into_item(const Message& msg)
{
    auto* msg_item = new QListWidgetItem();
    msg_item->setText(msg.text());

    if (msg.from() != QStringLiteral("Me"))
    {
        QColor background_color(rgba_receive::R, rgba_receive::G,
                                rgba_receive::B, rgba_receive::A);
        msg_item->setBackground(background_color);
    }
    return msg_item;
}

void ChatWidget::provide_chat_context_menu(const QPoint& pos)
{
    QPoint item = m_ui->MsgList->mapToGlobal(pos);
    auto* submenu = new QMenu(this);
    QListWidgetItem* cur_item = m_ui->MsgList->currentItem();

    if (m_current_chat_it->msg_by_qlistitem(cur_item)->from() == QStringLiteral("Me") )
    {
        submenu->addAction(tr("Edit"), this, SLOT(edit_msg()));
    }
    submenu->addAction(tr("Delete"), this, SLOT(delete_msg()));
    submenu->popup(item);
}

void ChatWidget::delete_msg()
{
    QListWidgetItem* item_msg = m_ui->MsgList->takeItem(m_ui->MsgList->currentRow());
    auto it_msg = m_current_chat_it->msg_by_qlistitem(item_msg);
    m_current_chat_it->delete_message(it_msg);

    delete item_msg;
}

void ChatWidget::edit_msg()
{
    QListWidgetItem* item = m_ui->MsgList->currentItem();
    auto it_msg = m_current_chat_it->msg_by_qlistitem(item);

    load_message_to_editor(*it_msg);
    m_edit_mode = true;
    m_edit_item = item;
}


}  // namespace melon::client_desktop
