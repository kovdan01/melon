#ifndef MELON_CLIENT_DESKTOP_MESSAGE_CHAT_MODEL_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_CHAT_MODEL_HPP_

#include <ram_storage.hpp>

#include <QAbstractListModel>

namespace melon::client_desktop
{

class ChatListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChatListModel(QObject* parent = nullptr);

    using message_handle_t = Chat::message_handle_t;
    using chat_handle_t = RAMStorageSingletone::chat_handle_t;

    [[nodiscard]] int rowCount(const QModelIndex& index) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex&) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    void add_chat(const Chat& chat, const QModelIndex& parent = QModelIndex());
    void delete_chat(const QModelIndex& index, const QModelIndex& parent = QModelIndex());
    void set_external_chat(const QModelIndex& index, const QString& name);

    chat_handle_t chat_it_by_index(const QModelIndex& index);

private:
    QVector<QString> m_names_chats;
    std::vector<chat_handle_t> m_it_chats;
};

}  // namespace melon::client_desktop

#endif // MELON_CLIENT_DESKTOP_MESSAGE_LIST_MODEL_HPP_
