#ifndef MELON_CLIENT_DESKTOP_CHAT_WIDGET_MODEL_HPP_
#define MELON_CLIENT_DESKTOP_CHAT_WIDGET_MODEL_HPP_

#include <QtSql/QSqlTableModel>

namespace melon::client_desktop
{

class ChatWidgetModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit ChatWidgetModel(QObject* parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index,int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index,const QVariant& value,int role = Qt::EditRole) override;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_CHAT_WIDGET_MODEL_HPP_
