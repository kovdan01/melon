#ifndef MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_

#include <ram_storage.hpp>

#include <QAbstractListModel>

#include <vector>

namespace melon::client_desktop
{

class MessageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MessageListModel(QObject* parent = nullptr);

    using message_handle_t = Chat::message_handle_t;
    using chat_handle_t = RAMStorageSingletone::chat_handle_t;

    [[nodiscard]] int rowCount(const QModelIndex& index) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex&) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    void add_message(message_handle_t it_message, const QModelIndex& parent = QModelIndex());
    void delete_message(chat_handle_t it_chat, const QModelIndex& index, const QModelIndex& parent = QModelIndex());

    message_handle_t add_external_message(chat_handle_t it_chat, const Message& message);
    void set_external_message(const QModelIndex& index, const QString& message);
    void clear();

private:
    constexpr static QColor M_RECEIVE_COLOR{250, 224, 180, 127};
    QVector<QString> m_text_messages;
    std::vector<message_handle_t> m_it_messages;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_
