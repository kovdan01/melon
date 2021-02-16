#ifndef MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
#define MELON_SERVER_AUTH_SASL_WRAPPER_HPP_

#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <array>
#include <cstring>
#include <memory>
#include <new> // std::bad_alloc
#include <string>
#include <string_view>

namespace melon::server::auth
{

using sasl_res = int;

class Credentials
{
private:
    constexpr static auto free_deleter = [](void* p){ std::free(p); };

    std::string username_;
    std::unique_ptr<sasl_secret_t,decltype(free_deleter)> password_;
public:
    Credentials(std::string username,std::string_view password)
        : username_{std::move(username)},
          password_{static_cast<sasl_secret_t*>(std::malloc(sizeof(sasl_secret_t)+password.size()))}
    {
        if(!password_)
            throw std::bad_alloc{};
        std::memcpy(password_->data,password.data(),password.size());
        password_->data[password.size()] = '\0';
        password_->len = password.size();
    }

    const std::string& username() const noexcept { return username_; }
    std::string_view password() const noexcept
    {
        return {reinterpret_cast<const char*>(password_->data),password_->len};
    }
    sasl_secret_t* secret() const noexcept { return password_.get(); }
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

}  // namespace melon::server::auth

#include "sasl_wrapper.ipp"

#endif  // MELON_SERVER_AUTH_SASL_WRAPPER_HPP_
