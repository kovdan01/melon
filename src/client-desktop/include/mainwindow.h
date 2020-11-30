#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_AddChat_clicked();

//    void on_MenuList_customContextMenuRequested(const QPoint &pos);

    void eraseItem();

    void on_MenuList_itemDoubleClicked(QListWidgetItem *item);

    void on_SendButton_clicked();

    void on_ReceiveButton_clicked();

private:
    QScopedPointer<Ui::MainWindow> ui_;
};
#endif // MAINWINDOW_H
