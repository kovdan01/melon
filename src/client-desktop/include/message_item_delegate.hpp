#ifndef MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_

#include <config.hpp>
#include <message_list_model.hpp>

#include <QStyledItemDelegate>
#include <QTextDocument>

#include <vector>

namespace melon::client_desktop
{

namespace dev_conf_ap = develop_config::appearance;

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
    std::vector<QString> m_month_names = {tr("Jan"), tr("Feb"), tr("Mar"), tr("Apr"),
                                          tr("May"), tr("Jun"), tr("Jul"), tr("Aug"),
                                          tr("Sep"), tr("Oct"), tr("Nov"), tr("Dec")};

    const QFont m_sender_font       {user_config::appearance::m_sender_font_params.font_family,
                                     user_config::appearance::m_sender_font_params.font_size,
                                     user_config::appearance::m_sender_font_params.weight};
    const QFont m_message_text_font {user_config::appearance::m_message_text_font_params.font_family,
                                     user_config::appearance::m_message_text_font_params.font_size,
                                     user_config::appearance::m_message_text_font_params.weight};
    const QFont m_timestamp_font    {user_config::appearance::m_timestamp_font_params.font_family,
                                     user_config::appearance::m_timestamp_font_params.font_size,
                                     user_config::appearance::m_timestamp_font_params.weight};
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MESSAGE_ITEM_DELEGATE_HPP_
