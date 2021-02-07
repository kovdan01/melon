#include <ram_storage.hpp>

#include <ui_chat_widget.h>

#include <chat_widget.hpp>

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

void ChatWidget::update(QListWidgetItem* current_chat, QListWidgetItem* /* previous_chat */)
{
    m_current_chat = current_chat;
    if (m_current_chat == nullptr)
        return;

    auto& ram_storage = RAMStorageSingletone::get_instance();
    auto it = ram_storage.chat_by_qlistitem(m_current_chat);

    m_ui->MsgList->clear();

    for (const Message& message : it->messages())
        m_ui->MsgList->addItem(message.text());
}


}  // namespace melon::client_desktop
