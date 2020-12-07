#include <sasl_wrapper.hpp>

#include <utility>
#include <stdexcept>
#include <iostream>

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

std::string_view SaslServer::get_username()
{
    const char* username;

    int res = sasl_getprop(m_conn, SASL_USERNAME, reinterpret_cast<const void**>(&username));

    if (res != SASL_OK)
        throw std::runtime_error("Sasl getprop exit code " + std::to_string(res));

    return { username };
}



SaslClient::SaslClient(std::string service)
    : m_service(std::move(service))
{
    int res = sasl_client_init(m_callbacks.data());

    if (res != SASL_OK)
        throw std::runtime_error("Sasl client init exit code " + std::to_string(res));

    res = sasl_client_new(m_service.c_str(), nullptr, nullptr, nullptr, nullptr,  0, &m_conn);

    if (res != SASL_OK)
        throw std::runtime_error("Sasl client new exit code " + std::to_string(res));
}

std::string_view SaslClient::start(std::string_view mechanism)
{
    const char* clientout;
    unsigned clientout_len;
    const char* mech;
    int res = sasl_client_start(m_conn, mechanism.data(), nullptr, &clientout, &clientout_len, &mech);

    if (res != SASL_OK)
        throw std::runtime_error("Sasl client start exit code " + std::to_string(res));

    return { clientout, clientout_len };
}

SaslClient::~SaslClient()
{
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

const std::string SaslClient::get_username()
{
    return m_username;
}

//register callback to allow library to ask for authentification id
int client_getsimple(void* context, int id, const char** result, unsigned* result_len)
{
    auto client = static_cast<SaslClient*>(context);
    if (!result)
        return SASL_BADPARAM;

    switch (id)
    {
    case SASL_CB_AUTHNAME:
    case SASL_CB_USER:
            *result = client->get_username().c_str();
            *result_len = static_cast<unsigned>(client->get_username().size());
        break;
    default:
        return SASL_FAIL;
    }
    return SASL_OK;
}

//int client_getpassword(sasl_conn_t* conn, void* context, int id, sasl_secret_t** out_secret)
//{
//    SaslClient* client = static_cast<SaslClient*>(context);
//    if (!out_secret)
//        return SASL_BADPARAM;

//    sasl_secret_t* password = client->get_password();
//    if (secret == nullptr) //User didn't provide password
//        return SASL_FAIL;
//    *out_secret = secret;
//    return SASL_OK;
//}


}  // namespace melon::server::auth





