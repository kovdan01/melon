#ifndef MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
#define MELON_SERVER_AUTH_SASL_WRAPPER_HPP_

#include <melon/server/auth/export.h>

#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

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


int client_getsimple(void* context, int id, const char** result, unsigned* result_len);
int client_getpassword(sasl_conn_t* conn, void* context, int id, sasl_secret_t** out_secret);

typedef int (*saslcallback)();

class SASL_WRAPPERS_EXPORT SaslClient
{
public:
    SaslClient(std::string service);
    ~SaslClient();

    std::string_view start(std::string_view mechanism);
    //std::string_view get_username();

    [[nodiscard]]const sasl_conn_t* conn() const;
    [[nodiscard]]sasl_conn_t* conn();
    void set_username(std::string username);
    [[nodiscard]]const std::string  get_username();

private:
    std::string m_service;
    std::string username ="user";
    sasl_conn_t* m_conn;
    sasl_callback_t m_callbacks[4] =
    {
      {SASL_CB_AUTHNAME, saslcallback(client_getsimple), this},
      {SASL_CB_USER, saslcallback(client_getsimple), this},
      // {SASL_CB_PASS, (sasl_callback_ft)&client_getpassword, this},
      {SASL_CB_LIST_END, nullptr, nullptr}
    };
};

}  // namespace melon::server::auth

#endif  // MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
