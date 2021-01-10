#include <sasl_wrapper.hpp>

#include <iostream>
#include <string_view>

namespace msa = melon::server::auth;

class SaslServerSingleton
{
public:
    SaslServerSingleton()
    {
        int res = sasl_server_init(nullptr, "sample");
        if (res != SASL_OK)
            throw std::runtime_error("Sasl server init exit code " + std::to_string(res));
    }

    ~SaslServerSingleton()
    {
        sasl_server_done();
    }

private:
    static SaslServerSingleton* g_instance;
};

int main()
{
    static const SaslServerSingleton singletone;

    msa::SaslServer server("fake");
    std::string_view servermech = server.list_mechanisms();
    std::cout << servermech << std::endl;
    msa::SaslClient client("fake", "username", "username", "password");


//    //client recieve server mechs and choose plain and send to server

//    //server recieve client mechs and checks that client chose available mech and send confirmation (?)

//    //client start

    //output clientout

    //server start

    //output serverout





/*
    std::cout << "Mechs: " << server.list_mechanisms() << '\n';   
    std::cout << "Start: " << server.start("PLAIN") << '\n';
    std::cout << "Username: " << server.get_username() << '\n';
*/
    return 0;
}
