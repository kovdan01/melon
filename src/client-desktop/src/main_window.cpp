#include <chat_widget.hpp>
#include <chat_widget_model.hpp>
#include <main_window.hpp>

#include <QInputDialog>
#include <QMessageBox>
#include <QWidget>

namespace melon::client_desktop
{

constexpr int MAX_NAME_CHAT_SIZE = 20;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}
    , m_ui{new Ui::MainWindow}
{
    m_ui->setupUi(this);
    m_ui->ChatList->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ui->ChatPlace->addWidget(new ChatWidget);
    m_ui->ChatList->setCurrentRow(0);
    connect(m_ui->AddChatButton, &QPushButton::clicked, this, &MainWindow::add_chat);
    connect(m_ui->ChatList, &QWidget::customContextMenuRequested,
            this, &MainWindow::provide_chat_context_menu);
}

void MainWindow::add_chat()
{
    static int counter = 1;
    bool ok;
    QString text = QInputDialog::getText(this, tr("Creating new chat"),
                                         tr("Name of chat:"), QLineEdit::Normal,
                                         tr("NewChat") + QString::number(counter), &ok);
   if (ok && !text.isEmpty() && text.size() <= MAX_NAME_CHAT_SIZE)
   {
       m_ui->ChatList->addItem(text);
       m_ui->ChatList->setCurrentRow(counter);
       //m_ui->ChatPlace->addWidget(new ChatWidget);
       ++counter;
   }
   else
   {
       QMessageBox::critical(this, tr("Oops!"), tr("Some problem with naming chat!"));
   }

   m_ui->ChatList->scrollToBottom();
}

void MainWindow::provide_chat_context_menu(const QPoint& pos)
{
    QPoint item = m_ui->ChatList->mapToGlobal(pos);
    if (m_ui->ChatList->currentRow() == 0)
    {
        auto* submenu = new QMenu(this);
        submenu->addAction(tr("Rename"), this, SLOT(rename_chat()));
        submenu->popup(item);
    }
    else
    {
        auto* submenu = new QMenu(this);
        submenu->addAction(tr("Rename"), this, SLOT(rename_chat()));
        submenu->addAction(tr("Delete"), this, SLOT(delete_chat()));
        submenu->popup(item);
    }
}

void MainWindow::delete_chat()
{
    int cur_row = m_ui->ChatList->currentRow();
    QListWidgetItem* item = m_ui->ChatList->takeItem(cur_row);
//    QWidget* chat = m_ui->ChatsWidgetStack->widget(cur_row);
//    m_ui->ChatsWidgetStack->removeWidget(chat);
//    delete chat;
    delete item;
}

void MainWindow::rename_chat()
{
    QListWidgetItem* item = m_ui->ChatList->currentItem();
    QString old_name = item->text();
    bool ok;
    QString text = QInputDialog::getText(this, tr("Type new name"),
                                         tr("Name of chat:"), QLineEdit::Normal,
                                         old_name, &ok);
    if (ok && !text.isEmpty() && text.size() <= MAX_NAME_CHAT_SIZE)
    {
        item->setText(text);
    }
    else
    {
        QMessageBox::critical(this, tr("Oops!"), tr("Some problem with renaming chat!"));
    }
}

}  // namespace melon::client_desktop
