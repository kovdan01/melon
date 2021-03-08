#include <sasl_server_wrapper.hpp>

#include <cassert>
#include <iostream>

std::string get_cli_response(const std::string serv_response, melon::core::auth::SaslClientConnection& conn, int counter)
{
   std::string res;
   if(counter == 1)
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

int main() try
{
    namespace mca = melon::core::auth;

    auto& client_singletone = mca::SaslClientSingleton::get_instance();
    mca::Credentials credentials = { "john", "doe" };
    client_singletone.set_credentials(&credentials);

    mca::SaslClientConnection client("melon");
    std::string serv_response;
    int counter = 0;
    while (true)
    {
        std::cout << "Performing extra auth step..." << std::endl;
        std::cin >> serv_response;
        std::cout << get_cli_response(serv_response, client, counter) << std::endl;
        ++counter;
    }
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
