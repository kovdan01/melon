#include <db_storage.hpp>

namespace melon::client_desktop
{

namespace mc = melon::core;

QtSqlException::~QtSqlException() = default;

/* Helpers */

melon::core::id_t max_domain_id();
melon::core::id_t max_user_id(id_t domain_id);
melon::core::id_t max_message_id(id_t chat_id, id_t domain_id);
melon::core::id_t max_chat_id(id_t domain_id);

/* Domain */

// For Insert
Domain::Domain(const QString& hostname, bool external)
    :ram::Domain(hostname, external)
{
    this->set_domain_id(max_domain_id() + 1);

    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    prepare_and_check_qtsql_query(qry, QStringLiteral("INSERT INTO domains VALUES(:domain_id, :hostname)"),
                                  "Prepare inserting domain");

    qry.bindValue(QStringLiteral(":domain_id"), QVariant::fromValue(this->domain_id()));
    qry.bindValue(QStringLiteral(":hostname"), this->hostname());

    exec_and_check_qtsql_query(qry, "Inserting domain");
}

// For Select
Domain::Domain(const QString& hostname)
    :ram::Domain(hostname)
{
    auto& storage = DBSingletone::get_instance();
    QSqlQuery qry(storage.db_name());

    QString qry_string = QStringLiteral("SELECT domain_id, external FROM domains WHERE hostname='") + this->hostname() + QStringLiteral("'");
    exec_and_check_qtsql_query(qry, qry_string, "Loading domain");
    qry.next();
    this->set_domain_id(qry.value(0).value<id_t>());
    this->set_external(qry.value(1).value<bool>());
}

void Domain::remove_from_db()
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("DELETE FROM domains WHERE domain_id=") + QString::number(this->domain_id());

    exec_and_check_qtsql_query(qry, qry_string, "Deleting domain");
}


/* User */

// For Insert
User::User(const QString& username, id_t domain_id, Status status)
    :ram::User(username, domain_id, status)
{
    this->set_user_id(max_user_id(this->domain_id()) + 1);

    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    prepare_and_check_qtsql_query(qry, QStringLiteral("INSERT INTO users VALUES(:username, :domain_id, :status)"),
                                  "Prepare inserting user");

    qry.bindValue(QStringLiteral(":username"), this->username());
    qry.bindValue(QStringLiteral(":domain_id"), QVariant::fromValue(this->domain_id()));
    qry.bindValue(QStringLiteral(":status"), static_cast<int>(this->status()));

    exec_and_check_qtsql_query(qry, "Inserting user");
}

// For Select
User::User(const QString& username, id_t domain_id)
    :ram::User(username, domain_id)
{
    auto& storage = DBSingletone::get_instance();
    QSqlQuery qry(storage.db_name());

    QString qry_string = QStringLiteral("SELECT user_id, status FROM users WHERE username='") + this->username() +
                         QStringLiteral("' and domain_id=") + QString::number(this->domain_id());
    exec_and_check_qtsql_query(qry, qry_string, "Loading user");

    qry.next();
    this->set_user_id(qry.value(0).value<id_t>());
    this->set_status(static_cast<melon::core::User::Status>(qry.value(1).toInt()));
}

void User::remove_from_db()
{
    auto& storage = DBSingletone::get_instance();
    QSqlQuery qry(storage.db_name());

    QString qry_string = QStringLiteral("DELETE FROM users WHERE user_id=") + QString::number(this->user_id()) +
                           QStringLiteral(" and domain_id=") + QString::number(this->domain_id());

    exec_and_check_qtsql_query(qry, qry_string, "Deleting user");
}

/* Message */

