#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QListWidget>

MainWindow::MainWindow(QWidget* parent)
    : QWidget{parent},
      ui_{new Ui::MainWindow}
{
    ui_->setupUi(this);
}

MainWindow::~MainWindow() = default;

void MainWindow::on_AddChat_clicked()
{
    static uint counter = 0;
    ui_->MenuList->addItem("NewChat" + QString::number(counter));
    ui_->statusbar->showMessage("Added Chat");
    ++counter;
}


//void MainWindow::on_MenuList_customContextMenuRequested(const QPoint &pos)
//{
//    ui_->MenuList->setContextMenuPolicy(Qt::CustomContextMenu);
//    QPoint globalPos = ui_->MenuList->mapToGlobal(pos);
//    ui_->statusbar->showMessage("Work");

//        // Create menu and insert some actions
//    QMenu myMenu;
//    myMenu.addAction("Insert", this, SLOT(addItem()));
//    myMenu.addAction("Erase",  this, SLOT(eraseItem()));

//    QModelIndex t = ui_->MenuList->indexAt(pos);
//    ui_->MenuList->item(t.row())->setSelected(true);			// even a right click will select the item
//        // Show context menu at handling position
//    myMenu.exec(globalPos);

//}

void MainWindow::eraseItem()
{
    QListWidgetItem *item = ui_->MenuList->takeItem(ui_->MenuList->currentRow());

    delete item;
}

void MainWindow::on_MenuList_itemDoubleClicked(QListWidgetItem *item)
{
    delete item;
}

void MainWindow::on_SendButton_clicked()
{
    ui->MsgList->setWordWrap(0); //???
    QString msgText = ui->MsgEdit->toPlainText();
    if (msgText != "")
    {
    QListWidgetItem *msgItem = new QListWidgetItem();
    msgItem->setText(msgText);
    ui->MsgEdit->clear();
    msgItem->setTextAlignment(Qt::AlignRight);
    ui->MsgList->addItem(msgItem);
    }
}

void MainWindow::on_ReceiveButton_clicked()
{
    QString msgText = "I wish I could hear you.";
    QListWidgetItem *msgItem = new QListWidgetItem();
    msgItem->setText(msgText);
    msgItem->setTextAlignment(Qt::AlignLeft);
    ui->MsgList->addItem(msgItem);
}
