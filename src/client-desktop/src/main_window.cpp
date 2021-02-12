#include <chat_widget.hpp>
#include <main_window.hpp>
#include <ram_storage.hpp>

#include <QInputDialog>
#include <QMessageBox>
#include <QSpacerItem>
#include <QWidget>

#include <stdexcept>

namespace melon::client_desktop
{

constexpr int MAX_NAME_CHAT_SIZE = 64;

void MainWindow::set_spacer()
{
    if (m_chat_widget != nullptr)
    {
        disconnect(m_ui->ChatList,
                   &QListWidget::currentItemChanged,
                   m_chat_widget,
                   &ChatWidget::update);

        m_ui->ChatPlace->removeWidget(m_chat_widget);
        delete m_chat_widget;
        m_chat_widget = nullptr;
    }

    m_spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_ui->ChatPlace->addSpacerItem(m_spacer);
}

void MainWindow::set_chat_widget()
{
    m_ui->ChatPlace->removeItem(m_spacer);
    delete m_spacer;
    m_spacer = nullptr;

    m_chat_widget = new ChatWidget();
    m_ui->ChatPlace->addWidget(m_chat_widget);

    connect(m_ui->ChatList,
            &QListWidget::currentItemChanged,
            m_chat_widget,
            &ChatWidget::update);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}
    , m_ui{new Ui::MainWindow}
{
    m_ui->setupUi(this);

    m_ui->ChatList->setContextMenuPolicy(Qt::CustomContextMenu);

    set_spacer();

    connect(m_ui->AddChatButton,
            &QPushButton::clicked,
            this,
            &MainWindow::add_chat);

    connect(m_ui->ChatList,
            &QWidget::customContextMenuRequested,
            this,
            &MainWindow::provide_chat_context_menu);
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

        if (!(ok && !text.isEmpty() && text.size() <= MAX_NAME_CHAT_SIZE))
            throw std::runtime_error("Name is incorrect");

        if (m_spacer != nullptr)
            set_chat_widget();

        auto* new_chat = new QListWidgetItem(text);

        m_ui->ChatList->addItem(new_chat);

        auto& ram_storage = RAMStorageSingletone::get_instance();

        auto added_chat = ram_storage.add_chat(Chat(text, static_cast<Chat::id_t>(counter), new_chat));

        QVariant pointer_to_chat;
        pointer_to_chat.setValue(added_chat);
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
    QPoint item = m_ui->ChatList->mapToGlobal(pos);
    auto* submenu = new QMenu(this);
    submenu->addAction(tr("Rename"), this, SLOT(rename_chat()));
    submenu->addAction(tr("Delete"), this, SLOT(delete_chat()));
    submenu->popup(item);
}

void MainWindow::delete_chat()
{
    QListWidgetItem* item = m_ui->ChatList->takeItem(m_ui->ChatList->currentRow());

    auto& ram_storage = RAMStorageSingletone::get_instance();

    auto it = ram_storage.chat_by_qlistitem(item);
    ram_storage.delete_chat(it);

    delete item;

    if (m_ui->ChatList->count() == 0)
        set_spacer();
}

void MainWindow::rename_chat()
{
    QListWidgetItem* item = m_ui->ChatList->currentItem();
    QString old_name = item->text();
    bool ok;

    try
    {
        QString text = QInputDialog::getText(this, tr("Type new name"),
                                             tr("Name of chat:"), QLineEdit::Normal,
                                             old_name, &ok);

        if (!(ok && !text.isEmpty() && text.size() <= MAX_NAME_CHAT_SIZE))
            throw std::runtime_error("Name is incorrect");

        auto& ram_storage = RAMStorageSingletone::get_instance();
        auto it = ram_storage.chat_by_qlistitem(item);
        it->name(text);

        item->setText(text);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Oops!"), QLatin1String(e.what()));
    }
}

}  // namespace melon::client_desktop
