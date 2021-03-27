#include <db_storage.hpp>

#include <iostream>

namespace melon::client_desktop
{


/* Helpers */

std::uint64_t max_message_id(std::uint64_t chat_id, std::uint64_t domain_id);
std::uint64_t max_chat_id(std::uint64_t domain_id);
int status_to_int(melon::core::Message::Status status);
melon::core::Message::Status int_to_status(int status);


/* Message */

// For Insert
Message::Message(std::uint64_t chat_id, std::uint64_t domain_id_chat,
                 std::uint64_t user_id, std::uint64_t domain_id_user,
                 QString text, timestamp_t timestamp, Status status)
    : MessageRAM(chat_id, domain_id_chat, user_id, domain_id_user, text, timestamp, status)
{
    this->set_message_id(max_message_id(this->chat_id(), this->domain_id_chat()) + 1);

    QSqlQuery qry(QSqlDatabase::database(DB_NAME));
    if (!qry.prepare(QStringLiteral("INSERT INTO messages"
                                   " VALUES("
                                   " :msg_id,"
                                   " :chat_id,"
                                   " :domain_id,"
                                   " :user_id,"
                                   " :timestamp,"
                                   " :text,"
                                   " :status)")))
    {
        std::cout << "Fail preparing inserting message!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
    qry.bindValue(QStringLiteral(":msg_id"), QVariant::fromValue(this->message_id()));
    qry.bindValue(QStringLiteral(":chat_id"), QVariant::fromValue(this->chat_id()));
    // TODO: add domain_id_user
    qry.bindValue(QStringLiteral(":domain_id"), QVariant::fromValue(this->domain_id_chat()));
    qry.bindValue(QStringLiteral(":user_id"), QVariant::fromValue(this->user_id()));

    auto timestamp_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(this->timestamp());
    std::uint64_t timestamp_ms_int = timestamp_ms.time_since_epoch().count();
    qry.bindValue(QStringLiteral(":timestamp"), QVariant::fromValue(timestamp_ms_int));

    qry.bindValue(QStringLiteral(":text"), this->text());
    qry.bindValue(QStringLiteral(":status"), status_to_int(this->status()));

    if (!qry.exec())
    {
        std::cout << "Fail inserting message!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
}

//For Select
Message::Message(std::uint64_t message_id, std::uint64_t chat_id, std::uint64_t domain_id_chat)
    : MessageRAM(message_id, chat_id, domain_id_chat)
{
    QSqlQuery qry(DB_NAME);
    QString qry_string = QStringLiteral("SELECT user_id, timestamp, text, status FROM messages"
"                                            WHERE (message_id=")
                        + QString::number(this->message_id())
                        + QStringLiteral(" and chat_id=")
                        + QString::number(this->chat_id())
                        + QStringLiteral(" and domain_id=")
                        + QString::number(this->domain_id_chat())
                        + QStringLiteral(")");
    if (!qry.exec(qry_string))
    {
        std::cout << "Fail loading message!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
    else
    {
        qry.next();
        this->set_user_id(qry.value(0).value<std::uint64_t>());
        std::uint64_t tp = qry.value(1).value<std::uint64_t>();
        std::chrono::milliseconds dur(tp);
        std::chrono::time_point<std::chrono::system_clock> tp_ms(dur);
        this->set_timestamp(tp_ms);
        this->set_text(qry.value(2).toString());
        this->set_status(int_to_status(qry.value(3).toInt()));
    }
    this->set_from();
}

void Message::set_text_impl()
{
    QSqlQuery qry(DB_NAME);
    QString qry_string = QStringLiteral("UPDATE messages SET text='") + this->text() +
                         QStringLiteral("' WHERE message_id=") + QString::number(this->message_id())
                        + QStringLiteral(" and chat_id=") + QString::number(this->chat_id())
                        + QStringLiteral(" and domain_id=") + QString::number(this->domain_id_chat());
    if (!qry.exec(qry_string))
    {
        std::cout << "Fail updating message text!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
}

void Message::remove_from_db()
{
    QSqlQuery qry(DB_NAME);

    QString query_string = QStringLiteral("DELETE FROM messages WHERE message_id=") + QString::number(this->message_id())
                           + QStringLiteral(" and chat_id=") + QString::number(this->chat_id())
                           + QStringLiteral(" and domain_id=") + QString::number(this->domain_id_chat());

    if (!qry.exec(query_string))
    {
        std::cout << "Fail deleting message!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
}


/* Chat */

// For Insert
Chat::Chat(std::uint64_t domain_id, QString name)
    : ChatRAM(domain_id, name)
    , m_incomplete_message(QLatin1String(""))
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
    qry.bindValue(QStringLiteral(":name"), this->chatname());

    if (!qry.exec())
    {
        std::cout << "Fail inserting chat!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
}

// For Select
Chat::Chat(std::uint64_t chat_id, std::uint64_t domain_id)
    : ChatRAM(chat_id, domain_id)
    , m_incomplete_message(QLatin1String(""))
{
    QSqlQuery qry(DB_NAME);
    QString qry_string = QStringLiteral("SELECT name FROM chats WHERE chat_id=") + QString::number(this->chat_id())
                        + QStringLiteral(" and domain_id=") + QString::number(this->domain_id());
    if (!qry.exec(qry_string))
    {
        std::cout << "Fail loading chat!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
    else
    {
        qry.next();
        this->set_chatname(qry.value(0).toString());
    }
}

void Chat::remove_from_db()
{
    QSqlQuery qry(DB_NAME);

    QString query_string = QStringLiteral("DELETE FROM chats WHERE chat_id=") + QString::number(this->chat_id())
                           + QStringLiteral(" and domain_id=") + QString::number(this->domain_id());

    if (!qry.exec(query_string))
    {
        std::cout << "Fail deleting chat!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }

    // Deleting messages, because cascade deleting seems not working
    query_string = QStringLiteral("DELETE FROM messages WHERE chat_id=") + QString::number(this->chat_id())
                           + QStringLiteral(" and domain_id=") + QString::number(this->domain_id());
    if (!qry.exec(query_string))
    {
        std::cout << "Fail deleting messages on cascade chat!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
}

void Chat::set_chatname_impl()
{
    QSqlQuery qry(DB_NAME);
    QString qry_string = QStringLiteral("UPDATE chats SET name='") + this->chatname()
                        + QStringLiteral("' WHERE chat_id=") + QString::number(this->chat_id())
                        + QStringLiteral(" and domain_id=") + QString::number(this->domain_id());
    if (!qry.exec(qry_string))
    {
        std::cout << "Fail updating chatname!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
}



/* Helpers */

std::uint64_t max_message_id(std::uint64_t chat_id, std::uint64_t domain_id)
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

std::uint64_t max_chat_id(std::uint64_t domain_id)
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

int status_to_int(melon::core::Message::Status status)
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

melon::core::Message::Status int_to_status(int status)
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

}  // namespace melon::client_desktop
