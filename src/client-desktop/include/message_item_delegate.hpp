#ifndef MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_

#include <message_list_model.hpp>

#include <QStyledItemDelegate>
#include <QTextDocument>

namespace melon::client_desktop
{

class MessageItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using MyRoles = MessageListModel::MyRoles;

    explicit MessageItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    float scale_message_length = 1.5;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_