// For Insert
Message::Message(id_t chat_id, id_t domain_id_chat,
                 id_t user_id, id_t domain_id_user,
                 const QString& text, timestamp_t timestamp, Status status)
    : ram::Message(chat_id, domain_id_chat, user_id, domain_id_user, text, timestamp, status)
{
    this->set_message_id(max_message_id(this->chat_id(), this->domain_id_chat()) + 1);
    this->set_is_edit(false);

    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("INSERT INTO messages"
                                        " VALUES("
                                        " :msg_id,"
                                        " :chat_id,"
                                        " :domain_id_chat,"
                                        " :user_id,"
                                        " :domain_id_user,"
                                        " :timestamp,"
                                        " :text,"
                                        " :status,"
                                        " :edit)");
    prepare_and_check_qtsql_query(qry, qry_string, "Prepare inserting message");

    qry.bindValue(QStringLiteral(":msg_id"), QVariant::fromValue(this->message_id()));
    qry.bindValue(QStringLiteral(":chat_id"), QVariant::fromValue(this->chat_id()));

    qry.bindValue(QStringLiteral(":domain_id_chat"), QVariant::fromValue(this->domain_id_chat()));
    qry.bindValue(QStringLiteral(":user_id"), QVariant::fromValue(this->user_id()));
    qry.bindValue(QStringLiteral(":domain_id_user"), QVariant::fromValue(this->domain_id_user()));

    auto timestamp_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(this->timestamp());
    std::uint64_t timestamp_ms_int = timestamp_ms.time_since_epoch().count();
    qry.bindValue(QStringLiteral(":timestamp"), QVariant::fromValue(timestamp_ms_int));

    qry.bindValue(QStringLiteral(":text"), this->text());
    qry.bindValue(QStringLiteral(":status"), static_cast<int>(this->status()));
    qry.bindValue(QStringLiteral(":edit"), this->is_edit());

    exec_and_check_qtsql_query(qry, "Inserting message");

    this->set_from();
}

//For Select
Message::Message(id_t message_id, id_t chat_id, id_t domain_id_chat)
    : ram::Message(message_id, chat_id, domain_id_chat)
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("SELECT user_id, domain_id_user, timestamp, text, status, edit FROM messages"
"                                        WHERE (message_id=") +
                         QString::number(this->message_id()) +
                         QStringLiteral(" and chat_id=") +
                         QString::number(this->chat_id()) +
                         QStringLiteral(" and domain_id_chat=") +
                         QString::number(this->domain_id_chat()) +
                         QStringLiteral(")");
    exec_and_check_qtsql_query(qry, qry_string, "Loading message");

    qry.next();
    this->set_user_id(qry.value(0).value<id_t>());
    this->set_domain_id_user(qry.value(1).value<id_t>());
    auto tp = qry.value(2).value<std::uint64_t>();
    std::chrono::milliseconds dur(tp);
    std::chrono::time_point<std::chrono::system_clock> tp_ms(dur);
    this->set_timestamp(tp_ms);
    this->set_text(qry.value(3).toString());
    this->set_status(static_cast<melon::core::Message::Status>(qry.value(4).toInt()));
    this->set_is_edit(qry.value(5).toBool());

    this->set_from();
}

void Message::set_text(const QString& text)
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("UPDATE messages SET text='") + text +
                         QStringLiteral("' WHERE message_id=") + QString::number(this->message_id()) +
                         QStringLiteral(" and chat_id=") + QString::number(this->chat_id()) +
                         QStringLiteral(" and domain_id_chat=") + QString::number(this->domain_id_chat());
    exec_and_check_qtsql_query(qry, qry_string, "Updating message text");

    mc::Message::set_text(text.toStdString());
}

void Message::set_from()
{
    auto& storage = UserDomainSingletone::get_instance();
    if (this->user_id() == storage.me().user_id())
        m_from = QObject::tr("Me");
    else
        m_from = storage.another_user().username();
}

void Message::set_is_edit(bool is_edit)
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("UPDATE messages SET edit='") + QString::number(is_edit) +
                         QStringLiteral("' WHERE message_id=") + QString::number(this->message_id()) +
                         QStringLiteral(" and chat_id=") + QString::number(this->chat_id()) +
                         QStringLiteral(" and domain_id_chat=") + QString::number(this->domain_id_chat());
    exec_and_check_qtsql_query(qry, qry_string, "Updating message edit mark");
    m_is_edit = is_edit;
}

void Message::remove_from_db()
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("DELETE FROM messages WHERE message_id=") + QString::number(this->message_id()) +
                           QStringLiteral(" and chat_id=") + QString::number(this->chat_id()) +
                           QStringLiteral(" and domain_id_chat=") + QString::number(this->domain_id_chat());

    exec_and_check_qtsql_query(qry, qry_string, "Deleting message");
}


/* Chat */

// For Insert
Chat::Chat(id_t domain_id, const QString& name)
    : ram::Chat(domain_id, name)
    , m_incomplete_message(QLatin1String(""))
{
    this->set_chat_id(max_chat_id(this->domain_id()) + 1);

    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());

    prepare_and_check_qtsql_query(qry, QStringLiteral("INSERT INTO chats VALUES(:chat_id, :domain_id, :name)"),
                                  "Prepare inserting chat");
    qry.bindValue(QStringLiteral(":chat_id"), QVariant::fromValue(this->chat_id()));
    qry.bindValue(QStringLiteral(":domain_id"), QVariant::fromValue(this->domain_id()));
    qry.bindValue(QStringLiteral(":name"), this->chatname());

    exec_and_check_qtsql_query(qry, "Inserting chat");
}

