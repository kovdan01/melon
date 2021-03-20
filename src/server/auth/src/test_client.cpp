#define CATCH_CONFIG_MAIN

#include <sasl_server_wrapper.hpp>

#include <boost/asio.hpp>
#include <catch2/catch.hpp>

#include <cassert>
#include <iostream>

using boost::asio::ip::tcp;

constexpr std::size_t BUFFER_LIMIT = 3000;

std::string get_cli_response(const std::string& serv_response, melon::core::auth::SaslClientConnection& conn, int counter)
{
   std::string res;
   if (counter == 0)
   {
       melon::core::auth::SaslClientConnection::StartResult step_res = conn.start(serv_response);
       res = step_res.response;
   }
   else
   {
       melon::core::auth::StepResult step_res = conn.step(serv_response);
       res = step_res.response;
   }

   return res;
}

std::string read_buffered_string(std::size_t n, std::string& in_buf)
{
    std::string x = in_buf.substr(0, n-1);
    in_buf.erase(0, n);
    return x;
}

bool run_auth(const std::string& ip, const std::string& port, const std::string& wanted_mech)
{
    namespace mca = melon::core::auth;
    mca::SaslClientConnection client("melon");

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), ip, port);
    tcp::resolver::iterator iterator = resolver.resolve(query);
    tcp::socket s(io_service);
    boost::asio::connect(s, iterator);

    std::string reply, to_send = wanted_mech;;

    bool confirmation_recieved = false;

    std::string in_buf;
    size_t n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf, BUFFER_LIMIT},'\n');
    reply = read_buffered_string(n, in_buf);
    //std::cout << "Reply is: " << reply << "\n";

    std::cout << "Ready to send \"" << to_send << "\".\n";
    boost::asio::write(s, boost::asio::buffer(to_send + '\n', to_send.size() + 1));

    n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf, BUFFER_LIMIT},'\n');
    reply = read_buffered_string(n, in_buf);
    std::cout << "Reply is: " << reply << " Length is " << reply.size() << "\n";

    auto cli_resp = client.start(wanted_mech);
    mca::StepResult cli_resp2;
    int counter = 0;

    for (;;)
    {
        if (counter == 0)
            to_send = cli_resp.response;
        else
            to_send = cli_resp2.response;
        std::cout << "Ready to send \"" << to_send << "\".\n";
        boost::asio::write(s, boost::asio::buffer(to_send + '\n', to_send.size() +1));
        size_t n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf, BUFFER_LIMIT},'\n');
        reply = read_buffered_string(n, in_buf);
        std::cout << "Reply is: " << reply << " Length is " << reply.size() << "\n";
        if (reply == "Okay, Mr. Client, here's your token...")
        {
            confirmation_recieved = true;
            break;
        }
        cli_resp2 = client.step(reply);
        ++counter;
    }
    in_buf.erase(0,n);
    return confirmation_recieved;
}

TEST_CASE("credential-based tests", "[creds]")
{

try
{
    const std::string ip = "localhost";
    const std::string port = "6666";

    namespace mca = melon::core::auth;
    auto& client_singletone = mca::SaslClientSingleton::get_instance();
    std::string wanted_mech;
    bool confirmation_recieved;
    SECTION("regisered credentials")
    {
        SECTION("SCRAM-SHA-256 mech")
        {
            wanted_mech = "SCRAM-SHA-256";
        }
        SECTION("PLAIN mech")
        {
            wanted_mech = "PLAIN";
        }
        mca::Credentials credentials = { "john", "doe" };
        client_singletone.set_credentials(&credentials);
        REQUIRE_NOTHROW(confirmation_recieved = run_auth(ip, port, wanted_mech));
        REQUIRE(confirmation_recieved == true);

    }
    SECTION("not registered credentials")
    {
        SECTION("SCRAM-SHA-256 mech2")
        {
            wanted_mech = "SCRAM-SHA-256";
        }
        SECTION("PLAIN mech2")
        {
            wanted_mech = "PLAIN";
        }
        mca::Credentials credentials = { "Igor", "Shcherbakov" };
        client_singletone.set_credentials(&credentials);
        REQUIRE_THROWS(confirmation_recieved = run_auth(ip, port, wanted_mech));
    }
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}

}
