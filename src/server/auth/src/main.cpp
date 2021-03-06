#include <sasl_server_wrapper.hpp>

#include <cassert>
#include <iostream>

int main() try
{
    namespace mca = melon::core::auth;
    namespace msa = melon::server::auth;

    [[maybe_unused]] auto& server_singletone = msa::SaslServerSingleton::get_instance();

    auto& client_singletone = mca::SaslClientSingleton::get_instance();
    mca::Credentials credentials = { "john", "doe" };
    client_singletone.set_credentials(&credentials);

    msa::SaslServerConnection server("melon");
    std::string_view supported_mechanisms = server.list_mechanisms();

    mca::SaslClientConnection client("melon");
    const std::string wanted_mechanism = "SCRAM-SHA-256";
    if (supported_mechanisms.find(wanted_mechanism) == std::string_view::npos)
        throw std::runtime_error("Wanted machanism " + wanted_mechanism + " is not supported by server. Supported mechanisms: " + std::string(supported_mechanisms));

    auto [client_response,  selected_mechanism] = client.start(wanted_mechanism);
    assert(selected_mechanism == wanted_mechanism);

    auto [server_response, server_completness] = server.start(selected_mechanism, client_response);

    while (server_completness == mca::AuthCompletness::INCOMPLETE)
    {
        std::cout << "Performing extra auth step..." << std::endl;
        mca::StepResult client_step_res = client.step(server_response);
        mca::StepResult server_step_res = server.step(client_step_res.response);
        server_response = server_step_res.response;
        server_completness = server_step_res.completness;
    }

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
