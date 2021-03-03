#ifndef MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_

#include <QAbstractListModel>

#include <ram_storage.hpp>

namespace melon::client_desktop
{

class MessageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MessageListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& index = {}) const override;
    Qt::ItemFlags flags(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    void add_message(Message message, const QModelIndex& parent = QModelIndex());
    void clear();

private:
    QVector<QString> m_text;
};

}  // namespace melon::client_desktop

#endif // MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_
