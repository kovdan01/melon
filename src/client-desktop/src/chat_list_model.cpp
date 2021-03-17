#include <chat_list_model.hpp>
#include <ram_storage.hpp>

namespace melon::client_desktop
{

ChatListModel::ChatListModel(QObject* parent)
    : QAbstractListModel{parent}
{
}

int ChatListModel::rowCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 : static_cast<int>(m_it_chats.size());
}

QVariant ChatListModel::data(const QModelIndex& index, int role) const
{
    QVariant data;

    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            data.setValue(m_it_chats[index.row()]);
            break;
        case MyRoles::ChatNameRole:
            data.setValue(m_it_chats[index.row()]->name());
            break;
        default:
            break;
        }
    }
    return data;
}

Qt::ItemFlags ChatListModel::flags(const QModelIndex& index) const
{
    return index.isValid() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::ItemFlags();
}

bool ChatListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid())
    {
        switch(role)
        {
        case MyRoles::ChatNameRole:
            this->set_chat_name_in_ram_storage(index, value.toString());
            emit dataChanged(index, index);
            return true;
        case MyRoles::RepaintRole:
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

void ChatListModel::add_chat(const Chat& chat, const QModelIndex& parent)
{
    int row = this->rowCount(QModelIndex()) + 1;

    auto& ram_storage = RAMStorageSingletone::get_instance();

    auto it_added_chat = ram_storage.add_chat(chat);

    this->beginInsertRows(parent, row, row);
    m_it_chats.emplace_back(it_added_chat);
    this->endInsertRows();
}

void ChatListModel::delete_chat(const QModelIndex& index, const QModelIndex& parent)
{
    int row = index.row();

    auto it_chat = m_it_chats[row];

    this->beginRemoveRows(parent, row, row);
    m_it_chats.erase(m_it_chats.begin() + row);
    this->endRemoveRows();

    auto& ram_storage = RAMStorageSingletone::get_instance();
    ram_storage.delete_chat(it_chat);
}

void ChatListModel::set_chat_name_in_ram_storage(const QModelIndex &index, const QString &name)
{
    auto it_msg = m_it_chats[index.row()];
    it_msg->set_name(name);
}

ChatListModel::chat_handle_t ChatListModel::chat_it_by_index(const QModelIndex &index)
{
    int row = index.row();
    return m_it_chats[row];
}

}  // namespace melon::client_desktop
