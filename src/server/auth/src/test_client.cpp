#include <sasl_server_wrapper.hpp>

#include <boost/asio.hpp>

#include <cassert>
#include <iostream>

using boost::asio::ip::tcp;

constexpr std::size_t limit = 3000;

std::string get_cli_response(const std::string serv_response, melon::core::auth::SaslClientConnection& conn, int counter)
{
   std::string res;
   if(counter == 0)
   {
       auto step_res = conn.start(serv_response);
       res = step_res.response;
   }
   else {
       auto step_res = conn.step(serv_response);
       res = step_res.response;
   }

   return res;
}

int main(int argc, char* argv[]) try
{

    namespace mca = melon::core::auth;
    auto& client_singletone = mca::SaslClientSingleton::get_instance();
    mca::Credentials credentials = { "john", "doe" };
    client_singletone.set_credentials(&credentials);
    mca::SaslClientConnection client("melon");
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }


    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    tcp::socket s(io_service);
    boost::asio::connect(s, iterator);


     char confirm;
    std::cout<<"Ready to recieve reply. Proceed? [y]"; std::cin >> confirm;
    std::string in_buf;
    size_t n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf,limit},'\n');
    std::cout << "Reply is: " << in_buf.substr(0, n) << "\n";
    in_buf.erase(0,n);

    std::string wanted_mech = "PLAIN";
    std::cout << "Enter message: ";
    std::string str = wanted_mech;
    std::cout<<"Ready to send \"" << str << "\". Proceed? [y]"; std::cin >> confirm;
    boost::asio::write(s, boost::asio::buffer(str + '\n', str.size() + 1));

    std::cout<<"Ready to recieve reply. Proceed? [y]"; std::cin >> confirm;
    n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf,limit},'\n');
    std::cout << "Reply is: " << in_buf.substr(0, n) << " Length is " << n << "\n";
    in_buf.erase(0,n);
    auto cli_resp = client.start(wanted_mech);
    mca::StepResult cli_resp2;
    int counter = 0;

    for(;;)
    {
        if(counter == 0)
            str = cli_resp.response;
        else
            str = cli_resp2.response;
        std::cout<<"Ready to send \"" << str << "\". Proceed? [y]"; std::cin >> confirm;
        boost::asio::write(s, boost::asio::buffer(str + '\n', str.size() +1));
        std::cout<<"Ready to recieve reply. Proceed? [y]"; std::cin >> confirm;
        size_t n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf,limit},'\n');
        if(n == 1)
        {
            in_buf.erase(0,n);
            break;
        }
        in_buf = in_buf.substr(0, n);
        std::cout << "Reply is: " << in_buf << " Length is " << in_buf.size() << "\n";
        cli_resp2 = client.step(in_buf);
        in_buf.erase(0,n);
        ++counter;
    }
    n = boost::asio::read_until(s, boost::asio::dynamic_string_buffer{in_buf,limit},'\n');
    std::cout << "Reply is: " << in_buf.substr(0, n) << " Length is " << n << "\n";
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
