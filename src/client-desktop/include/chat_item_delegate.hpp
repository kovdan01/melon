#ifndef MELON_CLIENT_DESKTOP_CHAT_ITEM_DELEGATE_HPP_
#define MELON_CLIENT_DESKTOP_CHAT_ITEM_DELEGATE_HPP_

#include <chat_list_model.hpp>

#include <QStyledItemDelegate>

namespace melon::client_desktop
{

class ChatItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using MyRoles = ChatListModel::MyRoles;

    explicit ChatItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;

    void update_settings();

private:
    [[nodiscard]] QString date_number_handler(const int& num) const;
    [[nodiscard]] QString date_handler(const Message::timestamp_t& timestamp) const;

    std::vector<QString> m_month_names = { tr("Jan"), tr("Feb"), tr("Mar"), tr("Apr"),
                                           tr("May"), tr("Jun"), tr("Jul"), tr("Aug"),
                                           tr("Sep"), tr("Oct"), tr("Nov"), tr("Dec") };
    QFont m_chat_name_font;
    QFont m_timestamp_font;
    QFont m_last_message_font;
    QFont m_unread_counter_font;
    QFont m_sender_font;

    /* since QFontMetrics can not be initialized after creation,
       but all members of the class must be initialized in constructor,
       we put here fake initialization */
    QFontMetrics m_fm_chat_name = QFontMetrics(QFont());
    QFontMetrics m_fm_timestamp = QFontMetrics(QFont());
    QFontMetrics m_fm_last_message = QFontMetrics(QFont());
    QFontMetrics m_fm_sender = QFontMetrics(QFont());
    QFontMetrics m_fm_unread_counter = QFontMetrics(QFont());

    int m_first_row_height;
    int m_second_row_height;

    int m_base_margin;
    int m_icon_radius;
    int m_icon_diameter;
    int m_unread_indicator_round;

    QColor m_unread_background;
    QColor m_selected_background;
    QColor m_item_under_mouse_background;

    QPen m_pen_for_text;
    QPen m_pen_for_background;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_CHAT_ITEM_DELEGATE_HPP_
