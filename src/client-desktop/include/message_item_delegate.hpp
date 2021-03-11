#ifndef MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_

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

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    [[nodiscard]] QString date_number_handler(const int& num) const;
    std::vector<QString> m_month_names = {tr("Jan"), tr("Feb"), tr("Mar"), tr("Apr"),
                                          tr("Jun"), tr("Jul"), tr("Aug"),
                                          tr("Sep"), tr("Oct"), tr("Nov"), tr("Dec")};

    QFont m_sender_font     {QStringLiteral("Cantarell"), /*pointSize*/ 9, /*weight*/ QFont::DemiBold};  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    QFont m_base_font       {QStringLiteral("Cantarell"), /*pointSize*/ 9, /*weight*/ 41};  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    QFont m_timestamp_font  {QStringLiteral("Cantarell"), /*pointSize*/ 6, /*weight*/ 35};  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    constexpr static QColor M_SELECTED_COLOR{30, 30, 90, 50};
    const static int M_MIN_MESSAGE_WIDTH = 100;
    constexpr static qreal M_SCALE_MESSAGE_LENGTH = 0.5;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_
