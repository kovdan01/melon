#include <chat_widget.hpp>
#include <chat_list_widget.hpp>
#include <main_window.hpp>
#include <ram_storage.hpp>

#include <QInputDialog>
#include <QMessageBox>
#include <QSpacerItem>
#include <QWidget>
#include <QMouseEvent>
#include <QSplitter>

#include <stdexcept>
#include <iostream>

namespace melon::client_desktop
{

constexpr int MAX_NAME_CHAT_SIZE = 64;

void MainWindow::replace_chat_widget_with_spacer()
{
    if (m_chat_widget != nullptr)
    {
        disconnect(m_ui->ChatList,
                   &QListWidget::currentItemChanged,
                   m_chat_widget,
                   &ChatWidget::change_chat);

        //m_ui->ChatPlace->removeWidget(m_chat_widget);
        delete m_chat_widget;
        m_chat_widget = nullptr;
    }

    m_spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_ui->ChatPlace->addSpacerItem(m_spacer);
}

void MainWindow::replace_spacer_with_chat_widget()
{
    m_ui->ChatPlace->removeItem(m_spacer);
    delete m_spacer;
    m_spacer = nullptr;

    m_chat_widget = new ChatWidget();
    m_ui->ChatPlace->addWidget(m_chat_widget);

    connect(m_ui->ChatList,
            &QListWidget::currentItemChanged,
            m_chat_widget,
            &ChatWidget::change_chat);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}
    , m_submenu(QMenu(this))
    , m_ui{new Ui::MainWindow}
{
    m_ui->setupUi(this);

    replace_chat_widget_with_spacer();

    connect(m_ui->AddChatButton,
            &QPushButton::clicked,
            this,
            &MainWindow::add_chat);

    connect(m_ui->ChatList,
            &QWidget::customContextMenuRequested,
            this,
            &MainWindow::provide_chat_context_menu);

    m_submenu.addAction(tr("Rename"), this, SLOT(rename_chat()));
    m_submenu.addAction(tr("Delete"), this, SLOT(delete_chat()));
}


void MainWindow::add_chat()
{
    static int counter = 0;
    bool ok;

    try
    {
        QString text = QInputDialog::getText(this, tr("Creating new chat"),
                                             tr("Name of chat:"), QLineEdit::Normal,
                                             tr("NewChat") + QString::number(counter), &ok);

        if (!ok)
            return;

        if (text.isEmpty() && text.size() > MAX_NAME_CHAT_SIZE)
            throw std::runtime_error("Name is incorrect");

        if (m_spacer != nullptr)
            replace_spacer_with_chat_widget();

        auto* new_chat = new QListWidgetItem(text);

        m_ui->ChatList->addItem(new_chat);

        auto& ram_storage = RAMStorageSingletone::get_instance();

        auto it_added_chat = ram_storage.add_chat(Chat(text, static_cast<Chat::id_t>(counter)));

        QVariant pointer_to_chat;
        pointer_to_chat.setValue(it_added_chat);
        new_chat->setData(Qt::UserRole, pointer_to_chat);

        m_ui->ChatList->setCurrentItem(new_chat);

        ++counter;
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Oops!"), QLatin1String(e.what()));
    }

    m_ui->ChatList->scrollToBottom();
}


void MainWindow::provide_chat_context_menu(const QPoint& pos)
{
    if (m_ui->ChatList->itemAt(pos) == nullptr)
        return;

    m_requested_menu_position = pos;
    m_submenu.popup(m_ui->ChatList->mapToGlobal(pos));
}

void MainWindow::delete_chat()
{
    QListWidgetItem* item_by_pos = m_ui->ChatList->itemAt(m_requested_menu_position);
    QListWidgetItem* item = m_ui->ChatList->takeItem(m_ui->ChatList->row(item_by_pos));


    auto& ram_storage = RAMStorageSingletone::get_instance();

    auto it = item->data(Qt::UserRole).value<std::list<Chat>::iterator>();

    ram_storage.delete_chat(it);

    delete item;

    if (m_ui->ChatList->count() == 0)
        replace_chat_widget_with_spacer();
}

void MainWindow::rename_chat()
{
    QListWidgetItem* item = m_ui->ChatList->itemAt(m_requested_menu_position);

    QString old_name = item->text();
    bool ok;

    try
    {
        QString text = QInputDialog::getText(this, tr("Type new name"),
                                             tr("Name of chat:"), QLineEdit::Normal,
                                             old_name, &ok);
        if (!ok)
            return;

        if (text.isEmpty() && text.size() > MAX_NAME_CHAT_SIZE)
            throw std::runtime_error("Name is incorrect");

        auto it = item->data(Qt::UserRole).value<std::list<Chat>::iterator>();

        it->set_name(text);

        item->setText(text);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Oops!"), QLatin1String(e.what()));
    }
}

}  // namespace melon::client_desktop
