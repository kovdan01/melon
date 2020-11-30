#include "main_window.hpp"

namespace melon::client_desktop
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(std::make_unique<Ui::MainWindow>())
{
    m_ui->setupUi(this);
}

void MainWindow::on_AddChat_clicked()
{
    static uint counter = 0;
    m_ui->MenuList->addItem("NewChat" + QString::number(counter));
    m_ui->statusbar->showMessage("Added Chat");
    ++counter;
}


//void MainWindow::on_MenuList_customContextMenuRequested(const QPoint &pos)
//{
//    m_ui->MenuList->setContextMenuPolicy(Qt::CustomContextMenu);
//    QPoint globalPos = m_ui->MenuList->mapToGlobal(pos);
//    m_ui->statusbar->showMessage("Work");

//        // Create menu and insert some actions
//    QMenu myMenu;
//    myMenu.addAction("Insert", this, SLOT(addItem()));
//    myMenu.addAction("Erase",  this, SLOT(eraseItem()));

//    QModelIndex t = m_ui->MenuList->indexAt(pos);
//    m_ui->MenuList->item(t.row())->setSelected(true);			// even a right click will select the item
//        // Show context menu at handling position
//    myMenu.exec(globalPos);

//}

void MainWindow::eraseItem()
{
    QListWidgetItem *item = m_ui->MenuList->takeItem(m_ui->MenuList->currentRow());

    delete item;
}

void MainWindow::on_MenuList_itemDoubleClicked(QListWidgetItem *item)
{
    delete item;
}

void MainWindow::on_SendButton_clicked()
{
    m_ui->MsgList->setWordWrap(0); //???
    QString msgText = m_ui->MsgEdit->toPlainText();
    if (msgText != "")
    {
    QListWidgetItem *msgItem = new QListWidgetItem();
    msgItem->setText(msgText);
    m_ui->MsgEdit->clear();
    msgItem->setTextAlignment(Qt::AlignRight);
    m_ui->MsgList->addItem(msgItem);
    }
}

void MainWindow::on_ReceiveButton_clicked()
{
    QString msgText = "I wish I could hear you.";
    QListWidgetItem *msgItem = new QListWidgetItem();
    msgItem->setText(msgText);
    msgItem->setTextAlignment(Qt::AlignLeft);
    m_ui->MsgList->addItem(msgItem);
}

}  // namespace melon::client_desktop
