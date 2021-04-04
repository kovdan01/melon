#include <chat_item_delegate.hpp>
#include <chat_list_model.hpp>
#include <chat_widget.hpp>
#include <db_storage.hpp>
#include <main_window.hpp>
#include <melon/core/exception.hpp>

#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSpacerItem>
#include <QSplitter>
#include <QWidget>

#include <cassert>
#include <stdexcept>
#include <iostream>

namespace melon::client_desktop
{

constexpr int MAX_CHAT_NAME_SIZE = 64;

void MainWindow::replace_chat_widget_with_spacer()
{
    if (m_chat_widget != nullptr)
    {
        disconnect(m_ui->ChatList->selectionModel(),
                   &QItemSelectionModel::currentChanged,
                   this,
                   &MainWindow::change_chat);

        disconnect(m_chat_widget,
                   &ChatWidget::last_message_changed,
                   this,
                   &MainWindow::repaint_chat_list);

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

    m_chat_widget = new ChatWidget(this);
    m_ui->ChatPlace->addWidget(m_chat_widget);

    connect(m_ui->ChatList->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &MainWindow::change_chat);

    connect(m_chat_widget,
            &ChatWidget::last_message_changed,
            this,
            &MainWindow::repaint_chat_list);
}

void MainWindow::load_data_from_database()
{
    auto& storage = DBSingletone::get_instance();

    QString db_name = storage.db_name();
    QSqlQuery qry_for_chats(db_name);
    QSqlQuery qry_for_messages(db_name);
    if (!qry_for_chats.exec(QStringLiteral("SELECT chat_id, domain_id FROM chats")))
    {
        std::cout << "Error while loading chats: " << std::flush;
        std::cout << qry_for_chats.lastError().text().toStdString() << std::endl;
        return;
    }
    while (qry_for_chats.next())
    {
        auto chat_id = qry_for_chats.value(0).value<std::uint64_t>();
        auto domain_id = qry_for_chats.value(1).value<std::uint64_t>();
        Chat chat(chat_id, domain_id);

        QString qry_str = QStringLiteral("SELECT message_id FROM messages WHERE chat_id=") +QString::number(chat_id)
                         + QStringLiteral(" and domain_id_chat=") + QString::number(domain_id);
        if (!qry_for_messages.exec(qry_str))
        {
            std::cout << "Error while loading messages: " << std::flush;
            std::cout << qry_for_messages.lastError().text().toStdString() << std::endl;
            return;
        }
        while (qry_for_messages.next())
        {
            auto message_id = qry_for_messages.value(0).value<std::uint64_t>();
            Message message(message_id, chat_id, domain_id);
            chat.add_message(message);
        }

        m_model_chat_list->add_chat(chat);
    }
    m_ui->ChatList->setCurrentIndex(m_model_chat_list->index(0));
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}
    , m_ui{new Ui::MainWindow}
{
    m_ui->setupUi(this);

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

    m_chat_item_delegate = new ChatItemDelegate{m_ui->ChatList};
    m_ui->ChatList->setItemDelegate(m_chat_item_delegate);

    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    qry.exec(QStringLiteral("SELECT COUNT(chat_id) from chats"));
    qry.next();
    if (qry.value(0).toInt() > 0)
    {
        this->replace_spacer_with_chat_widget();
        this->load_data_from_database();
    }
    else
    {
        this->replace_chat_widget_with_spacer();
    }
}

namespace
{

class ChatNameException : public melon::Exception
{
public:
    using melon::Exception::Exception;
};

}  // namespace

static void check_chat_name(const QString& text)
{
    if (text.isEmpty() || text.size() > MAX_CHAT_NAME_SIZE)
        throw ChatNameException("Name is incorrect");
}

void MainWindow::add_chat()
{
    static int counter = 0;
    bool ok;

    try
    {
        QString name = QInputDialog::getText(this, tr("Creating new chat"),
                                             tr("Name of chat:"), QLineEdit::Normal,
                                             tr("NewChat") + QString::number(counter), &ok);
        if (!ok)
            return;

        check_chat_name(name);

        if (m_spacer != nullptr)
            this->replace_spacer_with_chat_widget();

        auto& storage = UserDomainSingletone::get_instance();
        Chat chat(storage.my_domain().domain_id(), name);
        m_model_chat_list->add_chat(chat);
        int cur_chat_row = m_model_chat_list->rowCount(QModelIndex()) - 1;
        QModelIndex cur_index = m_model_chat_list->index(cur_chat_row);
        m_ui->ChatList->setCurrentIndex(cur_index);

        ++counter;
    }
    catch (const ChatNameException& e)
    {
        QMessageBox::critical(this, tr("Oops!"), QLatin1String(e.what()));
    }

    m_ui->ChatList->scrollToBottom();
}


void MainWindow::provide_chat_context_menu(const QPoint& pos)
{
    QModelIndex cur_index = m_ui->ChatList->indexAt(pos);
    if (!cur_index.isValid())
        return;

    m_requested_menu_position = pos;
    m_submenu.popup(m_ui->ChatList->mapToGlobal(pos));
}

void MainWindow::delete_chat()
{
    QModelIndex cur_index = m_ui->ChatList->indexAt(m_requested_menu_position);
    assert(cur_index.isValid());

    m_model_chat_list->delete_chat(cur_index);

    if (m_model_chat_list->rowCount(QModelIndex()) == 0)
        this->replace_chat_widget_with_spacer();
}

void MainWindow::rename_chat()
{
    try
    {
        bool ok;
        QModelIndex cur_index = m_ui->ChatList->indexAt(m_requested_menu_position);

        QString old_name = m_model_chat_list->data(cur_index, MyRoles::ChatNameRole).toString();

        QString text = QInputDialog::getText(this, tr("Type new name"),
                                             tr("Name of chat:"), QLineEdit::Normal,
                                             old_name, &ok);
        if (!ok)
            return;

        check_chat_name(text);

        if (old_name == text)
            return;

        m_model_chat_list->setData(cur_index, text, MyRoles::ChatNameRole);
    }
    catch (const ChatNameException& e)
    {
        QMessageBox::critical(this, tr("Oops!"), QLatin1String(e.what()));
    }
}

void MainWindow::repaint_chat_list()
{
    QModelIndex cur_index = m_ui->ChatList->currentIndex();
    m_model_chat_list->setData(cur_index, QVariant(), MyRoles::RepaintRole);
}

void MainWindow::change_chat(const QModelIndex& current_chat, const QModelIndex& previous_chat)
{
    if (!current_chat.isValid())
        return;

    auto current_it = m_model_chat_list->chat_it_by_index(current_chat);
    if (!previous_chat.isValid())
    {
        m_chat_widget->change_chat(current_it);
        return;
    }

    auto previous_it = m_model_chat_list->chat_it_by_index(previous_chat);
    m_chat_widget->change_chat(current_it, previous_it);
}

}  // namespace melon::client_desktop