// For Select
Chat::Chat(id_t chat_id, id_t domain_id)
    : ram::Chat(chat_id, domain_id)
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("SELECT name FROM chats WHERE chat_id=") + QString::number(this->chat_id()) +
                         QStringLiteral(" and domain_id=") + QString::number(this->domain_id());
    exec_and_check_qtsql_query(qry, qry_string, "Loading chat");
    qry.next();
    this->set_chatname(qry.value(0).toString());
}

void Chat::remove_from_db()
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("DELETE FROM chats WHERE chat_id=") + QString::number(this->chat_id()) +
                           QStringLiteral(" and domain_id=") + QString::number(this->domain_id());

    exec_and_check_qtsql_query(qry, qry_string, "Deleting chat");
}

void Chat::set_chatname(const QString& chatname)
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("UPDATE chats SET name='") + chatname +
                         QStringLiteral("' WHERE chat_id=") + QString::number(this->chat_id()) +
                         QStringLiteral(" and domain_id=") + QString::number(this->domain_id());
    exec_and_check_qtsql_query(qry, qry_string, "Updating chatname");

    mc::Chat::set_chatname(chatname.toStdString());
}



/* Helpers */

melon::core::id_t max_domain_id()
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("SELECT MAX(domain_id) FROM domains");
    exec_and_check_qtsql_query(qry, qry_string, "Selecting max domain_id");

    qry.next();
    QVariant chat_id_qvar = qry.value(0);
    if (!chat_id_qvar.isValid())
        return 0;
    auto chat_id = chat_id_qvar.value<id_t>();
    return chat_id;
}

melon::core::id_t max_user_id(id_t domain_id)
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("SELECT MAX(user_id) FROM users WHERE domain_id=") + QString::number(domain_id);
    exec_and_check_qtsql_query(qry, qry_string, "Selecting max user_id");

    qry.next();
    QVariant chat_id_qvar = qry.value(0);
    if (!chat_id_qvar.isValid())
        return 0;
    auto chat_id = chat_id_qvar.value<id_t>();
    return chat_id;
}

melon::core::id_t max_message_id(melon::core::id_t chat_id, melon::core::id_t domain_id)
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("SELECT MAX(message_id) FROM messages where chat_id=") + QString::number(chat_id) +
                         QStringLiteral(" and domain_id_chat=") + QString::number(domain_id);
    exec_and_check_qtsql_query(qry, qry_string, "Selecting max message_id");

    qry.next();
    QVariant msg_id_qvar = qry.value(0);
    if (!msg_id_qvar.isValid())
        return 0;
    auto msg_id = msg_id_qvar.value<id_t>();
    return msg_id;
}

melon::core::id_t max_chat_id(melon::core::id_t domain_id)
{
    auto& storage = DBSingletone::get_instance();

    QSqlQuery qry(storage.db_name());
    QString qry_string = QStringLiteral("SELECT MAX(chat_id) FROM chats WHERE domain_id=") + QString::number(domain_id);
    exec_and_check_qtsql_query(qry, qry_string, "Selecting max chat_id");

    qry.next();
    QVariant chat_id_qvar = qry.value(0);
    if (!chat_id_qvar.isValid())
        return 0;
    auto chat_id = chat_id_qvar.value<id_t>();
    return chat_id;
}


void exec_and_check_qtsql_query(QSqlQuery& qry, const QString& qry_string, const std::string& action)
{
    if (!qry.exec(qry_string))
    {
        std::string error = action + ": " + qry.lastError().text().toStdString();
        throw QtSqlException(error);
    }
}

void exec_and_check_qtsql_query(QSqlQuery& qry, const std::string& action)
{
    if (!qry.exec())
    {
        std::string error = action + ": " + qry.lastError().text().toStdString();
        throw QtSqlException(error);
    }
}

void prepare_and_check_qtsql_query(QSqlQuery& qry, const QString& qry_string, const std::string& action)
{
    if (!qry.prepare(qry_string))
    {
        std::string error = action + ": " + qry.lastError().text().toStdString();
        QMessageBox::critical(nullptr, QObject::tr("Error!"), QObject::tr("Error with local database!\nPlease contact us at bugreport@melon.com."));
        throw QtSqlException(error);
    }
}

}  // namespace melon::client_desktop
