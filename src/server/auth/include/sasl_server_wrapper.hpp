#ifndef MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
#define MELON_SERVER_AUTH_SASL_WRAPPER_HPP_

#include <melon/core/sasl_client_wrapper.hpp>

namespace melon::server::auth
{

using namespace melon::core::auth;

class SaslServerConnection
{
public:
    SaslServerConnection(std::string service);
    ~SaslServerConnection();

    [[nodiscard]] std::string_view list_mechanisms() const;
    StepResult start(std::string_view chosen_mechanism, std::string_view client_initial_response);
    StepResult step(std::string_view client_response);
    [[nodiscard]] const sasl_conn_t* conn() const;
    [[nodiscard]] sasl_conn_t* conn();

private:
    const std::string m_service;
    std::size_t m_step_count = 0;
    sasl_conn_t* m_conn;
};

class SaslServerSingleton
{
public:
    static SaslServerSingleton& get_instance();

    SaslServerSingleton(const SaslServerSingleton& root) = delete;
    SaslServerSingleton& operator=(const SaslServerSingleton&) = delete;
    SaslServerSingleton(SaslServerSingleton&& root) = delete;
    SaslServerSingleton& operator=(SaslServerSingleton&&) = delete;

private:
    SaslServerSingleton();
    ~SaslServerSingleton();
};

}  // namespace melon::server::auth

#include "sasl_server_wrapper.ipp"

#endif  // MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
