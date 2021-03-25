#ifndef MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_
#define MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_

#include <ram_storage.hpp>

#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtWidgets>

#include <iostream>

namespace melon::client_desktop
{

QString create_connection_with_db();

static std::uint64_t max_message_id(std::uint64_t chat_id, std::uint64_t domain_id)
{
    QSqlQuery qry(QSqlDatabase::database(DB_NAME));
    QString qry_string = QStringLiteral("SELECT MAX(message_id) FROM messages where chat_id=") + QString::number(chat_id)
                         + QStringLiteral(" and domain_id=") + QString::number(domain_id);
    if (!qry.exec(qry_string))
    {
        std::cout << "Fail selecting message_id!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
    qry.next();
    QVariant msg_id_qvar = qry.value(0);
    if (!msg_id_qvar.isValid())
    {
        std::cout << "message_id is null!" << std::endl;
        return 0;
    }
    std::uint64_t msg_id = msg_id_qvar.value<std::uint64_t>();

    return msg_id;
}

static int status_to_int(melon::core::Message::Status status)
{
    switch (status)
    {
    case melon::core::Message::Status::FAIL:
        return 0;
    case melon::core::Message::Status::SEEN:
        return 1;
    case melon::core::Message::Status::RECEIVED:
        return 2;
    case melon::core::Message::Status::SENT:
        return 3;
    }
    return 5;
}

static melon::core::Message::Status int_to_status(int status)
{
    switch (status)
    {
    case 0:
        return melon::core::Message::Status::FAIL;
    case 1:
        return melon::core::Message::Status::SEEN;
    case 2:
        return melon::core::Message::Status::RECEIVED;
    case 3:
        return melon::core::Message::Status::SENT;
    }
    return melon::core::Message::Status::FAIL;
}

class Message : public MessageRAM
{
public:
    //For Insert
    Message(std::uint64_t user_id, std::uint64_t chat_id, std::uint64_t domain_id,
            timestamp_t timestamp, QString text, Status status)
        : MessageRAM(user_id, chat_id, domain_id, timestamp, text, status)
    {
        this->set_message_id(max_message_id(this->chat_id(), this->domain_id()) + 1);

        QSqlQuery qry(QSqlDatabase::database(DB_NAME));
        if(!qry.prepare(QStringLiteral("INSERT INTO messages"
                                       " VALUES("
                                       " :msg_id,"
                                       " :user_id,"
                                       " :chat_id,"
                                       " :domain_id,"
                                       " :timestamp,"
                                       " :text,"
                                       " :status)")))
        {
            std::cout << "Fail preparing inserting message!" << std::endl;
            std::cout << qry.lastError().text().toStdString() << std::endl;
        }
        qry.bindValue(QStringLiteral(":msg_id"), QVariant::fromValue(this->message_id()));
        qry.bindValue(QStringLiteral(":user_id"), QVariant::fromValue(this->user_id()));
        qry.bindValue(QStringLiteral(":chat_id"), QVariant::fromValue(this->chat_id()));
        qry.bindValue(QStringLiteral(":domain_id"), QVariant::fromValue(this->domain_id()));

        auto timestamp_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(this->timestamp());
        std::uint64_t timestamp_ms_int = timestamp_ms.time_since_epoch().count();

        std::chrono::milliseconds dur(timestamp_ms_int);
        std::chrono::time_point<std::chrono::system_clock> dt(dur);
        if (dt != timestamp_ms)
            std::cout << "Sth went wrong with casting timestamps!" << std::endl;

        qry.bindValue(QStringLiteral(":timestamp"), QVariant::fromValue(timestamp_ms_int));
        qry.bindValue(QStringLiteral(":text"), this->text_qstring());
        qry.bindValue(QStringLiteral(":status"), status_to_int(this->status()));

        if (!qry.exec())
        {
            std::cout << "Fail inserting message!" << std::endl;
            std::cout << qry.lastError().text().toStdString() << std::endl;
        }
        else
        {
            QString qry_string = QStringLiteral("SELECT COUNT(*) FROM messages where chat_id=") + QString::number(this->chat_id());
            qry.exec(qry_string);
            qry.next();
            std::cout << "Now number of messages in chat is " << qry.value(0).toInt() << std::endl;
        }
    }

    //For Select
    Message(std::uint64_t user_id, std::uint64_t chat_id, std::uint64_t domain_id)
        : MessageRAM(user_id, chat_id, domain_id)
    {

    }

    //For incomplete message
    Message(QString text)
    {
        this->set_text_qstring(text);
    }

    //for QVariant
    Message() = default;

    void set_text_qstring(QString text)
    {
        set_text_qstring_RAM(text);
    }
};

static std::uint64_t max_chat_id(std::uint64_t domain_id)
{
    QSqlQuery qry(QSqlDatabase::database(DB_NAME));
    QString qry_string = QStringLiteral("SELECT MAX(chat_id) FROM chats WHERE domain_id=") + QString::number(domain_id);
    if (!qry.exec(qry_string))
    {
        std::cout << "Fail selecting chat_id!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
    qry.next();
    QVariant chat_id_qvar = qry.value(0);
    if (!chat_id_qvar.isValid())
    {
        std::cout << "chat_id is null!" << std::endl;
        return 0;
    }
    std::uint64_t chat_id = chat_id_qvar.value<std::uint64_t>();

    return chat_id;
}

class Chat : public ChatRAM
{
public:
    using message_handle_t = std::list<Message>::iterator;

    // For insert
    Chat(std::uint64_t domain_id, QString name)
        : ChatRAM(domain_id, name)
        , m_incomplete_message(QStringLiteral(""))
    {
        this->set_chat_id(max_chat_id(this->domain_id()) + 1);
        QSqlQuery qry(QSqlDatabase::database(DB_NAME));
        if(!qry.prepare(QStringLiteral("INSERT INTO chats"
                                       " VALUES(:chat_id, :domain_id, :name)")))
        {
            std::cout << "Fail preparing inserting chat!" << std::endl;
            std::cout << qry.lastError().text().toStdString() << std::endl;
        }
        qry.bindValue(QStringLiteral(":chat_id"), QVariant::fromValue(this->chat_id()));
        qry.bindValue(QStringLiteral(":domain_id"), QVariant::fromValue(this->domain_id()));
        qry.bindValue(QStringLiteral(":name"), this->name_qstring());

        if (!qry.exec())
        {
            std::cout << "Fail inserting chat!" << std::endl;
            std::cout << qry.lastError().text().toStdString() << std::endl;
        }
        else
        {
              qry.prepare(QStringLiteral("SELECT COUNT(*) FROM chats"));
              qry.exec();
              qry.next();
              std::cout << "Now number of chats is " << qry.value(0).toInt() << std::endl;
        }
    }

    void remove_from_db()
    {
        QSqlQuery qry(QSqlDatabase::database(DB_NAME));

        QString query_string = QStringLiteral("DELETE FROM chats WHERE chat_id=") + QString::number(this->chat_id())
                               + QStringLiteral(" and domain_id=") + QString::number(this->domain_id());

        if (!qry.exec(query_string))
        {
            std::cout << "Fail deleting chat!" << std::endl;
            std::cout << qry.lastError().text().toStdString() << std::endl;
        }
        else
        {
            qry.prepare(QStringLiteral("SELECT COUNT(*) FROM chats"));
            qry.exec();
            qry.next();
            std::cout << "Now number of chats is " << qry.value(0).toInt() << std::endl;
        }
    }

    [[nodiscard]] const std::list<Message>& messages() const noexcept
    {
        return m_messages;
    }

    [[nodiscard]] std::list<Message>& messages()noexcept
    {
        return m_messages;
    }

    message_handle_t add_message(Message message)
    {
        m_messages.emplace_back(std::move(message));
        return std::prev(m_messages.end());
    }

    message_handle_t delete_message(message_handle_t message_handle)
    {
        return m_messages.erase(message_handle);
    }

    void set_incomplete_message(Message incomplete_message)
    {
        m_incomplete_message = std::move(incomplete_message);
    }

    [[nodiscard]] message_handle_t last_message()
    {
        if (m_messages.empty())
            return m_messages.begin();
        return std::prev(m_messages.end());
    }

    [[nodiscard]] bool empty() const
    {
        return m_messages.empty();
    }

    [[nodiscard]] const Message& incomplete_message() const noexcept
    {
        return m_incomplete_message;
    }

    void set_name_qstring(QString text)
    {
        set_name_qstring_RAM(text);
    }

private:
    std::list<Message> m_messages = {};
    Message m_incomplete_message;
};


class StorageSingletone
{
public:
    using chat_handle_t = std::list<Chat>::iterator;
    using const_chat_handle_t = std::list<Chat>::const_iterator;

    [[nodiscard]] static StorageSingletone& get_instance()
    {
        static StorageSingletone instance;
        return instance;
    }

    StorageSingletone(const StorageSingletone& root) = delete;
    StorageSingletone& operator=(const StorageSingletone&) = delete;
    StorageSingletone(StorageSingletone&& root) = delete;
    StorageSingletone& operator=(StorageSingletone&&) = delete;

    chat_handle_t add_chat(std::uint64_t domain_id, const QString& name)
    {
        Chat chat(domain_id, name);
        m_chats.emplace_back(std::move(chat));
        return std::prev(m_chats.end());
    }

    chat_handle_t delete_chat(chat_handle_t chat_handle)
    {
        chat_handle->remove_from_db();

        return m_chats.erase(chat_handle);
    }

    [[nodiscard]] const std::list<Chat>& chats() const
    {
        return m_chats;
    }

    [[nodiscard]] std::list<Chat>& chats()
    {
        return m_chats;
    }
private:
    std::list<Chat> m_chats;

    StorageSingletone() = default;
    ~StorageSingletone() = default;
};

}  // namespace melon::client_desktop

Q_DECLARE_METATYPE(melon::client_desktop::StorageSingletone::chat_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Chat::message_handle_t)
Q_DECLARE_METATYPE(melon::client_desktop::Message)
Q_DECLARE_METATYPE(std::uint64_t)

#endif // MELON_CLIENT_DESKTOP_DB_STORAGE_HPP_
