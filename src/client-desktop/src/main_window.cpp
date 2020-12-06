#include "chat_widget.hpp"
#include "main_window.hpp"
#include <QInputDialog>
#include <QMessageBox>
#include <QWidget>

namespace melon::client_desktop
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}
    , m_ui{QScopedPointer<Ui::MainWindow>()}
{
    m_ui->setupUi(this);
    connect(m_ui->AddChatButton, &QPushButton::clicked, this, &MainWindow::add_chat);
}

void MainWindow::add_chat()
{
    static uint counter = 0;
    bool ok;
    QString text = QInputDialog::getText(this, tr("Creating new chat"),
                                         tr("Name of chat:"), QLineEdit::Normal,
                                         tr("NewChat") + QString::number(counter), &ok);
   if (ok && !text.isEmpty())
   {
       m_ui->MenuList->addItem(text);
       m_ui->statusbar->showMessage(tr("Added Chat"));
       m_ui->ChatsWidgetStack->addWidget(new ChatWidget);
       ++counter;
   }
   else
   {
       QMessageBox::critical(this, tr("Oops!"), tr("Some problem with naming chat!"));
   }

}

//void MainWindow::eraseItem()
//{
//    QListWidgetItem *item = m_ui->MenuList->takeItem(m_ui->MenuList->currentRow());


//    delete item;
//}

//void MainWindow::on_MenuList_itemDoubleClicked(QListWidgetItem *item)
//{
//    delete item;
//}

}  // namespace melon::client_desktop
