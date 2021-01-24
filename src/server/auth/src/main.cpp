#include <melon/core.hpp>

#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>

constexpr std::uint64_t MAX_MESSAGE_LEN = 25600;
const std::string wanted_mech = "PLAIN";

using saslret = int;

namespace fakenet  // just what it says on the tin: this is a simulation of network data channel
{
    using byte_t = unsigned char;

    static std::array<byte_t, MAX_MESSAGE_LEN> network;

    template <typename T>
    T read_type(const byte_t* from = network.data())  // yep
    {
        T ans = 0;
        for (std::size_t shift = 0; shift != sizeof(T) * 8; shift += 8, ++from)
            ans |= static_cast<T>(*from) << shift;
        return ans;
    }

    template <typename T>
    void write_type(T what, byte_t* where = network.data())
    {
        T mask = 0xff;
        for (std::size_t shift = 0; shift != sizeof(T) * 8; mask <<= 8, ++where, shift += 8)
            *where = (what & mask) >> shift;
    }

    static void write_bytes(byte_t* from, std::size_t len)
    {
        std::memmove(network.data(), from, len);
    }

    static byte_t* read_bytes(byte_t* where, std::size_t len)
    {
        std::memmove(where, network.data(), len);
        return where;
    }


}  // namespace callbacks

namespace callbacks
{
    struct MyContext
    {
        const std::string username;
        const std::string password;
    };

    static saslret login_name(void* context, int id, const char** result, unsigned* len)
    {
        auto* params = static_cast<MyContext*>(context);
        switch (id)
        {
        case SASL_CB_USER:
        case SASL_CB_AUTHNAME:
            std::cout << "Setting username to " << params->username <<std::endl;
            *result = params->username.c_str();
            if(len != nullptr)
                *len = static_cast<unsigned>(params->username.size());
            break;
        default:
            return SASL_BADPARAM;
        }
        return SASL_OK;
    }

    static saslret login_pass(sasl_conn_t*, void* context, int id, sasl_secret_t** psecret)
    {
        if(id != SASL_CB_PASS)
            return SASL_BADPARAM;
        auto* params = static_cast<MyContext*>(context);
        std::cout << "Setting pass to " << params->password <<std::endl;
        // somehow rewrite this proper C++ way
        static sasl_secret_t* secret;
        auto* temp = reinterpret_cast<sasl_secret_t*>(std::realloc(secret, sizeof(sasl_secret_t) + params->password.size()));
        if (temp != nullptr)
            secret = temp;
        else
            return SASL_NOMEM;
        secret->len = params->password.size();
        std::memcpy(secret->data, params->password.c_str(), secret->len + 1);
        *psecret = secret;
        // somehow rewrite this proper C++ way

        return SASL_OK;
    }



} // namespace fakenet

int main()
{
    melon::core::hello();

    // cyrus-SASL server
    saslret retcode = sasl_server_init(nullptr, "Local server");
    if(retcode != SASL_OK)
    {
      throw std::runtime_error("server inititalization: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }

    // cyrus-SASL client
    callbacks::MyContext context = {"igor","igors_pass"};
    static std::array<sasl_callback_t, 4> callbacks = {{
        {SASL_CB_USER, reinterpret_cast<sasl_callback_ft>(&callbacks::login_name), &context},
        {SASL_CB_AUTHNAME, reinterpret_cast<sasl_callback_ft>(&callbacks::login_name), &context},
        {SASL_CB_PASS, reinterpret_cast<sasl_callback_ft>(&callbacks::login_pass), &context},
        {SASL_CB_LIST_END, nullptr, nullptr}
      }};
    retcode = sasl_client_init(callbacks.data());
    if(retcode != SASL_OK)
    {
      throw std::runtime_error("client inititalization: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }

    /*
     * connection is set up through fakenet
     */

    // cyrus-SASL server
    sasl_conn_t* server_conn;
    retcode = sasl_server_new("rcmd", nullptr, nullptr, nullptr, nullptr, nullptr, 0, &server_conn);
    if(retcode != SASL_OK)
    {
      throw std::runtime_error("server new connection: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }
    const char* server_capabilites_ptr;
    unsigned int server_capabilites_len;
    retcode = sasl_listmech(server_conn, nullptr, nullptr, " ", nullptr, &server_capabilites_ptr, &server_capabilites_len, nullptr);
    if(retcode != SASL_OK)
    {
      throw std::runtime_error("server mech capabilities: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }
    std::string server_servermechs(server_capabilites_ptr, server_capabilites_len);
    fakenet::write_type<std::size_t>(server_servermechs.size());

    // cyrus-SASL client
    sasl_conn_t* client_conn;
    retcode = sasl_client_new("rcmd", nullptr, nullptr, nullptr, nullptr, 0, &client_conn);
    if(retcode != SASL_OK)
    {
      throw std::runtime_error("client new connection: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }
    std::size_t client_servermechs_len = fakenet::read_type<std::size_t>();

    // cyrus-SASL server
    fakenet::write_bytes(reinterpret_cast<unsigned char*>(server_servermechs.data()),server_servermechs.size());

    // cyrus-SASL client
    std::array<char, MAX_MESSAGE_LEN> client_recieved_buffer;
    fakenet::read_bytes(reinterpret_cast<unsigned char*>(client_recieved_buffer.data()),client_servermechs_len);
    client_recieved_buffer[client_servermechs_len]='\0';

    /*
     * client checks if he can use the wanted mech, sends it to server, server confirms that he's ready
     */

    // cyrus-SASL client
    const char* client_saslout;
    const char* client_saslselectedmech;
    unsigned int client_saslout_len;
    retcode = sasl_client_start(client_conn, /*client_recieved_buffer.data()*/wanted_mech.c_str(), nullptr, &client_saslout, &client_saslout_len, &client_saslselectedmech);
    if(retcode != SASL_OK && retcode != SASL_CONTINUE)
    {
      throw std::runtime_error("client negotioation start: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }
    fakenet::write_bytes(reinterpret_cast<unsigned char*>(const_cast<char*>(client_saslout)), client_saslout_len);
    // cyrus-SASL server
    std::array<char, MAX_MESSAGE_LEN> server_recieved_buffer;
    fakenet::read_bytes(reinterpret_cast<unsigned char*>(server_recieved_buffer.data()), client_saslout_len); // !! client_servermechs_len
    server_recieved_buffer[client_servermechs_len]='\0'; // !!
    const char* server_saslout;
    unsigned int server_saslout_len;
    retcode = sasl_server_start(server_conn, wanted_mech.c_str(), server_recieved_buffer.data(), client_saslout_len, &server_saslout, &server_saslout_len);
    if(retcode != SASL_OK && retcode != SASL_CONTINUE)
    {
      throw std::runtime_error("server negotioation start: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }

    // cyrus-SASL client
    sasl_dispose(&client_conn);
    sasl_client_done();

    // cyrus-SASL server
    sasl_dispose(&server_conn);
    sasl_server_done();

    return 0;
}
