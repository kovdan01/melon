#ifndef MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
#define MELON_SERVER_AUTH_SASL_WRAPPER_HPP_

#include <melon/server/auth/export.h>

#include <sasl/saslutil.h>

#include <string>
#include <string_view>
#include <memory>

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

    [[nodiscard]]const sasl_conn_t* conn() const;
    [[nodiscard]]sasl_conn_t* conn();

private:
    std::string m_service;
    sasl_conn_t* m_conn;
};



class SASL_WRAPPERS_EXPORT SaslClient
{
public:
    SaslClient(std::string service);
    ~SaslClient();

    std::string_view start(std::string_view mechanism);
    //std::string_view get_username();

    [[nodiscard]]const sasl_conn_t* conn() const;
    [[nodiscard]]sasl_conn_t* conn();
    std::string username;
    //int client_password(sasl_conn_t* conn, void* context, int id, sasl_secret_t** out_secret);

private:
    std::string m_service;
    sasl_conn_t* m_conn;
    sasl_callback_t m_callbacks[2];
    std::unique_ptr<char[]> m_password;
};



}  // namespace melon::server::auth

#endif  // MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
