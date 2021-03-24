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

class Message : public MessageRAM
{
public:
    Message(std::uint64_t user_id, std::uint64_t chat_id, std::uint64_t domain_id,
            timestamp_t timestamp, QString text, Status status)
        : MessageRAM(user_id, chat_id, domain_id, timestamp, text, status)
    {
    }

    //for QVariant
    Message() = default;

    void set_text_qstring(QString text)
    {
        set_text_qstring_RAM(text);
    }
};

static std::uint64_t max_chat_id()
{
    QSqlQuery qry(QSqlDatabase::database(DB_NAME));
    if (!qry.exec(QStringLiteral("SELECT MAX(chat_id) FROM chats")))
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

    Chat(std::uint64_t domain_name, QString name)
        : ChatRAM(domain_name, name)
        , m_incomplete_message(/* from */ MY_USER_ID,
                               /* chat_id*/ DRAFT_CHAT_ID,
                               /* domain_id*/ DRAFT_DOMAIN_ID,
                               /* timestamp */ {},
                               /* text */ QStringLiteral(""),
                               /* status*/ melon::core::Message::Status::FAIL)
    {
        this->set_chat_id(max_chat_id() + 1);
        std::cout << "[Constructor] Id of current chat is " << this->chat_id() << std::endl;
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
//        QSqlQuery qry(DB_NAME);
//        qry.prepare(QStringLiteral("INSERT INTO messages"
//                                   " values "
//                                   "(:message_id,"
//                                   " :user_id,"
//                                    ":chat_id,"
//                                    ":domain_id,"
//                                    ":timestamp,"
//                                    ":text,"
//                                    ":status)"));

//        qry.bindValue(QStringLiteral(":message_id"), QVariant::fromValue(message.message_id()));
//        qry.bindValue(QStringLiteral(":user_id"), QVariant::fromValue(message.user_id()));
//        qry.bindValue(QStringLiteral(":chat_id"), QVariant::fromValue(message.chat_id()));
//        qry.bindValue(QStringLiteral(":domain_id"), QVariant::fromValue(message.domain_id()));
//        int timestamp_in_ms = message.timestamp().time_since_epoch().count();
//        qry.bindValue(QStringLiteral(":timestamp"), 1);
//        qry.bindValue(QStringLiteral(":text"), QVariant::fromValue(message.text_qstring()));
//        qry.bindValue(QStringLiteral(":status"), 0);

//        if (!qry.exec())
//        {
//            std::cout << "Error! Can not insert values!" <<
//                      << QSqlQuery::lastError().toStdString() << std::endl;
//        }
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

    chat_handle_t add_chat(Chat chat)
    {
        QSqlQuery qry(QSqlDatabase::database(DB_NAME));

        if(!qry.prepare(QStringLiteral("INSERT INTO chats"
                                   " VALUES"
                                   "(:chat_id,"
                                   ":domain_id,"
                                    ":name)"))
                )
        {
            std::cout << "Fail preparing inserting chat!" << std::endl;
            std::cout << qry.lastError().text().toStdString() << std::endl;
        }

        std::uint64_t chat_id = max_chat_id() + 1;
        qry.bindValue(QStringLiteral(":chat_id"), QVariant::fromValue(chat_id));
        qry.bindValue(QStringLiteral(":domain_id"), QVariant::fromValue(chat.domain_id()));
        qry.bindValue(QStringLiteral(":name"), chat.name_qstring());

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

        m_chats.emplace_back(std::move(chat));
        return std::prev(m_chats.end());
    }

    chat_handle_t delete_chat(chat_handle_t chat_handle)
    {
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
