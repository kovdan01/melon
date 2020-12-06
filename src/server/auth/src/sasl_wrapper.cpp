#include <sasl_wrapper.hpp>

#include <utility>
#include <stdexcept>

namespace melon::server::auth
{

SaslServer::SaslServer(std::string service)
    : m_service(std::move(service))
{
    sasl_server_new(service.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, &m_conn);
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

}  // namespace melon::server::auth
