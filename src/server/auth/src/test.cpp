#include <sasl_wrapper.hpp>

#include <cassert>
#include <iostream>

int test() try
{


    namespace msa = melon::server::auth;

    [[maybe_unused]] int x = msa::a();

    [[maybe_unused]] auto& server_singletone = msa::SaslServerSingleton::get_instance();

    auto& client_singletone = msa::SaslClientSingleton::get_instance();
    msa::Credentials credentials = { .username = "john", .password = "doe" };
    client_singletone.set_credentials(&credentials);

    msa::SaslServerConnection server("melon");
    std::string_view supported_mechanisms = server.list_mechanisms();

    msa::SaslClientConnection client("melon");
    const std::string wanted_mechanism = "SCRAM-SHA-256";
    if (supported_mechanisms.find(wanted_mechanism) == std::string_view::npos)
        throw std::runtime_error("Wanted machanism " + wanted_mechanism + " is not supported by server. Supported mechanisms: " + std::string(supported_mechanisms));

    auto [client_response, selected_mechanism] = client.start(wanted_mechanism);
    assert(selected_mechanism == wanted_mechanism);

    auto [server_completness, server_response] = server.start(selected_mechanism, client_response);

    while (server_completness == msa::AuthCompletness::INCOMPLETE)
    {
        std::cout << "Performing extra auth step..." << std::endl;
        msa::StepResult client_step_res = client.step(server_response);
        msa::StepResult server_step_res = server.step(client_step_res.response);
        server_response = server_step_res.response;
        server_completness = server_step_res.completness;
    }

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
