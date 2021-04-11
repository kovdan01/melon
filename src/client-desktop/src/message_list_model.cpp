#include <config.hpp>
#include <helpers.hpp>
#include <message_list_model.hpp>

#include <iterator>

namespace melon::client_desktop
{

MessageListModel::MessageListModel(QObject* parent)
    : QAbstractListModel{parent}
{
}

int MessageListModel::rowCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 : to_int(m_it_messages.size());
}

QVariant MessageListModel::data(const QModelIndex& index, int role) const
{
    QVariant data;

    if (index.isValid())
    {
        std::size_t row = to_size_t(index.row());
        switch (role)
        {
        case Qt::DisplayRole:
            data.setValue(m_it_messages[row]);
            break;
        case MyRoles::MessageTextRole:
            data.setValue(m_it_messages[row]->text());
            break;
        case MyRoles::AreIconAndSendernameNeededRole:
            return this->are_icon_and_sendername_needed(row - 1, row);
        case MyRoles::IsEditRole:
            data.setValue(m_it_messages[row]->is_edit());
            break;
        default:
            break;
        }
    }
    return data;
}

Qt::ItemFlags MessageListModel::flags(const QModelIndex& index) const
{
    return index.isValid() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::ItemFlags();
}

bool MessageListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            this->set_message_in_ram_storage(index, value.toString());
            emit dataChanged(index, index);
            return true;
        case MyRoles::IsEditRole:
            emit dataChanged(index, index);
            return true;
        default:
            break;
        }
    }
    return false;
}

void MessageListModel::add_message(chat_handle_t it_chat, const Message& message)
{
    int row = this->rowCount(QModelIndex()) + 1;

    auto it_message = this->add_message_to_ram_storage(it_chat, message);

    this->beginInsertRows(QModelIndex(), row, row);
    m_it_messages.emplace_back(it_message);
    this->endInsertRows();
    emit this->dataChanged(QModelIndex(), QModelIndex());
}

void MessageListModel::load_message(message_handle_t it_message)
{
    int row = this->rowCount(QModelIndex()) + 1;

    this->beginInsertRows(QModelIndex(), row, row);
    m_it_messages.emplace_back(it_message);
    this->endInsertRows();
}


void MessageListModel::delete_message(chat_handle_t it_chat, const QModelIndex& index, const QModelIndex& parent)
{
    int row = index.row();

    auto it_message = m_it_messages[to_size_t(row)];

    this->beginRemoveRows(parent, row, row);
    m_it_messages.erase(m_it_messages.begin() + row);
    this->endRemoveRows();

    it_chat->delete_message(it_message);
}

MessageListModel::message_handle_t MessageListModel::add_message_to_ram_storage(chat_handle_t it_chat, const Message& message)
{
    return it_chat->add_message(message);
}

void MessageListModel::set_message_in_ram_storage(const QModelIndex& index, const QString& message)
{
    m_it_messages[to_size_t(index.row())]->set_text(message);
}

bool MessageListModel::are_icon_and_sendername_needed(std::size_t less_row, std::size_t bigger_row) const
{
    if (less_row == std::size_t(-1) || bigger_row >= m_it_messages.size())
        return false;
    auto message1 = m_it_messages[less_row];
    auto message2 = m_it_messages[bigger_row];

    auto diff_time = std::chrono::duration_cast<std::chrono::minutes>(message2->timestamp() - message1->timestamp());

    return (message1->user_id() == message2->user_id() && message1->domain_id_user() == message2->domain_id_user() && diff_time.count() < 2);
}

void MessageListModel::clear()
{
    this->beginRemoveRows(QModelIndex(), 1, to_int(m_it_messages.size()));
    m_it_messages.clear();
    this->endRemoveRows();
}

}  // namespace melon::client_desktop
