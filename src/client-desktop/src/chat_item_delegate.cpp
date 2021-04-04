#include <chat_item_delegate.hpp>
#include <config.hpp>
#include <db_storage.hpp>
#include <helpers.hpp>

#include <QListView>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>

namespace melon::client_desktop
{

ChatItemDelegate::ChatItemDelegate(QObject* parent)
    : QStyledItemDelegate{parent}
{
    const auto& user_config = UserConfigSingletone::get_instance();
    const UserConfigSingletone::Appearance& user_ap = user_config.appearance();

    m_selected_background = user_ap.selected_chat_color();
    m_unread_background = user_ap.unread_background_color();

    m_chat_name_font        = { user_ap.chat_name_font_params().family,
                                user_ap.chat_name_font_params().size,
                                user_ap.chat_name_font_params().weight };

    m_last_message_font     = { user_ap.last_message_font_params().family,
                                user_ap.last_message_font_params().size,
                                user_ap.last_message_font_params().weight };

    m_timestamp_font        = { user_ap.chat_timestamp_font_params().family,
                                user_ap.chat_timestamp_font_params().size,
                                user_ap.chat_timestamp_font_params().weight };

    m_sender_font           = { user_ap.last_message_sender_font_params().family,
                                user_ap.last_message_sender_font_params().size,
                                user_ap.last_message_sender_font_params().weight };

    m_unread_counter_font   = { user_ap.unread_counter_font_params().family,
                                user_ap.unread_counter_font_params().size,
                                user_ap.unread_counter_font_params().weight };

    const auto& dev_config = DevelopConfigSingletone::get_instance();
    const DevelopConfigSingletone::Appearance& dev_ap = dev_config.appearance();

    QFontMetrics fm_chat_name = QFontMetrics(m_chat_name_font);
    m_fm_chat_name.swap(fm_chat_name);
    QFontMetrics fm_timestamp = QFontMetrics(m_timestamp_font);
    m_fm_timestamp.swap(fm_timestamp);
    QFontMetrics fm_last_message = QFontMetrics(m_last_message_font);
    m_fm_last_message.swap(fm_last_message);
    QFontMetrics fm_sender = QFontMetrics(m_sender_font);
    m_fm_sender.swap(fm_sender);
    QFontMetrics fm_unread_counter = QFontMetrics(m_unread_counter_font);
    m_fm_unread_counter.swap(fm_unread_counter);

    m_item_under_mouse_background = dev_ap.item_under_mouse_color();

    m_base_margin = dev_ap.chat_base_margin();
    m_icon_radius = dev_ap.chat_icon_radius();
    m_icon_diameter = m_icon_radius * 2;
    m_unread_indicator_round = dev_ap.unread_indicator_round();

    m_first_row_height = std::max(fm_chat_name.height(), m_fm_timestamp.height()) + m_base_margin;
    m_second_row_height = std::max(m_fm_last_message.height(), std::max(m_fm_sender.height(), m_fm_unread_counter.height()));

    m_pen_for_background.setStyle(Qt::NoPen);
    m_pen_for_text.setColor(Qt::black);
}

QString ChatItemDelegate::date_number_handler(const int& num) const
{
    if (num < 10)  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        return QString::number(0) + QString::number(num);
    return QString::number(num);
}

QString ChatItemDelegate::date_handler(const Message::timestamp_t& timestamp) const
{
    Message::timestamp_t now = std::chrono::system_clock::now();
    auto diff_time_days = std::chrono::duration_cast<std::chrono::days>(timestamp - now).count();
    std::time_t time_tt = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm;
    ::localtime_r(&time_tt, &tm);

    if (diff_time_days == 0)
        return this->date_number_handler(tm.tm_hour) + QStringLiteral(":") + this->date_number_handler(tm.tm_min);

    return this->date_number_handler(tm.tm_mday) + QStringLiteral(" ") +
           m_month_names[to_size_t(tm.tm_mon)] + QStringLiteral(" ") +
           this->date_number_handler(tm.tm_year % 100);  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
}

void ChatItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    auto chat = index.data(Qt::DisplayRole).value<StorageSingletone::chat_handle_t>();
    auto last_message = chat->last_message();

    QPen standart_pen = painter->pen();

