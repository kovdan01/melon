#include <melon/core/sasl_client_wrapper.hpp>
#include <melon/core/serialization.hpp>
#include <melon/core/session.hpp>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <catch2/catch.hpp>

#include <cassert>

using boost::asio::ip::tcp;

namespace ba = boost::asio;
namespace mc = melon::core;
namespace mca = melon::core::auth;
namespace mcs = melon::core::serialization;

static bool run_auth(const std::string& ip, const std::string& port, const std::string& wanted_mech)
{
    // Test client is supposed to run on the same computer as server
    // so client's and server's hostnames are the same
    try
    {
        mca::SaslClientConnection client("melon", ba::ip::host_name());
        mc::SyncSession session(ip, port);

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
        return false;
    }
}

TEST_CASE("credential-based tests", "[creds]")
{
    const std::string ip = "localhost";
    const std::string port = "6666";

    auto& client_singletone = mca::SaslClientSingleton::get_instance();
    std::string wanted_mech;
    bool confirmation_recieved;
    SECTION("regisered credentials")
    {
        SECTION("SCRAM-SHA-256 mech")
        {
            wanted_mech = "SCRAM-SHA-256";
        }
        SECTION("DIGEST-MD5 mech")
        {
            wanted_mech = "DIGEST-MD5";
        }
        mca::Credentials credentials = { "john", "doe" };
        client_singletone.set_credentials(&credentials);
        REQUIRE_NOTHROW(confirmation_recieved = run_auth(ip, port, wanted_mech));
        REQUIRE(confirmation_recieved == true);
    }
    SECTION("not registered credentials")
    {
        SECTION("SCRAM-SHA-256 mech")
        {
            wanted_mech = "SCRAM-SHA-256";
        }
        SECTION("DIGEST-MD5 mech")
        {
            wanted_mech = "DIGEST-MD5";
        }
        mca::Credentials credentials = { "Igor", "Shcherbakov" };
        client_singletone.set_credentials(&credentials);
        REQUIRE_NOTHROW(confirmation_recieved = run_auth(ip, port, wanted_mech));
        REQUIRE(confirmation_recieved == false);
    }
    SECTION("incorrect password")
    {
        SECTION("SCRAM-SHA-256 mech")
        {
            wanted_mech = "SCRAM-SHA-256";
        }
        SECTION("DIGEST-MD5 mech")
        {
            wanted_mech = "DIGEST-MD5";
        }
        mca::Credentials credentials = { "john", "password" };
        client_singletone.set_credentials(&credentials);
        REQUIRE_NOTHROW(confirmation_recieved = run_auth(ip, port, wanted_mech));
        REQUIRE(confirmation_recieved == false);
    }
}
