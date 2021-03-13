#include <config.hpp>
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
    return index.isValid() ? 0 : static_cast<int>(m_it_messages.size());
}

QVariant MessageListModel::data(const QModelIndex& index, int role) const
{
    QVariant data;

    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            data.setValue(m_it_messages[index.row()]);
            break;
        case MyRoles::MessageTextRole:
            data.setValue(m_it_messages[index.row()]->text());
            break;
        case MyRoles::IsPreviousSameSenderAndTimeRole:
            return this->has_messages_same_sender_and_time(index.row() - 1, index.row());
        case MyRoles::IsNextSameSenderAndTimeRole:
            return this->has_messages_same_sender_and_time(index.row(), index.row() + 1);
        case MyRoles::IsEditRole:
            data.setValue(m_it_messages[index.row()]->is_edit());
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
            m_it_messages[index.row()]->set_is_edit(value.toBool());
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


void MessageListModel::delete_message(chat_handle_t it_chat, const QModelIndex &index, const QModelIndex& parent)
{
    int row = index.row();

    auto it_message = m_it_messages[row];

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
    auto it_msg = m_it_messages[index.row()];
    it_msg->set_text(message);
}

bool MessageListModel::has_messages_same_sender_and_time(const int& less_row, const int& bigger_row) const
{
    if (less_row < 0 || bigger_row >= static_cast<int>(m_it_messages.size()))
        return false;
    auto message1 = m_it_messages[less_row];
    auto message2 = m_it_messages[bigger_row];

    auto diff_time = std::chrono::duration_cast<std::chrono::minutes>(message2->timestamp() - message1->timestamp());

    return ((message1->from() == message2->from()) && (diff_time.count() < 2));
}

void MessageListModel::clear()
{
    this->beginRemoveRows(QModelIndex(), 1, static_cast<int>(m_it_messages.size()));
    m_it_messages.clear();
    this->endRemoveRows();
}

}  // namespace melon::client_desktop
