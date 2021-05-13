#include <boost/asio/ip/host_name.hpp>
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
    , m_hostname(boost::asio::ip::host_name())
{
    mca::sasl_res res = sasl_server_new(m_service.c_str(), m_hostname.c_str(), nullptr, nullptr, nullptr, nullptr, 0, &m_conn);
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

inline mca::StepResult SaslServerConnection::start(std::string_view chosen_mechanism, std::span<const melon::core::byte_t> client_initial_response)
{
    const char* serverout;
    unsigned serverout_len;
    mca::sasl_res res = sasl_server_start(m_conn, chosen_mechanism.data(), reinterpret_cast<const char*>(client_initial_response.data()),
                                     static_cast<unsigned>(client_initial_response.size()), &serverout, &serverout_len);
    mca::detail::check_sasl_result(res, "server start");

    return { .response = std::span{ reinterpret_cast<const melon::core::byte_t*>(serverout), serverout_len }, .completness = static_cast<mca::AuthState>(res) };
}


inline mca::StepResult SaslServerConnection::step(std::span<const melon::core::byte_t> client_response)
{
    const char* serverout;
    unsigned serverout_len;
    mca::sasl_res res = sasl_server_step(m_conn, reinterpret_cast<const char*>(client_response.data()), static_cast<unsigned>(client_response.size()), &serverout, &serverout_len);
    ++m_step_count;

    mca::detail::check_sasl_result(res, "server step" + std::to_string(m_step_count));

    return { .response = std::span{ reinterpret_cast<const melon::core::byte_t*>(serverout), serverout_len }, .completness = static_cast<mca::AuthState>(res) };
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
    std::string config_path_string = m_config_path.string();
    sasl_set_path(SASL_PATH_TYPE_CONFIG, const_cast<char*>(config_path_string.c_str()));
    mca::sasl_res res = sasl_server_init(nullptr, "melon");
    mca::detail::check_sasl_result(res, "server init");
}

inline SaslServerSingleton::~SaslServerSingleton()
{
    sasl_server_done();
}

}  // namespace melon::server::auth
