#include <chat_widget.hpp>
#include <config.hpp>
#include <db_storage.hpp>
#include <helpers.hpp>
#include <message_item_delegate.hpp>

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
    const auto& user_config = UserConfigSingletone::get_instance();
    const UserConfigSingletone::Appearance& user_ap = user_config.appearance();

    m_sender_font       = { user_ap.sender_font_params().family,
                            user_ap.sender_font_params().size,
                            user_ap.sender_font_params().weight };

    m_message_text_font = { user_ap.message_text_font_params().family,
                            user_ap.message_text_font_params().size,
                            user_ap.message_text_font_params().weight };

    m_timestamp_font    = { user_ap.timestamp_font_params().family,
                            user_ap.timestamp_font_params().size,
                            user_ap.timestamp_font_params().weight };

    m_sended_message_color = user_ap.sended_message_color();
    m_receive_message_color = user_ap.receive_message_color();

    const auto& dev_config = DevelopConfigSingletone::get_instance();
    const DevelopConfigSingletone::Appearance& dev_ap = dev_config.appearance();

    m_min_message_width = dev_ap.min_message_width();
    m_scale_message_width = dev_ap.scale_message_width();
    m_base_margin = dev_ap.base_margin();
    m_icon_diameter = dev_ap.icon_diameter();
    m_message_round_radius = dev_ap.message_round_radius();
    m_selected_message_color = dev_ap.selected_message_color();
}

void MessageItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option,
                                const QModelIndex& index) const
{
    auto message = index.data(Qt::DisplayRole).value<Chat::message_handle_t>();
    bool is_previous_same = index.data(MyRoles::AreIconAndSendernameNeededRole).value<bool>();

    painter->setRenderHint(QPainter::Antialiasing);
    QPen pen_for_background;
    pen_for_background.setStyle(Qt::NoPen);
    QPen standart_pen = painter->pen();

    // Icon rect and render
    if (!is_previous_same)
    {
        QPoint top_left(option.rect.topLeft());
        top_left.setX(top_left.x() + m_base_margin);
        top_left.setY(top_left.y() + m_base_margin);
        QPoint bottom_right(top_left.x() + m_icon_diameter, top_left.y() + m_icon_diameter);
        QRect icon_rect(top_left, bottom_right);

        QPainterPath icon_rect_path;
        icon_rect_path.addEllipse(icon_rect);
        painter->setPen(pen_for_background);

        QColor icon_color;
        if (message->user_id() == MY_USER_ID)
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
    QRect sender_rect = m_fm_sender.boundingRect(option.rect, Qt::AlignLeft, sender);
    sender_rect += QMargins(-m_icon_diameter - m_base_margin * 3,
                            -m_base_margin,
                             m_icon_diameter + m_base_margin * 5,  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
                             m_base_margin * 2);

    // Message text rect
    QString message_text = message->text_qstring();
    QRect message_max_rect = option.rect;
    message_max_rect.setWidth(static_cast<int>(option.rect.width() * m_scale_message_width));
    QRect message_text_rect = m_fm_message_text.boundingRect(message_max_rect,
                                                           Qt::AlignLeft | Qt::TextWordWrap,
                                                           message_text);
    if (!is_previous_same)
    {
        int message_text_height = message_text_rect.height();
        message_text_rect += QMargins(-m_icon_diameter - m_base_margin * 3,
                                       0,
                                       m_icon_diameter + m_base_margin * 5,  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
                                       m_base_margin * 2);
        message_text_rect.setY(sender_rect.y() + sender_rect.height());
        message_text_rect.setHeight(message_text_height);
    }
    else
    {
        message_text_rect += QMargins(-m_icon_diameter - m_base_margin * 3,
                                      -m_base_margin,
                                       m_icon_diameter + m_base_margin * 5,  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
                                       m_base_margin * 2);
    }
    if (message_text_rect.width() < m_min_message_width)
        message_text_rect.setWidth(m_min_message_width);

    // Timestamp rect
    QRect timestamp_rect = message_text_rect;
    timestamp_rect.setY(message_text_rect.y() + message_text_rect.height());

    if (!is_previous_same)
        timestamp_rect.setHeight(option.rect.height() - message_text_rect.height()
                                 - sender_rect.height() - m_base_margin * 3);
    else
        timestamp_rect.setHeight(option.rect.height() - message_text_rect.height() - m_base_margin * 3);


    // Rendering
    // Background rendering
    QRect message_background_rect = message_text_rect;

    if (!is_previous_same)
    {
        message_background_rect += QMargins(m_base_margin,
                                            sender_rect.height(),
                                            m_base_margin,
                                            timestamp_rect.height() + m_base_margin);
    }
    else
    {
        message_background_rect += QMargins(m_base_margin,
                                            0,
                                            m_base_margin,
                                            timestamp_rect.height() + m_base_margin);
    }
    QPainterPath message_background_path;
    QColor color;
    if (message->from() == QStringLiteral("Me"))
        color = m_sended_message_color;
    else
        color = m_receive_message_color;

    message_background_path.addRoundedRect(message_background_rect,
                                           m_message_round_radius,
                                           m_message_round_radius);
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

    // Timestamp and edit mark rendering
    painter->setFont(m_timestamp_font);
    Message::timestamp_t timestamp = message->timestamp();
    std::time_t time_tt = std::chrono::system_clock::to_time_t(timestamp);
    std::tm timestamp_tm;
    ::localtime_r(&time_tt, &timestamp_tm);

    QString time_str = this->date_number_handler(timestamp_tm.tm_mday) + QStringLiteral(" ") +
                       m_month_names[timestamp_tm.tm_mon] + QStringLiteral(" ") +
                       this->date_number_handler(timestamp_tm.tm_hour) + QStringLiteral(":") +
                       this->date_number_handler(timestamp_tm.tm_min);
    if (message->is_edit())
        time_str = tr("edit") + QStringLiteral("  ") + time_str;

    painter->drawText(timestamp_rect, Qt::AlignRight, time_str);
    painter->setFont(m_message_text_font);


    // Selection handling
    if (option.state & QStyle::State_Selected)
    {
        painter->setPen(pen_for_background);
        painter->fillPath(message_background_path, m_selected_message_color);
        painter->drawPath(message_background_path);
        painter->setPen(standart_pen);
    }
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QString text = index.data(MyRoles::MessageTextRole).toString();


    QRect message_max_rect = option.rect;
    message_max_rect.setWidth(static_cast<int>(option.rect.width() * m_scale_message_width));
    QRect my_rect = m_fm_message_text.boundingRect(message_max_rect,
                                                 Qt::AlignLeft | Qt::TextWordWrap,
                                                 text);

    int row_height = /*message text height*/ my_rect.height() +
                     /*timestamp height*/ m_fm_timestamp.height() +
                     /*margin for whitespaces between messages*/ m_base_margin * 4;

    if (!index.data(MyRoles::AreIconAndSendernameNeededRole).value<bool>())
        row_height += /*message sender*/ m_fm_sender.height();

    const auto* p = qobject_cast<QListView*>(this->parent());
    return QSize(p->viewport()->size().width(), row_height);
}

QString MessageItemDelegate::date_number_handler(const int& num) const
{
    if (num < 10)  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        return QString::number(0) + QString::number(num);
    return QString::number(num);
}

}  // namespace melon::client_desktop
