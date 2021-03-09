#include <sasl_server_wrapper.hpp>

#include <boost/asio.hpp>

#include <cassert>
#include <iostream>

using boost::asio::ip::tcp;

constexpr std::size_t BUFFER_LIMIT = 3000;

std::string get_cli_response(const std::string& serv_response, melon::core::auth::SaslClientConnection& conn, int counter)
{
   std::string res;
   if(counter == 0)
   {
       melon::core::auth::SaslClientConnection::StartResult step_res = conn.start(serv_response);
       res = step_res.response;
   }
   else {
       melon::core::auth::StepResult step_res = conn.step(serv_response);
       res = step_res.response;
   }

   return res;
}

std::string read_buffered_string(std::size_t n, std::string& in_buf)
{
    std::string x = in_buf.substr(0, n-1);
    in_buf.erase(0,n);
    return x;
}

int main(int argc, char* argv[]) try
{

    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }

    namespace mca = melon::core::auth;
    auto& client_singletone = mca::SaslClientSingleton::get_instance();
    mca::Credentials credentials = { "john", "doe" };
    client_singletone.set_credentials(&credentials);
    mca::SaslClientConnection client("melon");
    std::string wanted_mech = "SCRAM-SHA-256";

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);
    tcp::socket s(io_service);
    boost::asio::connect(s, iterator);

    char confirm;
    std::string reply, to_send = wanted_mech;;

    std::cout<<"Ready to recieve reply. Proceed? [y]"; std::cin >> confirm;
    std::string in_buf;
    size_t n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf, BUFFER_LIMIT},'\n');
    reply = read_buffered_string(n, in_buf);
    std::cout << "Reply is: " << reply << "\n";

    std::cout<<"Ready to send \"" << to_send << "\". Proceed? [y]"; std::cin >> confirm;
    boost::asio::write(s, boost::asio::buffer(to_send + '\n', to_send.size() + 1));

    std::cout<<"Ready to recieve reply. Proceed? [y]"; std::cin >> confirm;
    n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf, BUFFER_LIMIT},'\n');
    reply = read_buffered_string(n, in_buf);
    std::cout << "Reply is: " << reply << " Length is " << reply.size() << "\n";
    // confirmation of protocol

    auto cli_resp = client.start(wanted_mech);
    mca::StepResult cli_resp2;
    int counter = 0;

    for (;;)
    {
        if (counter == 0)
            to_send = cli_resp.response;
        else
            to_send = cli_resp2.response;
        std::cout<<"Ready to send \"" << to_send << "\". Proceed? [y]"; std::cin >> confirm;
        boost::asio::write(s, boost::asio::buffer(to_send + '\n', to_send.size() +1));
        std::cout<<"Ready to recieve reply. Proceed? [y]"; std::cin >> confirm;
        size_t n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf, BUFFER_LIMIT},'\n');
        reply = read_buffered_string(n, in_buf);
        std::cout << "Reply is: " << reply << " Length is " << reply.size() << "\n";
        if(reply == "Okay, Mr. Client, here's your token...")
        {
            in_buf.erase(0,n);
            break;
        }
        cli_resp2 = client.step(reply);
        ++counter;
    }
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
