#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>

namespace melon::server::auth
{
inline SaslServerConnection::SaslServerConnection(std::string service)
    : m_service(std::move(service))
{
    sasl_res res = sasl_server_new(m_service.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, &m_conn);
    detail::check_sasl_result(res, "server new");
}

inline SaslServerConnection::~SaslServerConnection()
{
    sasl_dispose(&m_conn);
}

[[nodiscard]] inline std::string_view SaslServerConnection::list_mechanisms() const
{
    const char* data;
    unsigned plen;
    sasl_res res = sasl_listmech(m_conn, nullptr, nullptr, " ", nullptr, &data, &plen, nullptr);

    detail::check_sasl_result(res, "listmech");

    return { data, plen };
}

inline StepResult SaslServerConnection::start(std::string_view chosen_mechanism, std::string_view client_initial_response)
{
    const char* serverout;
    unsigned serverout_len;
    sasl_res res = sasl_server_start(m_conn, chosen_mechanism.data(), client_initial_response.data(),
                                     static_cast<unsigned>(client_initial_response.size()), &serverout, &serverout_len);

    detail::check_sasl_result(res, "server start");

    return { .response = { serverout, serverout_len }, .completness = static_cast<AuthCompletness>(res) };
}


inline StepResult SaslServerConnection::step(std::string_view client_response)
{
    const char* serverout;
    unsigned serverout_len;
    sasl_res res = sasl_server_step(m_conn, client_response.data(), static_cast<unsigned>(client_response.size()), &serverout, &serverout_len);
    ++m_step_count;

    detail::check_sasl_result(res, "server step" + std::to_string(m_step_count));

    return { .response = { serverout, serverout_len }, .completness = static_cast<AuthCompletness>(res) };
}

[[nodiscard]] inline const sasl_conn_t* SaslServerConnection::conn() const
{
    return m_conn;
}

[[nodiscard]] inline sasl_conn_t* SaslServerConnection::conn()
{
    return m_conn;
}

inline SaslServerSingleton& SaslServerSingleton::get_instance()
{
    static SaslServerSingleton instance;
    return instance;
}

inline SaslServerSingleton::SaslServerSingleton()
{
    sasl_res res = sasl_server_init(nullptr, "localserver");
    detail::check_sasl_result(res, "server init");
}

inline SaslServerSingleton::~SaslServerSingleton()
{
    sasl_server_done();
}

}  // namespace melon::server::auth
