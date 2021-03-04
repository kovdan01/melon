#include <chat_widget.hpp>
#include <chat_list_widget.hpp>
#include <main_window.hpp>
#include <ram_storage.hpp>

#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSpacerItem>
#include <QSplitter>
#include <QWidget>

#include <stdexcept>
#include <iostream>

namespace melon::client_desktop
{

constexpr int MAX_NAME_CHAT_SIZE = 64;

void MainWindow::replace_chat_widget_with_spacer()
{
    if (m_chat_widget != nullptr)
    {
//        disconnect(m_ui->ChatList,
//                   &m_ui->ChatList->selectionModel()->currentChanged(),
//                   m_chat_widget,
//                   &ChatWidget::change_chat);

        m_ui->ChatPlace->removeWidget(m_chat_widget);
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

    connect(m_ui->ChatList->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &MainWindow::change_chat);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}
    , m_submenu(QMenu(this))
    , m_ui{new Ui::MainWindow}
{
    m_ui->setupUi(this);

    this->replace_chat_widget_with_spacer();

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

    m_ui->ChatList->setModel(m_model_chat_list);
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

        Chat chat(text, static_cast<Chat::id_t>(counter));

        m_model_chat_list->add_chat(chat);
        auto cur_chat_row = m_model_chat_list->rowCount(QModelIndex()) - 1;
        auto cur_index = m_model_chat_list->index(cur_chat_row);
        m_ui->ChatList->setCurrentIndex(cur_index);

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
    auto cur_index = m_ui->ChatList->indexAt(pos);
    if (!cur_index.isValid())
        return;

    m_requested_menu_position = pos;
    m_submenu.popup(m_ui->ChatList->mapToGlobal(pos));
}

void MainWindow::delete_chat()
{
    auto cur_index = m_ui->ChatList->indexAt(m_requested_menu_position);

    m_model_chat_list->delete_chat(cur_index);

    if (m_model_chat_list->rowCount(QModelIndex()) == 0)
        replace_chat_widget_with_spacer();
}

void MainWindow::rename_chat()
{
    auto cur_index = m_ui->ChatList->indexAt(m_requested_menu_position);

    QString old_name = m_model_chat_list->data(cur_index, Qt::DisplayRole).toString();
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

        m_model_chat_list->setData(cur_index, text, Qt::DisplayRole);
        m_model_chat_list->set_external_chat(cur_index, text);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Oops!"), QLatin1String(e.what()));
    }
}

void MainWindow::change_chat(const QModelIndex &current_chat, const QModelIndex &previous_chat)
{
    if (!current_chat.isValid())
        return;

    auto current_it = m_model_chat_list->chat_it_by_index(current_chat);

    if (!previous_chat.isValid())
    {
        m_chat_widget->change_chat(current_it, current_it, false);
        return;
    }

    auto previous_it = m_model_chat_list->chat_it_by_index(previous_chat);

    m_chat_widget->change_chat(current_it, previous_it, true);
}

}  // namespace melon::client_desktop
