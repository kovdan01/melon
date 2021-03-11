#include <chat_widget.hpp>
#include <message_item_delegate.hpp>
#include <ram_storage.hpp>

#include <QFont>
#include <QFontMetrics>
#include <QListView>
#include <QPainter>
#include <QPainterPath>
#include <QRect>

#include <ctime>

namespace melon::client_desktop
{

MessageItemDelegate::MessageItemDelegate(QObject* parent)
    : QStyledItemDelegate{parent}
{
}

void MessageItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option,
                                const QModelIndex& index) const
{
    auto message = index.data(Qt::DisplayRole).value<Chat::message_handle_t>();
    bool is_previous_same = index.data(MyRoles::IsPreviousSameSenderAndTimeRole).value<bool>();

    painter->setRenderHint(QPainter::Antialiasing);
    QPen pen_for_background;
    pen_for_background.setStyle(Qt::NoPen);
    QPen standart_pen = painter->pen();

    // Fonts setting
    QFontMetrics fm_sender(m_sender_font);
    QFontMetrics fm_message_text(m_message_text_font);

    // Icon rect and render
    if (!is_previous_same)
    {
        QPoint top_left(option.rect.topLeft());
        top_left.setX(top_left.x() + dev_conf_ap::M_BASE_MARGIN);
        top_left.setY(top_left.y() + dev_conf_ap::M_BASE_MARGIN);
        QPoint bottom_right(top_left.x() + dev_conf_ap::M_ICON_DIAMETER, top_left.y() + dev_conf_ap::M_ICON_DIAMETER);
        QRect icon_rect(top_left, bottom_right);

        QPainterPath icon_rect_path;
        icon_rect_path.addEllipse(icon_rect);
        painter->setPen(pen_for_background);

        QColor icon_color;
        if (message->from() == QStringLiteral("Me"))
            icon_color = Qt::darkCyan;
        else
            icon_color = Qt::darkGreen;
        painter->fillPath(icon_rect_path, icon_color);
        painter->drawPath(icon_rect_path);
        painter->setPen(standart_pen);
    }

    // Rect setting
    // Sender rect
    QString sender = message->from();
    QRect sender_rect = fm_sender.boundingRect(option.rect, Qt::AlignLeft, sender);
    sender_rect += QMargins(-dev_conf_ap::M_ICON_DIAMETER - dev_conf_ap::M_BASE_MARGIN * 3,
                            -dev_conf_ap::M_BASE_MARGIN,
                             dev_conf_ap::M_ICON_DIAMETER + dev_conf_ap::M_BASE_MARGIN * 5,
                             dev_conf_ap::M_BASE_MARGIN * 2);

    // Message text rect
    QString message_text = message->text();
    QRect message_max_rect = option.rect;
    message_max_rect.setWidth(static_cast<int>(option.rect.width() * dev_conf_ap::M_SCALE_MESSAGE_LENGTH));
    QRect message_text_rect = fm_message_text.boundingRect(message_max_rect,
                                                           Qt::AlignLeft | Qt::TextWordWrap,
                                                           message_text);
    if (!is_previous_same)
    {
        int message_text_height = message_text_rect.height();
        message_text_rect += QMargins(-dev_conf_ap::M_ICON_DIAMETER - dev_conf_ap::M_BASE_MARGIN * 3,
                                       0,
                                       dev_conf_ap::M_ICON_DIAMETER + dev_conf_ap::M_BASE_MARGIN * 5,
                                       dev_conf_ap::M_BASE_MARGIN * 2);
        message_text_rect.setY(sender_rect.y() + sender_rect.height());
        message_text_rect.setHeight(message_text_height);
    }
    else
    {
        message_text_rect += QMargins(-dev_conf_ap::M_ICON_DIAMETER - dev_conf_ap::M_BASE_MARGIN * 3,
                                      -dev_conf_ap::M_BASE_MARGIN,
                                       dev_conf_ap::M_ICON_DIAMETER + dev_conf_ap::M_BASE_MARGIN * 5,
                                       dev_conf_ap::M_BASE_MARGIN * 2);
    }
    if (message_text_rect.width() < dev_conf_ap::M_MIN_MESSAGE_WIDTH)
        message_text_rect.setWidth(dev_conf_ap::M_MIN_MESSAGE_WIDTH);

    // Timestamp rect
    QRect timestamp_rect = message_text_rect;
    timestamp_rect.setY(message_text_rect.y() + message_text_rect.height());

    if (!is_previous_same)
        timestamp_rect.setHeight(option.rect.height() - message_text_rect.height()
                                 - sender_rect.height() - dev_conf_ap::M_BASE_MARGIN * 3);
    else
        timestamp_rect.setHeight(option.rect.height() - message_text_rect.height() - dev_conf_ap::M_BASE_MARGIN * 3);


    // Rendering
    // Background rendering
    QRect message_background_rect = message_text_rect;

    if (!is_previous_same)
        message_background_rect += QMargins( dev_conf_ap::M_BASE_MARGIN,
                                             sender_rect.height(),
                                             dev_conf_ap::M_BASE_MARGIN,
                                             timestamp_rect.height() + dev_conf_ap::M_BASE_MARGIN);
    else
        message_background_rect += QMargins( dev_conf_ap::M_BASE_MARGIN,
                                             0,
                                             dev_conf_ap::M_BASE_MARGIN,
                                             timestamp_rect.height() + dev_conf_ap::M_BASE_MARGIN);
    QPainterPath message_background_path;
    auto color = index.data(Qt::BackgroundRole).value<QColor>();
    message_background_path.addRoundedRect(message_background_rect,
                                           dev_conf_ap::M_MESSAGE_ROUND_RADIUS,
                                           dev_conf_ap::M_MESSAGE_ROUND_RADIUS);
    painter->setPen(pen_for_background);
    painter->fillPath(message_background_path, color);
    painter->drawPath(message_background_path);
    painter->setPen(standart_pen);

    // Sender name rendering
    if (!is_previous_same)
    {
        painter->setFont(m_sender_font);
        painter->drawText(sender_rect, sender);
    }

    // Message text rendering
    painter->setFont(m_message_text_font);
    painter->drawText(message_text_rect,
                      Qt::AlignLeft | Qt::TextWordWrap,
                      message_text);

    // Timestamp rendering
    painter->setFont(m_timestamp_font);
    Message::timestamp_t timestamp = message->timestamp();
    std::time_t time_tt = std::chrono::system_clock::to_time_t(timestamp);
    std::tm* timestamp_tm = std::localtime(&time_tt);

    QString time_str = this->date_number_handler(timestamp_tm->tm_mday) + QStringLiteral(" ")
                       + m_month_names[timestamp_tm->tm_mon] + QStringLiteral(" ")
                       + this->date_number_handler(timestamp_tm->tm_hour) + QStringLiteral(":")
                       + this->date_number_handler(timestamp_tm->tm_min);

    painter->drawText(timestamp_rect, Qt::AlignRight, time_str);
    painter->setFont(m_message_text_font);


    // Selection handling
    if (option.state & QStyle::State_Selected)
    {
        painter->setPen(pen_for_background);
        painter->fillPath(message_background_path, dev_conf_ap::M_SELECTED_MESSAGE_COLOR);
        painter->drawPath(message_background_path);
        painter->setPen(standart_pen);
    }
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QString text = index.data(MyRoles::MessageTextRole).toString();
    QFontMetrics fm_sender(m_sender_font);
    QFontMetrics fm_message_text(m_message_text_font);
    QFontMetrics fm_timestamp(m_timestamp_font);

    QRect message_max_rect = option.rect;
    message_max_rect.setWidth(static_cast<int>(option.rect.width() * dev_conf_ap::M_SCALE_MESSAGE_LENGTH));
    QRect my_rect = fm_message_text.boundingRect(message_max_rect,
                                                 Qt::AlignLeft | Qt::TextWordWrap,
                                                 text);

    int row_height = /*message text height*/ my_rect.height() +
                     /*timestamp height*/ fm_timestamp.height() +
                     /*margin for whitespaces between messages*/ dev_conf_ap::M_BASE_MARGIN * 4;

    if (!index.data(MyRoles::IsPreviousSameSenderAndTimeRole).value<bool>())
        row_height += /*message sender*/ fm_sender.height();

    auto* p = qobject_cast<QListView*>(this->parent());
    return QSize(p->viewport()->size().width(), row_height);
}

QString MessageItemDelegate::date_number_handler(const int& num) const
{
    if (num < 10)
        return QString::number(0) + QString::number(num);
    return QString::number(num);
}

}  // namespace melon::client_desktop
