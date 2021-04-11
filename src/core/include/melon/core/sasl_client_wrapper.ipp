#include <melon/core/exception.hpp>

#include <boost/asio/ip/host_name.hpp>
#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>

namespace melon::core::auth
{

inline ConfirmationSingleton& ConfirmationSingleton::get_instance()
{
    static ConfirmationSingleton instance;
    return instance;
}

inline const std::string& ConfirmationSingleton::confirmation_string() const noexcept
{
    return m_token_confirmation_string;
}

inline Credentials::Credentials(std::string username,std::string_view password)
    : m_username{std::move(username)}
    // There is no additional byte for '\0' allocated, as sasl_secret_t already contains a single byte for password
    , m_password{static_cast<sasl_secret_t*>(std::malloc(sizeof(sasl_secret_t) + password.size()))}  // NOLINT cppcoreguidelines-no-malloc
{
    if (m_password == nullptr)
        throw std::bad_alloc{};
    std::memcpy(m_password->data,password.data(), password.size());
    m_password->data[password.size()] = '\0';
    m_password->len = password.size();
}

[[nodiscard]] inline const std::string& Credentials::username() const noexcept
{
    return m_username;
}

[[nodiscard]] inline std::string_view Credentials::password() const noexcept
{
    return { reinterpret_cast<const char*>(m_password->data), m_password->len };
}

[[nodiscard]] inline sasl_secret_t* Credentials::secret() const noexcept
{
    return m_password.get();
}

namespace detail
{

inline void check_sasl_result(sasl_res res, std::string_view function_name)
{
    if (res != SASL_OK && res != SASL_CONTINUE)
    {
        throw melon::core::Exception("Sasl " + std::string(function_name) + " exit code " +
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
        *result = params->username().c_str();
        if (len != nullptr)
            *len = static_cast<unsigned>(params->username().size());
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
    *psecret = params->secret();
    return SASL_OK;
}

}  // namespace callbacks

}  // namespace detail

inline SaslClientConnection::SaslClientConnection(std::string service)
    : m_service(std::move(service))
    , m_hostname(boost::asio::ip::host_name())
{
    std::cerr << "CLIENT HOSTNAME" << m_hostname << std::endl;
    sasl_res res = sasl_client_new(m_service.c_str(), m_hostname.c_str(), nullptr, nullptr, nullptr, 0, &m_conn);
    detail::check_sasl_result(res, "client new");
}

inline SaslClientConnection::~SaslClientConnection()
{
    sasl_dispose(&m_conn);
}

inline SaslClientConnection::StartResult SaslClientConnection::start(std::string_view wanted_mech_list)
{
    const char* out;
    const char* selected_mechanism;
    unsigned len;
    sasl_res res = sasl_client_start(m_conn, wanted_mech_list.data(), nullptr, &out, &len, &selected_mechanism);
    detail::check_sasl_result(res, "client start");

    return { .response = { out, len }, .selected_mechanism = selected_mechanism };
}

inline StepResult SaslClientConnection::step(std::string_view server_response)
{
    const char* clientout;
    unsigned clientout_len;
    sasl_res res = sasl_client_step(m_conn, server_response.data(), static_cast<unsigned>(server_response.size()), nullptr,  &clientout, &clientout_len);
    ++m_step_count;

    detail::check_sasl_result(res, "client step " + std::to_string(m_step_count));

    return { .response = { clientout, clientout_len }, .completness = static_cast<AuthCompletness>(res) };
}

[[nodiscard]] inline const sasl_conn_t* SaslClientConnection::conn() const
{
    return m_conn;
}

[[nodiscard]] inline sasl_conn_t* SaslClientConnection::conn()
{
    return m_conn;
}

inline SaslClientSingleton& SaslClientSingleton::get_instance()
{
    static SaslClientSingleton instance;
    return instance;
}

inline void SaslClientSingleton::set_credentials(Credentials* credentials)
{
    m_callbacks[0].context = credentials;
    m_callbacks[1].context = credentials;
    m_callbacks[2].context = credentials;
}

inline SaslClientSingleton::SaslClientSingleton()
{
    sasl_res res = sasl_client_init(m_callbacks.data());
    detail::check_sasl_result(res, "client init");
}

inline SaslClientSingleton::~SaslClientSingleton()
{
    sasl_server_done();
}

}  // namespace melon::core::auth
