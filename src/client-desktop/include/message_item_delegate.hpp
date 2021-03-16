#ifndef MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_

#include <config.hpp>
#include <message_list_model.hpp>

#include <QStyledItemDelegate>
#include <QTextDocument>

#include <vector>

namespace melon::client_desktop
{

class MessageItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using MyRoles = MessageListModel::MyRoles;

    explicit MessageItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    [[nodiscard]] QString date_number_handler(const int& num) const;

    std::vector<QString> m_month_names = { tr("Jan"), tr("Feb"), tr("Mar"), tr("Apr"),
                                           tr("May"), tr("Jun"), tr("Jul"), tr("Aug"),
                                           tr("Sep"), tr("Oct"), tr("Nov"), tr("Dec") };

    QFont m_sender_font;
    QFont m_message_text_font;
    QFont m_timestamp_font;
    QColor m_sended_message_color;
    QColor m_receive_message_color;
    QColor m_selected_message_color;

    int m_min_message_width;
    qreal m_scale_message_width;
    int m_base_margin;
    int m_icon_diameter;
    int m_message_round_radius;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_
