#include <message_item_delegate.hpp>
#include <chat_widget.hpp>
#include <ram_storage.hpp>

#include <QListView>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QRect>
#include <QFontMetrics>

#include <iostream>

namespace melon::client_desktop
{

MessageItemDelegate::MessageItemDelegate(QObject* parent)
    : QStyledItemDelegate{parent}
{
}

void MessageItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option,
                                const QModelIndex& index) const
{
    painter->setRenderHint(QPainter::Antialiasing);
    QPen pen_for_background;
    pen_for_background.setStyle(Qt::NoPen);
    QPen standart_pen = painter->pen();

    QString message_text = index.data(MyRoles::MessageTextRole).toString();
    QFontMetrics fm(painter->font());

    QRect message_max_rect = option.rect;
    message_max_rect.setWidth(static_cast<int>(option.rect.width() / scale_message_length));
    QRect message_text_rect = fm.boundingRect(message_max_rect, Qt::AlignLeft | Qt::TextWordWrap, message_text);
    message_text_rect += QMargins(-10, -5, 10, 10);

    QRect message_background_rect = message_text_rect;
    message_background_rect += QMargins(5, 5, 5, 5);

    QPainterPath message_background_path;

    auto message = index.data(Qt::DisplayRole).value<Chat::message_handle_t>();

    QColor color;

    if (message->from() == QStringLiteral("Me"))
    {
        color = QColor(10, 10, 80, 30);
    }
    else
    {
        color = index.data(Qt::BackgroundRole).value<QColor>();
    }

    message_background_path.addRoundedRect(message_background_rect, 10, 10);
    painter->setPen(pen_for_background);
    painter->fillPath(message_background_path, color);
    painter->drawPath(message_background_path);
    painter->setPen(standart_pen);

    painter->drawText(message_text_rect, Qt::AlignLeft | Qt::TextWordWrap | Qt::AlignVCenter, message_text);

    QFont prev_font = painter->font();

    QFont font;
    font.setWeight(QFont::Light);
    font.setPointSizeF(prev_font.pointSize() / scale_message_length);
    painter->setFont(font);
    painter->drawText(option.rect, Qt::AlignRight, QStringLiteral("Time!"));

    painter->setFont(prev_font);

    if (option.state & QStyle::State_Selected)
    {
        painter->setPen(pen_for_background);
        painter->fillPath(message_background_path, QColor(30, 30, 90, 50));
        painter->drawPath(message_background_path);
        painter->setPen(standart_pen);
    }
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QListView *p = qobject_cast<QListView*>(this->parent());
    QString text = index.data(MyRoles::MessageTextRole).toString();
    QFontMetrics fm(p->font());

    QRect message_max_rect = option.rect;
    message_max_rect.setWidth(static_cast<int>(option.rect.width() / scale_message_length));
    QRect my_rect = fm.boundingRect(message_max_rect, Qt::TextWordWrap, text);

    return QSize(p->viewport()->size().width(), my_rect.height() + 20);
}

}  // namespace melon::client_desktop
