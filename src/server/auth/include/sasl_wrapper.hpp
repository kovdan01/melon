#ifndef MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
#define MELON_SERVER_AUTH_SASL_WRAPPER_HPP_

#include <melon/server/auth/export.h>

#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <string>
#include <string_view>
#include <memory>
#include <array>

namespace melon::server::auth
{

class SASL_WRAPPERS_EXPORT SaslServer
{
public:
    SaslServer(std::string service);
    ~SaslServer();

    std::string_view list_mechanisms();
    std::string_view start(std::string_view chosen_mechanism);
    //std::string_view get_username();

    [[nodiscard]]const sasl_conn_t* conn() const;
    [[nodiscard]]sasl_conn_t* conn();

private:
    std::string m_service;
    sasl_conn_t* m_conn;
};


int sasl_getsimple(void* context, int id, const char** result, unsigned* result_len);
int sasl_getsecret(sasl_conn_t* conn, void* context, int id, sasl_secret_t** out_secret);

class SASL_WRAPPERS_EXPORT SaslClient
{
public:
    SaslClient(std::string service, std::string login, std::string username, std::string password);
    ~SaslClient();

    //std::string_view start(std::string mechs);
    [[nodiscard]]const sasl_conn_t* conn() const;
    [[nodiscard]]sasl_conn_t* conn();
    [[nodiscard]]const std::string  get_password();
    [[nodiscard]]const std::string  get_login();
    [[nodiscard]]const std::string  get_username();
    [[nodiscard]]const sasl_secret_t* get_sasl_secret();
    sasl_secret* secret;

private:
    std::string m_service;
    std::string m_login;
    std::string m_username;
    std::string m_password;
    sasl_secret_t* m_sasl_secret;
    sasl_conn_t* m_conn;
    sasl_callback_t m_callbacks[4];

//    std::array<sasl_callback_t, 4> m_callbacks;=
//    {{
//      {SASL_CB_USER, (sasl_callback_ft)&sasl_getsimple, this},
//      {SASL_CB_AUTHNAME, (sasl_callback_ft)&sasl_getsimple, this},
//      {SASL_CB_PASS, (sasl_callback_ft)&sasl_getsecret, this},
//      {SASL_CB_LIST_END, nullptr, nullptr}
//    }};
//    std::array<sasl_callback_t, 4> m_callbacks;


};

}  // namespace melon::server::auth

#endif  // MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
