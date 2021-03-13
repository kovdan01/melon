#ifndef MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_

#include <ram_storage.hpp>

#include <QAbstractListModel>
#include <QColor>

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

    void add_message(chat_handle_t it_chat, const Message& message);
    void load_message(message_handle_t it_message);
    void delete_message(chat_handle_t it_chat, const QModelIndex& index, const QModelIndex& parent = QModelIndex());

    void clear();

    enum MyRoles
    {
        MessageTextRole = Qt::UserRole + 0,  // NOLINT (readability-identifier-naming)
        MessageTimestampRole = Qt::UserRole + 1,  // NOLINT (readability-identifier-naming)
        IsPreviousSameSenderAndTimeRole = Qt::UserRole + 2,  // NOLINT (readability-identifier-naming)
        IsNextSameSenderAndTimeRole = Qt::UserRole + 3,  // NOLINT (readability-identifier-naming)
        IsEditRole = Qt::UserRole + 4,  // NOLINT (readability-identifier-naming)
    };

private:
    message_handle_t add_message_to_ram_storage(chat_handle_t it_chat, const Message& message);
    void set_message_in_ram_storage(const QModelIndex& index, const QString& message);
    [[nodiscard]] bool has_messages_same_sender_and_time(const int& less_row, const int& bigger_row) const;

    std::vector<message_handle_t> m_it_messages;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_
