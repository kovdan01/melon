#include <ram_storage.hpp>

#include <ui_chat_widget.h>

#include <chat_widget.hpp>

#include <QScrollBar>

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
}

void ChatWidget::send_message()
{
    QString msg_text = m_ui->MsgEdit->toPlainText();
    if (msg_text.isEmpty())
        return;

    auto& ram_storage = RAMStorageSingletone::get_instance();

    auto it = ram_storage.chat_by_qlistitem(m_current_chat);

    it->add_message(Message(QLatin1String("Me"), msg_text, {}, std::chrono::high_resolution_clock::now()));

    auto* msg_item = new QListWidgetItem();
    msg_item->setText(msg_text);
    m_ui->MsgEdit->clear();
    m_ui->MsgList->addItem(msg_item);
    m_ui->MsgList->scrollToBottom();
}

void ChatWidget::receive_message()
{
    QString msg_text = QStringLiteral("I wish I could hear you.");

    auto& ram_storage = RAMStorageSingletone::get_instance();
    auto it = ram_storage.chat_by_qlistitem(m_current_chat);
    it->add_message(Message(QLatin1String("Some Sender"), msg_text, {}, std::chrono::high_resolution_clock::now()));

    auto* msg_item = new QListWidgetItem();
    msg_item->setText(msg_text);
    msg_item->setTextAlignment(Qt::AlignLeft);
    QColor background_color(rgba_receive::R, rgba_receive::G,
                            rgba_receive::B, rgba_receive::A);
    msg_item->setBackground(background_color);
    m_ui->MsgList->addItem(msg_item);
    m_ui->MsgList->scrollToBottom();
}

void ChatWidget::update(QListWidgetItem* current_chat, QListWidgetItem* previous_chat)
{
    m_current_chat = current_chat;
    if (m_current_chat == nullptr)
        return;

    auto& ram_storage = RAMStorageSingletone::get_instance();

    if (previous_chat != nullptr)
    {
        auto it_previous = ram_storage.chat_by_qlistitem(previous_chat);
        it_previous->set_incomplete_message(capture_incomplete_message());
        it_previous->set_scrolling_position(m_ui->MsgList->verticalScrollBar()->value());
    }

    auto it_current = ram_storage.chat_by_qlistitem(m_current_chat);

    m_ui->MsgList->clear();

    for (const Message& message : it_current->messages())
        m_ui->MsgList->addItem(load_message_into_item(message));

    load_incomplete_message(it_current);
    int my_scroll_pos = it_current->scrolling_position();
    m_ui->MsgList->verticalScrollBar()->setMaximum(my_scroll_pos);
    m_ui->MsgList->verticalScrollBar()->setValue(my_scroll_pos);
}

Message ChatWidget::capture_incomplete_message()
{
    QString msg_text = m_ui->MsgEdit->toPlainText();
    if (msg_text.isEmpty())
        return Message(QLatin1String("Me"), QLatin1String(""), {}, std::chrono::high_resolution_clock::now());

    return Message(QLatin1String("Me"), msg_text, {}, std::chrono::high_resolution_clock::now());
}

void ChatWidget::load_incomplete_message(RAMStorageSingletone::chat_handle_t it)
{
    m_ui->MsgEdit->setText(it->incomplete_message().text());
}

QListWidgetItem* ChatWidget::load_message_into_item(Message msg)
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


}  // namespace melon::client_desktop
