#include "ui_chat_widget.h"

#include "chat_widget.hpp"

namespace melon::client_desktop
{

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget{parent}
    , m_ui{new Ui::ChatWidget}
{
    m_ui->setupUi(this);
    connect(m_ui->SendButton, &QPushButton::clicked, this, &ChatWidget::send_message);
    connect(m_ui->ReceiveButton, &QPushButton::clicked, this, &ChatWidget::receive_message);
}

void ChatWidget::send_message()
{
    //m_ui->MsgList->setWordWrap(true);
    QString msg_text = m_ui->MsgEdit->toPlainText();
    if (!msg_text.isEmpty())
    {
        auto *msg_item = new QListWidgetItem();
        msg_item->setText(msg_text);
        m_ui->MsgEdit->clear();
        //msg_item->setTextAlignment(Qt::AlignRight);
        // set width of item?..
        //m_ui->MsgList->setItemAlignment(Qt::AlignRight);
        m_ui->MsgList->addItem(msg_item);
        //m_ui->MsgList->currentItem()->setTextAlignment(Qt::AlignRight);
        m_ui->MsgList->scrollToBottom();
    }
}

void ChatWidget::receive_message()
{
    QString msg_text = QStringLiteral("I wish I could hear you.");
    auto *msg_item = new QListWidgetItem();
    msg_item->setText(msg_text);
    msg_item->setTextAlignment(Qt::AlignLeft);
    QColor background_color(250, 224, 180, 127);
    msg_item->setBackground(background_color);
    //m_ui->MsgList->setItemAlignment(Qt::AlignLeft);
    m_ui->MsgList->addItem(msg_item);
    m_ui->MsgList->scrollToBottom();
}

} // namespace melon::client_desktop
