#ifndef MELON_CORE_AUTH_HPP_
#define MELON_CORE_AUTH_HPP_

#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <array>
#include <string>
#include <string_view>

namespace melon::core::auth
{

using sasl_res = int;

struct Credentials
{
    const std::string username;
    const std::string password;
};

enum class AuthCompletness
{
    COMPLETE = SASL_OK,
    INCOMPLETE = SASL_CONTINUE,
};

struct StepResult
{
    std::string_view response;
    AuthCompletness completness;
};


namespace detail
{

inline void check_sasl_result(sasl_res res, std::string_view function_name);

namespace callbacks
{

inline sasl_res get_username(void* context, int id, const char** result, unsigned* len);
inline sasl_res get_password(sasl_conn_t*, void* context, int id, sasl_secret_t** psecret);

}  // namespace callbacks

}  // namespace detail

class SaslClientConnection
{
public:
    SaslClientConnection(std::string service);
    ~SaslClientConnection();

    struct StartResult
    {
        std::string_view response;
        std::string_view selected_mechanism;
    };

    StartResult start(std::string_view wanted_mech_list);
    StepResult step(std::string_view server_response);
    [[nodiscard]] const sasl_conn_t* conn() const;
    [[nodiscard]] sasl_conn_t* conn();

private:
    const std::string m_service;
    std::size_t m_step_count = 0;
    sasl_conn_t* m_conn;
};

class SaslClientSingleton
{
public:
    static SaslClientSingleton& get_instance();

    SaslClientSingleton(const SaslClientSingleton& root) = delete;
    SaslClientSingleton& operator=(const SaslClientSingleton&) = delete;
    SaslClientSingleton(SaslClientSingleton&& root) = delete;
    SaslClientSingleton& operator=(SaslClientSingleton&&) = delete;

    void set_credentials(Credentials* credentials);

private:
    SaslClientSingleton();
    ~SaslClientSingleton();

    std::array<sasl_callback_t, 4> m_callbacks =
    {
        sasl_callback_t{ .id = SASL_CB_USER,     .proc = reinterpret_cast<sasl_callback_ft>(&detail::callbacks::get_username), .context = nullptr },
        sasl_callback_t{ .id = SASL_CB_AUTHNAME, .proc = reinterpret_cast<sasl_callback_ft>(&detail::callbacks::get_username), .context = nullptr },
        sasl_callback_t{ .id = SASL_CB_PASS,     .proc = reinterpret_cast<sasl_callback_ft>(&detail::callbacks::get_password), .context = nullptr },
        sasl_callback_t{ .id = SASL_CB_LIST_END, .proc = nullptr,                                                              .context = nullptr }
    };
};

}  // namespace melon::core::auth

#include "auth.ipp"

#endif // MELON_CORE_AUTH_HPP_