    // Backgound render if needed
    painter->setPen(m_pen_for_background);
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, m_selected_background);
    else if (option.state & QStyle::State_MouseOver)
        painter->fillRect(option.rect, m_item_under_mouse_background);
    painter->setPen(standart_pen);

    // Icon rect and render
    QPoint top_left(option.rect.topLeft().x() + m_base_margin, option.rect.center().y() - m_icon_radius);
    QPoint bottom_right(top_left.x() + m_icon_diameter, top_left.y() + m_icon_diameter);
    QRect icon_rect(top_left, bottom_right);

    QPainterPath icon_rect_path;
    icon_rect_path.addEllipse(icon_rect);
    painter->setPen(m_pen_for_background);

    QColor icon_color = Qt::darkYellow;
    painter->fillPath(icon_rect_path, icon_color);
    painter->drawPath(icon_rect_path);
    painter->setPen(standart_pen);

     // Max chat name rect
    QRect max_chat_name_rect = option.rect;
    max_chat_name_rect.setWidth(option.rect.width() - m_icon_diameter - m_base_margin * 3);

    if (chat->empty())
    {
        QString elided_name = m_fm_chat_name.elidedText(chat->chatname(), Qt::ElideRight, max_chat_name_rect.width());
        QRect chat_name_rect = m_fm_chat_name.boundingRect(max_chat_name_rect, Qt::AlignLeft, elided_name);
        chat_name_rect += QMargins(-m_icon_diameter - m_base_margin * 3,
                                   -m_base_margin,
                                    m_icon_diameter + m_base_margin * 3,
                                    m_base_margin);
        painter->setFont(m_chat_name_font);
        painter->setPen(m_pen_for_text);
        painter->drawText(chat_name_rect, Qt::AlignLeft, elided_name);
        painter->setPen(standart_pen);
        return;
    }

    // Timestamp rect and render
    QString time_str = date_handler(last_message->timestamp());
    QRect timestamp_rect = m_fm_timestamp.boundingRect(option.rect, Qt::AlignRight, time_str);
    timestamp_rect += QMargins( m_base_margin,
                               -m_base_margin,
                               -m_base_margin,
                                m_base_margin);
    painter->setFont(m_timestamp_font);
    painter->setPen(m_pen_for_text);
    painter->drawText(timestamp_rect, Qt::AlignRight, time_str);
    painter->setPen(standart_pen);

    // Chat name rect and render
    max_chat_name_rect.setWidth(max_chat_name_rect.width() - timestamp_rect.width() - m_base_margin);
    QString elided_name = m_fm_chat_name.elidedText(chat->chatname(), Qt::ElideRight, max_chat_name_rect.width());
    QRect chat_name_rect = m_fm_chat_name.boundingRect(max_chat_name_rect, Qt::AlignLeft, elided_name);
    chat_name_rect += QMargins(-m_icon_diameter - m_base_margin * 3,
                               -m_base_margin,
                                m_icon_diameter + m_base_margin * 3,
                                m_base_margin);
    painter->setFont(m_chat_name_font);
    painter->setPen(m_pen_for_text);
    painter->drawText(chat_name_rect, Qt::ElideRight, elided_name);
    painter->setPen(standart_pen);

    // Last message text rect definition
    QRect max_message_text_rect = option.rect;
    max_message_text_rect.setX(max_message_text_rect.x() + m_icon_diameter + m_base_margin * 3);
    max_message_text_rect.setY(max_message_text_rect.y() + m_first_row_height + m_base_margin);

    if (!chat->is_read())
    {
        // Unread indicator rect and render
        QString unread_num = QStringLiteral("unr");
        int unread_num_width = m_fm_unread_counter.boundingRect(option.rect, Qt::AlignRight, unread_num).width() + m_base_margin * 2;
        QRect unread_rect(option.rect.bottomRight().x() - unread_num_width - m_base_margin,
                          option.rect.bottomRight().y() - m_second_row_height - m_base_margin,
                          unread_num_width,
                          m_second_row_height);
        QPainterPath unread_rect_path;
        unread_rect_path.addRoundedRect(unread_rect,
                                        m_unread_indicator_round,
                                        m_unread_indicator_round);
        painter->setPen(m_pen_for_background);
        painter->fillPath(unread_rect_path, m_unread_background);
        painter->drawPath(unread_rect_path);
        painter->setPen(standart_pen);

        painter->setFont(m_unread_counter_font);
        painter->setPen(m_pen_for_text);
        painter->drawText(unread_rect, Qt::AlignCenter, unread_num);
        painter->setPen(standart_pen);

        // Last message max rect
        max_message_text_rect.setWidth(max_message_text_rect.width() - unread_rect.width() - m_base_margin);
    }

    // Sender rect and render
    QString sender(last_message->from());
    if (sender == tr("Me"))
        sender = tr("You");
    sender += QStringLiteral(": ");

    QString elided_sender = m_fm_sender.elidedText(sender, Qt::ElideRight, max_message_text_rect.width());
    QRect sender_rect = m_fm_sender.boundingRect(max_message_text_rect, Qt::AlignLeft, elided_sender);
    painter->setFont(m_sender_font);
    painter->setPen(m_pen_for_text);
    painter->drawText(sender_rect, Qt::AlignLeft, sender);

    max_message_text_rect.setX(max_message_text_rect.x() + sender_rect.width());

    // Last message rect and render
    QString message_text = last_message->text();
    message_text = message_text.replace(QStringLiteral("\n"), QStringLiteral(" "));  // -V567
    QString elided_text = m_fm_last_message.elidedText(message_text, Qt::ElideRight, max_message_text_rect.width());

    QRect message_rect = m_fm_last_message.boundingRect(max_message_text_rect, Qt::AlignLeft, elided_text);
    painter->setFont(m_last_message_font);
    painter->drawText(message_rect, Qt::AlignLeft, elided_text);
    painter->setPen(standart_pen);
}

QSize ChatItemDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
{
    const auto* p = qobject_cast<QListView*>(this->parent());
    return QSize(p->viewport()->size().width(), m_first_row_height + m_second_row_height + m_base_margin * 2);
}

}  // namespace melon::client_desktop
