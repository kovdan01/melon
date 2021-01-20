#include <melon/core.hpp>

#include <sasl/saslutil.h>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>

#include <array>
#include <cstring>
#include <iostream>

constexpr unsigned long max_pass_len = 100;

using saslret = int;

namespace callbacks
{
    struct my_secret_t  // it's the same as sasl_secret_t, butwith set size
    {
        unsigned long len;
        unsigned char data[max_pass_len];
    };

    struct my_context_t
    {
        const std::string username;
        const std::string password;
        my_secret_t secret;
    };

    static saslret login_name(void* context, int id, const char **result, unsigned *len)
    {
        auto* params = static_cast<my_context_t*>(context);
        switch (id)
        {
        case SASL_CB_USER:
        case SASL_CB_AUTHNAME:
            std::cout << "Setting username to " << params->username <<"\n";
            *result = params->username.c_str();
            *len = static_cast<unsigned>(params->username.size());
            break;
        default:
            return SASL_BADPARAM;
        }
        return SASL_OK;
    }

    static saslret login_pass(sasl_conn_t*, void* context, int id, sasl_secret_t **psecret)
    {
        auto* params = static_cast<my_context_t*>(context);
        if(id != SASL_CB_PASS)
            return SASL_BADPARAM;

        params->secret.len = params->password.size();
        memcpy(params->secret.data, params->password.c_str(),params->secret.len);
        *psecret = reinterpret_cast<sasl_secret_t*>(&params->secret);
        return SASL_OK;
    }



} // namespace callbacks

int main()
{
    melon::core::hello();

    // cyrus-SASL client
    callbacks::my_context_t context = {"igor","igors_pass",{}};
    static std::array<sasl_callback_t, 4> callbacks = {{
        {SASL_CB_USER, reinterpret_cast<sasl_callback_ft>(&callbacks::login_name), &context},
        {SASL_CB_AUTHNAME, reinterpret_cast<sasl_callback_ft>(&callbacks::login_name), &context},
        {SASL_CB_PASS, reinterpret_cast<sasl_callback_ft>(&callbacks::login_pass), &context},
        {SASL_CB_LIST_END, nullptr, nullptr}
      }};

    saslret retcode = sasl_client_init(callbacks.data());
    if(retcode != SASL_OK)
    {
      throw std::runtime_error("sasl inititalization: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }
    /*
     * code to recieve server mech capabilites here
     */
    std::string recieved_mech_list = {"mech"};

    return 0;
}
