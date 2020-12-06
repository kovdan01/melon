#ifndef MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
#define MELON_SERVER_AUTH_SASL_WRAPPER_HPP_

#include <melon/server/auth/export.h>

#include <sasl/saslutil.h>

#include <string>
#include <string_view>

namespace melon::server::auth
{

class SASL_WRAPPERS_EXPORT SaslServer
{
public:
    SaslServer(std::string service);
    ~SaslServer();

    std::string_view list_mechanisms();
    std::string_view start(std::string_view chosen_mechanism);
    std::string_view get_username();

    const sasl_conn_t* conn() const;
    sasl_conn_t* conn();

private:
    std::string m_service;
    sasl_conn_t* m_conn;
};

}  // namespace melon::server::auth

#endif  // MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
