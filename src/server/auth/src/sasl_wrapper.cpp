#include <sasl_wrapper.hpp>

#include <utility>
#include <stdexcept>
#include <cstring>

namespace melon::server::auth
{

SaslServer::SaslServer(std::string service)
    : m_service(std::move(service))
{
    sasl_server_new(m_service.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, &m_conn);
}

std::string_view SaslServer::start(std::string_view chosen_mechanism)
{
    const char* serverout;
    unsigned serverout_len;
    int res = sasl_server_start(m_conn, chosen_mechanism.data(), nullptr, 0, &serverout, &serverout_len);

    if (res != SASL_OK)
        throw std::runtime_error("Sasl server start exit code " + std::to_string(res));

    return { serverout, serverout_len };
}

SaslServer::~SaslServer()
{
    sasl_dispose(&m_conn);
}

std::string_view SaslServer::list_mechanisms()
{
    const char* data;
    unsigned plen;
    int pcount;
    int res = sasl_listmech(m_conn, nullptr, nullptr, " ", nullptr, &data, &plen, &pcount);

    if (res != SASL_OK)
        throw std::runtime_error("Sasl listmech exit code " + std::to_string(res));

    return { data, plen };
}

sasl_conn_t* SaslServer::conn()
{
    return m_conn;
}

const sasl_conn_t* SaslServer::conn() const
{
    return m_conn;
}

//std::string_view SaslServer::get_username()
//{
//    const char* username;

//    int res = sasl_getprop(m_conn, SASL_USERNAME, reinterpret_cast<const void**>(&username));

//    if (res != SASL_OK)
//        throw std::runtime_error("Sasl getprop exit code " + std::to_string(res));

//    return { username };
//}



//SaslClient::SaslClient(std::string service, std::string login, std::string username, std::string password)
//    : m_service(std::move(service)), m_login(std::move(login)), m_username(std::move(username)), m_password(std::move(password))
//{



//    secret = (sasl_secret_t*)malloc(sizeof(*secret) + get_password().length());
//    secret->len = m_password.length();
//    memcpy(secret->data, m_password.c_str(), m_password.length() + 1);

//    //int res = sasl_client_init(m_callbacks.data());
//    int res = sasl_client_init(m_callbacks);

//    if (res != SASL_OK)
//        throw std::runtime_error("Sasl client init exit code " + std::to_string(res));

//    res = sasl_client_new(m_service.c_str(), nullptr, nullptr, nullptr, nullptr,  0, &m_conn);

//    if (res != SASL_OK)
//        throw std::runtime_error("Sasl client new exit code " + std::to_string(res));

//}

SaslClient::SaslClient(std::string service, std::string login, std::string username, std::string password)
    : m_service(std::move(service)), m_login(std::move(login)), m_username(std::move(username)), m_password(std::move(password))
{
    m_callbacks[0] = {SASL_CB_USER, (sasl_callback_ft)&sasl_getsimple, this};
    m_callbacks[1] = {SASL_CB_AUTHNAME, (sasl_callback_ft)&sasl_getsimple, this};
    m_callbacks[2] = {SASL_CB_PASS, (sasl_callback_ft)&sasl_getsecret, this};
    m_callbacks[3] = {SASL_CB_LIST_END, nullptr, nullptr};

    secret = (sasl_secret_t*)malloc(sizeof(*secret) + get_password().length());
    secret->len = m_password.length();
    memcpy(secret->data, m_password.c_str(), m_password.length() + 1);

    //int res = sasl_client_init(m_callbacks.data());
    int res = sasl_client_init(m_callbacks);

    if (res != SASL_OK)
        throw std::runtime_error("Sasl client init exit code " + std::to_string(res));

    res = sasl_client_new(m_service.c_str(), nullptr, nullptr, nullptr, nullptr,  0, &m_conn);

    if (res != SASL_OK)
        throw std::runtime_error("Sasl client new exit code " + std::to_string(res));

    const char* clientout;
    unsigned clientout_len;
    const char* mech;

    res = sasl_client_start(m_conn, "PLAIN", nullptr, &clientout, &clientout_len, &mech);

    if ((res != SASL_CONTINUE) && (res != SASL_OK))
        throw std::runtime_error("Sasl client start exit code " + std::to_string(res));

}

//std::string_view SaslClient::start(std::string mechs)
//{
//    const char* clientout;
//    unsigned clientout_len;
//    const char* mech;
//    int res = sasl_client_start(m_conn, mechs.c_str(), nullptr, &clientout, &clientout_len, &mech);

//    if (res != SASL_OK)
//        throw std::runtime_error("Sasl client start exit code " + std::to_string(res));

//    return { clientout, clientout_len };
//}

SaslClient::~SaslClient()
{
    free(secret);
    sasl_dispose(&m_conn);
}

sasl_conn_t* SaslClient::conn()
{
    return m_conn;
}

const sasl_conn_t* SaslClient::conn() const
{
    return m_conn;
}

const std::string SaslClient::get_password()
{
    return m_password;
}

const std::string SaslClient::get_login()
{
    return m_login;
}

const std::string SaslClient::get_username()
{
    return m_username;
}

const sasl_secret_t* SaslClient::get_sasl_secret()
{
    return m_sasl_secret;
}


int sasl_getsimple(void * context, int id, const char ** result, unsigned * len)
{
    auto* client = static_cast<SaslClient*>(context);
    switch (id) {
    case SASL_CB_USER:
        if (result != NULL)
            *result = client->get_login().c_str();
        if (len != NULL)
            *len = client->get_login().length();
        return SASL_OK;

    case SASL_CB_AUTHNAME:
        if (result != NULL)
            *result = client->get_username().c_str();
        if (len != NULL)
            *len = client->get_username().length();
        return SASL_OK;
 }
 return SASL_FAIL;
}

int sasl_getsecret(sasl_conn_t* conn, void* context, int id, sasl_secret_t** psecret)
{
    auto* client = static_cast<SaslClient*>(context);
    switch (id) {
    case SASL_CB_PASS:
        if (psecret != nullptr)
            *psecret = (sasl_secret_t*)client->get_sasl_secret();
        return SASL_OK;
    }
    return SASL_FAIL;
}


}  // namespace melon::server::auth





