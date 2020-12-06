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
    static SaslServerSingleton* m_instance;
};

int main()
{
    static const SaslServerSingleton singletone;

    msa::SaslServer server("fake");
    std::cout << "Mechs: " << server.list_mechanisms() << '\n';
    std::cout << "Start: " << server.start("PLAIN") << '\n';
    std::cout << "Username: " << server.get_username() << '\n';
}
