#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>

// debug headers
#include <iostream>
#include <cstdlib>

namespace melon::server::auth
{

inline SaslServerConnection::SaslServerConnection(std::string service)
    : m_service(std::move(service))
{
    mca::sasl_res res = sasl_server_new(m_service.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, &m_conn);
    mca::detail::check_sasl_result(res, "server new");
}

inline SaslServerConnection::~SaslServerConnection()
{
    sasl_dispose(&m_conn);
}

[[nodiscard]] inline std::string_view SaslServerConnection::list_mechanisms() const
{
    const char* data;
    unsigned plen;
    mca::sasl_res res = sasl_listmech(m_conn, nullptr, nullptr, " ", nullptr, &data, &plen, nullptr);

    mca::detail::check_sasl_result(res, "listmech");

    return { data, plen };
}

inline mca::StepResult SaslServerConnection::start(std::string_view chosen_mechanism, std::string_view client_initial_response)
{
    const char* serverout;
    unsigned serverout_len;
    mca::sasl_res res = sasl_server_start(m_conn, chosen_mechanism.data(), client_initial_response.data(),
                                     static_cast<unsigned>(client_initial_response.size()), &serverout, &serverout_len);
    std::cerr<< std::system("sasldblistusers2 -f ~/.melon/sasldb2") << std::endl << "Authorising with" << client_initial_response << std::endl;
    mca::detail::check_sasl_result(res, "server start");

    return { .response = { serverout, serverout_len }, .completness = static_cast<mca::AuthCompletness>(res) };
}


inline mca::StepResult SaslServerConnection::step(std::string_view client_response)
{
    const char* serverout;
    unsigned serverout_len;
    mca::sasl_res res = sasl_server_step(m_conn, client_response.data(), static_cast<unsigned>(client_response.size()), &serverout, &serverout_len);
    ++m_step_count;

    mca::detail::check_sasl_result(res, "server step" + std::to_string(m_step_count));

    return { .response = { serverout, serverout_len }, .completness = static_cast<mca::AuthCompletness>(res) };
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
    m_config_path /= std::filesystem::path(std::getenv("HOME")) / ".melon";
    sasl_set_path(SASL_PATH_TYPE_CONFIG, const_cast<char*>(m_config_path.c_str()));
    mca::sasl_res res = sasl_server_init(nullptr, "localserver");
    mca::detail::check_sasl_result(res, "server init");
}

inline SaslServerSingleton::~SaslServerSingleton()
{
    sasl_server_done();
}

}  // namespace melon::server::auth
