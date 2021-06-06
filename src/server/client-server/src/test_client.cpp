#include <melon/core/commands.hpp>
#include <melon/core/sasl_client_wrapper.hpp>
#include <melon/core/serialization.hpp>
#include <melon/core/session.hpp>
#include <melon/core/token.hpp>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <iostream>

namespace mc = melon::core;
namespace mca = melon::core::auth;

static bool run_auth(mc::SyncSession& session, const std::string& wanted_mech)
{
    // Test client is supposed to run on the same computer as server
    // so client's and server's hostnames are the same
    try
    {
        mca::SaslClientConnection client("melon", boost::asio::ip::host_name());
        mc::StringViewOverBinary supported_mechanisms(session.receive());
        session.send(wanted_mech);
        mc::StringViewOverBinary mechanism_confirmation(session.receive());
        // TODO: handle mechanism mismatch
        assert(mechanism_confirmation.view() == wanted_mech);
        auto [start_response, selected_mechanism, completness] = client.start(mechanism_confirmation.view());
        if (completness == mca::AuthState::COMPLETE)
            return true;
        assert(selected_mechanism == wanted_mech);
        session.send(start_response);

        while (completness == mca::AuthState::INCOMPLETE)
        {
            mc::buffer_t server_reply = session.receive();
            mca::StepResult step_result = client.step(server_reply);
            mc::buffer_view_t step_response = step_result.response;
            completness = step_result.completness;
            switch (completness)
            {
            case mca::AuthState::COMPLETE:
                session.send(step_response);
                return true;
            case mca::AuthState::INCOMPLETE:
                session.send(step_response);
                break;
            default:
                return false;
            }
        }
        return false;
    }
    catch (const mca::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

int main() try
{
    const std::string ip = "localhost";
    const std::string port = "6667";
    mc::SyncSession session(ip, port);

//    {
//        const std::string username = "igor";
//        const std::uint64_t domain_id = 42;
//        const mc::Token token
//        ({
//            0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
//            0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
//            0x20, 0x21, 0x22, 0x23, 0x24, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
//            0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
//        });

//        session.send_code(static_cast<std::uint32_t>(mc::ClientCode::SEND_TOKEN));
//        session.send(username);
//        session.send(domain_id);
//        session.send(token.token());
//    }
//    {
//        const std::string username = "lana";
//        const std::uint64_t domain_id = 42;
//        const mc::Token token
//        ({
//            0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
//            0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
//            0x20, 0x21, 0x22, 0x23, 0x24, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
//            0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
//        });

//        session.send_code(static_cast<std::uint32_t>(mc::ClientCode::SEND_TOKEN));
//        session.send(username);
//        session.send(domain_id);
//        session.send(token.token());
//    }
//    {
//        const std::string username = "igor";
//        const std::uint64_t domain_id = 42;
//        const mc::Token token;

//        session.send_code(static_cast<std::uint32_t>(mc::ClientCode::SEND_TOKEN));
//        session.send(username);
//        session.send(domain_id);
//        session.send(token.token());
//    }

    {
        auto& client_singletone = mca::SaslClientSingleton::get_instance();
        mca::Credentials credentials = { "john", "doe" };
        client_singletone.set_credentials(&credentials);

        const std::string username = "john";
        const std::uint64_t domain_id = 42;

        session.send_code(static_cast<std::uint32_t>(mc::ClientCode::REQUEST_TOKEN));
        session.send(username);
        session.send(domain_id);

        run_auth(session, "SCRAM-SHA-256");
        auto code = static_cast<mc::ServerCode>(session.receive_code());

        mc::Token token{session.receive<mc::Token::token_t>()};

        session.send_code(static_cast<std::uint32_t>(mc::ClientCode::SEND_TOKEN));
        session.send(username);
        session.send(domain_id);
        session.send(token.token());
    }

//    {
//        auto& client_singletone = mca::SaslClientSingleton::get_instance();
//        mca::Credentials credentials = { "john", "invalid" };
//        client_singletone.set_credentials(&credentials);

//        const std::string username = "john";
//        const std::uint64_t domain_id = 42;
//        const mc::Token token;

//        session.send_code(static_cast<std::uint32_t>(mc::ClientCode::REQUEST_TOKEN));
//        session.send(username);
//        session.send(domain_id);

//        run_auth(session, "SCRAM-SHA-256");
//    }

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
catch (...)
{
    std::cerr << "Unknown error\n";
}
