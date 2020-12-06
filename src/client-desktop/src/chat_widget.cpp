#include "chat_widget.hpp"
#include "ui_chat_widget.h"


namespace melon::client_desktop
{

ChatWidget::ChatWidget(QWidget *parent) :
    QWidget{parent},
    m_ui{new Ui::ChatWidget}  //how to use QScopedPointer here?..
{
    m_ui->setupUi(this);
    connect(m_ui->SendButton, &QPushButton::clicked, this, &ChatWidget::send_message);
    connect(m_ui->ReceiveButton, &QPushButton::clicked, this, &ChatWidget::receive_message);
}

void ChatWidget::send_message()
{
    m_ui->MsgList->setWordWrap(true);
    QString msgText = m_ui->MsgEdit->toPlainText();
    if (!msgText.isEmpty())
    {
        QListWidgetItem *msgItem = new QListWidgetItem();
        msgItem->setText(msgText);
        m_ui->MsgEdit->clear();
        msgItem->setTextAlignment(Qt::AlignRight);
        //set width of item?..
        // and color it
        m_ui->MsgList->addItem(msgItem);
    }
}

void ChatWidget::receive_message()
{
    QString msgText = QStringLiteral("I wish I could hear you.");
    QListWidgetItem *msgItem = new QListWidgetItem();
    msgItem->setText(msgText);
    msgItem->setTextAlignment(Qt::AlignLeft);
    m_ui->MsgList->addItem(msgItem);
}

} //namespace melon::client_desktop
