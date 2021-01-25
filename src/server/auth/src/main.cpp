#include <sasl_wrapper.hpp>

#include <cassert>
#include <iostream>

int main() try
{
    namespace msa = melon::server::auth;

    static const msa::SaslServerSingleton server_singletone;

    msa::SaslCientSingleton client_singletone;
    msa::Credentials credentials = { .username = "cat", .password = "dog" };
    client_singletone.set_credentials(&credentials);

    msa::SaslServer server("melon");
    std::string_view supported_mechanisms = server.list_mechanisms();

    msa::SaslClient client("melon");
    const std::string wanted_mechanism = "PLAIN";
    if (supported_mechanisms.find(wanted_mechanism) == std::string_view::npos)
        throw std::runtime_error("Wanted machanism " + wanted_mechanism + " is not supported by server. Supported mechanisms: " + std::string(supported_mechanisms));

    auto [client_response, selected_mechanism] = client.start(wanted_mechanism);
    assert(selected_mechanism == wanted_mechanism);

    [[maybe_unused]] std::string_view server_response = server.start(selected_mechanism, client_response);

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
