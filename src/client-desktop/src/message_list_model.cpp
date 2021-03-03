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
    return index.isValid()?0:m_text_messages.count();
}

QVariant MessageListModel::data(const QModelIndex& index, int role) const
{
    QVariant data;

    if(index.isValid())
        switch(role)
        {
            case Qt::DisplayRole:
                data.setValue(m_text_messages[index.row()]);
                break;
            case Qt::UserRole:
                data.setValue(m_it_messages[index.row()]);
                break;
            case Qt::BackgroundRole:
                if (m_text_messages[index.row()] == QStringLiteral("I wish I could hear you."))
                    return M_RECEIVE_COLOR;
                break;
            default:
                break;
        }
    return data;
}

Qt::ItemFlags MessageListModel::flags(const QModelIndex& index) const
{
    return index.isValid()?Qt::ItemIsEnabled|Qt::ItemIsSelectable:Qt::ItemFlags();
}

bool MessageListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(index.isValid())
        switch(role)
        {
            case Qt::DisplayRole:
                m_text_messages[index.row()] = value.toString();
                emit dataChanged(index, index);
                return true;
            default:
                break;
        }

    return false;
}

void MessageListModel::add_message(message_handle_t message, const QModelIndex& parent)
{
    int row = this->rowCount() + 1;

    this->beginInsertRows(parent, row, row);
    m_text_messages.append(message->text());
    this->endInsertRows();
    m_it_messages.emplace_back(message);
}

void MessageListModel::delete_message(chat_handle_t it_chat, const QModelIndex &index, const QModelIndex& parent)
{
    int row = index.row();

    this->beginRemoveRows(parent, row, row);
    m_text_messages.remove(row);
    this->endRemoveRows();

    auto it_message = m_it_messages[row];
    it_chat->delete_message(it_message);

    m_it_messages.erase(m_it_messages.begin() + row);
}

void MessageListModel::set_external_message(const QModelIndex& index, const QString& message)
{
    auto it_msg = m_it_messages[index.row()];
    it_msg->set_text(message);
}

void MessageListModel::clear()
{
    this->beginRemoveRows(QModelIndex(), 1, m_text_messages.count());
    m_text_messages.clear();
    this->endRemoveRows();
}

}  // namespace melon::client_desktop
