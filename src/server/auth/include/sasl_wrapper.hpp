#ifndef MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
#define MELON_SERVER_AUTH_SASL_WRAPPER_HPP_

#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace melon::server::auth
{

using sasl_res = int;

struct Credentials
{
    const std::string username;
    const std::string password;
};

enum auth_completness
{
    complete = SASL_OK,
    incomplete = SASL_CONTINUE,
};

struct StepResult
{
    auth_completness completness;
    std::string_view response;
};

namespace detail
{

inline void check_sasl_result(sasl_res res, std::string_view function_name)
{
    if (res != SASL_OK && res != SASL_CONTINUE)
    {
        throw std::runtime_error("Sasl " + std::string(function_name) + " exit code " +
                                 std::to_string(res) + ": " + sasl_errstring(res, nullptr, nullptr));
    }
}

namespace callbacks
{

inline sasl_res get_username(void* context, int id, const char** result, unsigned* len)
{
    auto* params = static_cast<Credentials*>(context);
    switch (id)
    {
    case SASL_CB_USER:
    case SASL_CB_AUTHNAME:
        // std::cerr << "Setting username to " << params->username << std::endl;
        *result = params->username.c_str();
        if (len != nullptr)
            *len = static_cast<unsigned>(params->username.size());
        break;
    default:
        return SASL_BADPARAM;
    }
    return SASL_OK;
}

inline sasl_res get_password(sasl_conn_t*, void* context, int id, sasl_secret_t** psecret)
{
    if (id != SASL_CB_PASS)
        return SASL_BADPARAM;
    auto* params = static_cast<Credentials*>(context);
    // std::cerr << "Setting pass to " << params->password << std::endl;

    static auto* secret = reinterpret_cast<sasl_secret_t*>(std::malloc(sizeof(sasl_secret_t) + params->password.size() + 1));  // NOLINT cppcoreguidelines-no-malloc
    if (secret == nullptr)
        return SASL_NOMEM;
    secret->len = params->password.size();
    std::memcpy(secret->data, params->password.c_str(), secret->len + 1);
    *psecret = secret;

    return SASL_OK;
}

}  // namespace callbacks

}  // namespace detail

class SaslServer
{
public:
    SaslServer(std::string service)
        : m_service(std::move(service))
    {
        sasl_res res = sasl_server_new(m_service.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, &m_conn);
        detail::check_sasl_result(res, "server new");
    }

    ~SaslServer()
    {
        sasl_dispose(&m_conn);
    }

    std::string_view list_mechanisms()
    {
        const char* data;
        unsigned plen;
        int pcount;
        sasl_res res = sasl_listmech(m_conn, nullptr, nullptr, " ", nullptr, &data, &plen, &pcount);

        detail::check_sasl_result(res, "listmech");

        return { data, plen };
    }

    StepResult start(std::string_view chosen_mechanism, std::string_view client_initial_response)
    {
        const char* serverout;
        unsigned serverout_len;
        sasl_res res = sasl_server_start(m_conn, chosen_mechanism.data(), client_initial_response.data(),
                                         static_cast<unsigned>(client_initial_response.size()), &serverout, &serverout_len);

        detail::check_sasl_result(res, "server start");

        return {.completness = static_cast<auth_completness>(res), .response = { serverout, serverout_len }};
    }


    StepResult step(std::string_view client_response)
    {
        const char* serverout;
        unsigned serverout_len;
        sasl_res res = sasl_server_step(m_conn, client_response.data(), static_cast<unsigned>(client_response.size()), &serverout, &serverout_len);
        ++m_step_count;

        detail::check_sasl_result(res, "server step" + std::to_string(m_step_count));

        return {.completness = static_cast<auth_completness>(res), .response = { serverout, serverout_len }};
    }

    [[nodiscard]] const sasl_conn_t* conn() const
    {
        return m_conn;
    }

    [[nodiscard]] sasl_conn_t* conn()
    {
        return m_conn;
    }

private:
    const std::string m_service;
    size_t m_step_count = 0;
    sasl_conn_t* m_conn;
};

class SaslServerSingleton
{
public:
    SaslServerSingleton()
    {
        sasl_res res = sasl_server_init(nullptr, "localserver");
        detail::check_sasl_result(res, "server init");
    }

    ~SaslServerSingleton()
    {
        sasl_server_done();
    }

private:
    static SaslServerSingleton* g_instance;
};

class SaslClient
{
public:
    SaslClient(std::string service)
        : m_service(std::move(service))
    {
        sasl_res res = sasl_client_new(m_service.c_str(), nullptr, nullptr, nullptr, nullptr, 0, &m_conn);
        detail::check_sasl_result(res, "client new");
    }

    ~SaslClient()
    {
        sasl_dispose(&m_conn);
    }

    struct StartResult
    {
        std::string_view response;
        std::string_view selected_mechanism;
    };

    StartResult start(std::string_view wanted_mech_list)
    {
        const char* out;
        const char* selected_mechanism;
        unsigned len;
        sasl_res res = sasl_client_start(m_conn, wanted_mech_list.data(), nullptr, &out, &len, &selected_mechanism);
        detail::check_sasl_result(res, "client start");

        return StartResult{ .response = { out, len }, .selected_mechanism = selected_mechanism };
    }

    StepResult step(std::string_view server_response)
    {
        const char* clientout;
        unsigned clientout_len;
        sasl_res res = sasl_client_step(m_conn, server_response.data(), static_cast<unsigned>(server_response.size()), nullptr,  &clientout, &clientout_len);
        ++m_step_count;

        detail::check_sasl_result(res, "server step" + std::to_string(m_step_count));

        return {.completness = static_cast<auth_completness>(res), .response = { clientout, clientout_len }};
    }

    [[nodiscard]] const sasl_conn_t* conn() const
    {
        return m_conn;
    }

    [[nodiscard]] sasl_conn_t* conn()
    {
        return m_conn;
    }

private:
    const std::string m_service;
    size_t m_step_count = 0;
    sasl_conn_t* m_conn;
};

class SaslCientSingleton
{
public:
    SaslCientSingleton()
    {
        sasl_res res = sasl_client_init(m_callbacks.data());
        detail::check_sasl_result(res, "client init");
    }

    void set_credentials(Credentials* credentials)
    {
        m_callbacks[0].context = credentials;
        m_callbacks[1].context = credentials;
        m_callbacks[2].context = credentials;
    }

    ~SaslCientSingleton()
    {
        sasl_server_done();
    }

private:
    std::array<sasl_callback_t, 4> m_callbacks =
    {
        sasl_callback_t{ .id = SASL_CB_USER,     .proc = reinterpret_cast<sasl_callback_ft>(&detail::callbacks::get_username), .context = nullptr },
        sasl_callback_t{ .id = SASL_CB_AUTHNAME, .proc = reinterpret_cast<sasl_callback_ft>(&detail::callbacks::get_username), .context = nullptr },
        sasl_callback_t{ .id = SASL_CB_PASS,     .proc = reinterpret_cast<sasl_callback_ft>(&detail::callbacks::get_password), .context = nullptr },
        sasl_callback_t{ .id = SASL_CB_LIST_END, .proc = nullptr,                                                              .context = nullptr }
    };
};

}  // namespace melon::server::auth

#endif  // MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
