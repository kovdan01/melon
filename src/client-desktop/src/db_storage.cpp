#include <db_storage.hpp>

#include <iostream>

namespace melon::client_desktop
{

namespace mc = melon::core;

/* Helpers */

melon::core::id_t max_message_id(melon::core::id_t chat_id, melon::core::id_t domain_id);
melon::core::id_t max_chat_id(melon::core::id_t domain_id);
int status_to_int(melon::core::Message::Status status);
melon::core::Message::Status int_to_status(int status);


/* Message */

// For Insert
Message::Message(id_t chat_id, id_t domain_id_chat,
                 id_t user_id, id_t domain_id_user,
                 const QString& text, timestamp_t timestamp, Status status)
    : MessageRAM(chat_id, domain_id_chat, user_id, domain_id_user, text, timestamp, status)
{
    this->set_message_id(max_message_id(this->chat_id(), this->domain_id_chat()) + 1);

    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
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
Message::Message(id_t message_id, id_t chat_id, id_t domain_id_chat)
    : MessageRAM(message_id, chat_id, domain_id_chat)
{
    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
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
        this->set_user_id(qry.value(0).value<id_t>());
        auto tp = qry.value(1).value<std::uint64_t>();
        std::chrono::milliseconds dur(tp);
        std::chrono::time_point<std::chrono::system_clock> tp_ms(dur);
        this->set_timestamp(tp_ms);
        this->set_text(qry.value(2).toString());
        this->set_status(int_to_status(qry.value(3).toInt()));
    }
    this->set_from();
}

void Message::set_text(const QString& text)
{
    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("UPDATE messages SET text='") + text +
                         QStringLiteral("' WHERE message_id=") + QString::number(this->message_id())
                        + QStringLiteral(" and chat_id=") + QString::number(this->chat_id())
                        + QStringLiteral(" and domain_id=") + QString::number(this->domain_id_chat());
    if (!qry.exec(qry_string))
    {
        std::cout << "Fail updating message text!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
    mc::Message::set_text(text.toStdString());
}

void Message::remove_from_db()
{
    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
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
Chat::Chat(id_t domain_id, const QString& name)
    : ChatRAM(domain_id, name)
    , m_incomplete_message(QLatin1String(""))
{
    this->set_chat_id(max_chat_id(this->domain_id()) + 1);

    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
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
Chat::Chat(id_t chat_id, id_t domain_id)
    : ChatRAM(chat_id, domain_id)
{
    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
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
    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
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

void Chat::set_chatname(const QString& chatname)
{
    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("UPDATE chats SET name='") + chatname
                        + QStringLiteral("' WHERE chat_id=") + QString::number(this->chat_id())
                        + QStringLiteral(" and domain_id=") + QString::number(this->domain_id());
    if (!qry.exec(qry_string))
    {
        std::cout << "Fail updating chatname!" << std::endl;
        std::cout << qry.lastError().text().toStdString() << std::endl;
    }
    mc::Chat::set_chatname(chatname.toStdString());
}



/* Helpers */

melon::core::id_t max_message_id(melon::core::id_t chat_id, melon::core::id_t domain_id)
{
    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
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
    auto msg_id = msg_id_qvar.value<id_t>();

    return msg_id;
}

melon::core::id_t max_chat_id(melon::core::id_t domain_id)
{
    auto& storage = StorageSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
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
    auto chat_id = chat_id_qvar.value<id_t>();

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
    default:
        return 5;
    }
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
    default:
        return melon::core::Message::Status::FAIL;
    }
}

}  // namespace melon::client_desktop
