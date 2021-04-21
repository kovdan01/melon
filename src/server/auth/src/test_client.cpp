#include <melon/core/serialization.hpp>
#include <sasl_server_wrapper.hpp>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <catch2/catch.hpp>

#include <cassert>


using boost::asio::ip::tcp;

namespace mca = melon::core::auth;

constexpr std::size_t BUFFER_LIMIT = 3000;

std::string get_client_response(const std::string& server_response, mca::SaslClientConnection& conn, int counter)
{
    std::string_view response;
    if (counter == 0)
        response = conn.start(server_response).response;
    else
        response = conn.step(server_response).response;
    if (response.data() != nullptr)  // -V547
        return std::string(response);
    return "";
}

// The function copies string read before the delimiter and erases that part of buffer
std::string read_erase_buffered_string(std::size_t n, std::string& in_buf)
{
    std::string before_separator = std::move(in_buf);
    in_buf = std::string(before_separator.c_str() + n + 1, before_separator.size() - n);
    before_separator.erase(n - 1,  before_separator.size() - 1);
    return before_separator;
}

template<typename Stream, typename What>
void send_serialized(Stream& stream, What& what)
{
    auto [send_size, serialized_data] = melon::core::serialization::serialize(what);
    boost::asio::write(stream, boost::asio::buffer(&send_size, sizeof(send_size)));
    boost::asio::write(stream, boost::asio::buffer(serialized_data));
}

template<typename Stream>
std::string recieve_serialized(Stream& stream, std::string& buffer)
{
    std::uint32_t recieve_size;
    boost::asio::read(stream, boost::asio::buffer(&recieve_size, sizeof(recieve_size)), boost::asio::transfer_exactly(sizeof(recieve_size)), 0);
    std::size_t n = boost::asio::read(stream, boost::asio::dynamic_string_buffer{buffer, BUFFER_LIMIT}, boost::asio::transfer_exactly(recieve_size));
    auto reply = melon::core::serialization::deserialize<std::string>(buffer);
    buffer.erase(0, n);
    return reply;
}

bool run_auth(const std::string& ip, const std::string& port, const std::string& wanted_mech)
{
    // Test client is supposed to run on the same computer as server
    // so client's and server's hostnames are the same
    mca::SaslClientConnection client("melon", boost::asio::ip::host_name());

    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::socket s(io_context);
    boost::asio::connect(s, resolver.resolve(ip, port));

    std::string reply, to_send = wanted_mech;

    bool confirmation_recieved = false;

    std::size_t n;
    std::string in_buf;

    recieve_serialized(s, in_buf);

    send_serialized(s, to_send);

    n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf, BUFFER_LIMIT}, '\n');
    reply = read_erase_buffered_string(n, in_buf);

    for (int counter = 0; ; ++counter)
    {
        to_send = get_client_response(reply, client, counter);
        boost::asio::write(s, boost::asio::buffer(to_send + '\n', to_send.size() + 1));
        std::size_t n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf, BUFFER_LIMIT}, '\n');
        reply = read_erase_buffered_string(n, in_buf);
        if (reply == mca::AuthResultSingleton::get_instance().success())
        {
            confirmation_recieved = true;
            break;
        }
        if (reply == mca::AuthResultSingleton::get_instance().failure())
        {
            confirmation_recieved = false;
            break;
        }
    }
    in_buf.erase(0, n);
    return confirmation_recieved;
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
        SECTION("SCRAM-SHA-256 mech")
        {
            wanted_mech = "SCRAM-SHA-256";
        }
        SECTION("PLAIN mech")
        {
            wanted_mech = "PLAIN";
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
        SECTION("PLAIN mech")
        {
            wanted_mech = "PLAIN";
        }
        mca::Credentials credentials = { "john", "password" };
        client_singletone.set_credentials(&credentials);
        REQUIRE_NOTHROW(confirmation_recieved = run_auth(ip, port, wanted_mech));
        REQUIRE(confirmation_recieved == false);
    }
}
