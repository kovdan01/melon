#include <storage_singletones.hpp>

namespace melon::client_desktop
{

DBSingletone::DBSingletone()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));

    auto& storage = StorageNameSingletone::get_instance();
    QString db_name = storage.db_name();

    db.setDatabaseName(db_name);

    bool exists = QFile::exists(db_name);

    if (!db.open())
        throw QtSqlException("Fail opening database");

    QSqlQuery query(db);

    exec_and_check_qtsql_query(query, QStringLiteral("PRAGMA foreign_keys=ON"), "Foreign keys ON");

    if (exists)
    {
        m_me = User(QStringLiteral("MelonUser"), 1);
        m_another_user = User(QStringLiteral("SomeSender"), 1);
        m_my_domain = Domain(QStringLiteral("melon"));
        return;
    }

    QString qry_string = QStringLiteral("CREATE TABLE [domains]"
                                        "(domain_id INT NOT NULL,"
                                        " hostname TEXT,"
                                        " external INT,"
                                        " PRIMARY KEY (domain_id))");
    exec_and_check_qtsql_query(query, qry_string, "Creating [domains] table");

    qry_string = QStringLiteral("CREATE TABLE [users]"
                                "(user_id INT NOT NULL,"
                                " username TEXT,"
                                " status INT,"
                                " domain_id INT NOT NULL,"
                                " PRIMARY KEY (user_id, domain_id),"
                                " FOREIGN KEY (domain_id) REFERENCES domains(domain_id) ON DELETE CASCADE)");
    exec_and_check_qtsql_query(query, qry_string, "Creating [users] table");

    qry_string = QStringLiteral("CREATE TABLE [chats]"
                                "(chat_id INT NOT NULL,"
                                " domain_id INT NOT NULL,"
                                " name TEXT,"
                                " PRIMARY KEY (chat_id, domain_id),"
                                " FOREIGN KEY (domain_id) REFERENCES domains(domain_id) ON DELETE CASCADE)");
    exec_and_check_qtsql_query(query, qry_string, "Creating [chats] table");


    qry_string = QStringLiteral("CREATE TABLE [messages]"
                                "(message_id int NOT NULL,"
                                " chat_id INT NOT NULL,"
                                " domain_id_chat INT NOT NULL,"
                                " user_id INT,"
                                " domain_id_user INT,"
                                " timestamp INT,"
                                " text TEXT,"
                                " status INT, "
                                " edit INT, "
                                " PRIMARY KEY (message_id, chat_id, domain_id_chat),"
                                " FOREIGN KEY (chat_id, domain_id_chat) REFERENCES chats(chat_id, domain_id) ON DELETE CASCADE,"
                                " FOREIGN KEY (user_id, domain_id_user) REFERENCES users(user_id, domain_id) ON DELETE CASCADE)");
    exec_and_check_qtsql_query(query, qry_string, "Creating [messages] table");

    m_my_domain = Domain(QStringLiteral("melon"), false);
    m_me = User(QStringLiteral("MelonUser"), 1, User::Status::ONLINE);
    m_another_user = User(QStringLiteral("SomeSender"), 1, User::Status::ONLINE);
}

}  // namespace melon::client_desktop
