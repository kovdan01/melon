#include <message_list_model.hpp>

namespace melon::client_desktop
{

MessageListModel::MessageListModel(QObject* parent)
    : QAbstractListModel{parent}
{}

int MessageListModel::rowCount(const QModelIndex& index) const
{
    return index.isValid()?0:m_text.count();
}

QVariant MessageListModel::data(const QModelIndex& index,int role) const
{
    QVariant data;

    if(index.isValid())
        switch(role)
        {
            case Qt::DisplayRole:
                data.setValue(m_text[index.row()]);
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
            m_text[index.row()] = value.toString();
                emit dataChanged(index, index);
                return true;
        }

    return false;
}

void MessageListModel::add_message(Message message, const QModelIndex& parent)
{
    int row = this->rowCount() + 1;

    this->beginInsertRows(parent, row, row);
    m_text.append(message.text());
    this->endInsertRows();
}

void MessageListModel::clear()
{
    this->beginRemoveRows(QModelIndex(), 1, m_text.count());
    m_text.clear();
    this->endRemoveRows();
}

}  // namespace melon::client_desktop
