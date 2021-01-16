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
    const std::string username = "Igor";
    const std::string password = "igor_pass";

    static saslret login_name(void*, int id, const char **result, unsigned *len)
    {
        switch (id)
        {
        case SASL_CB_USER:
        case SASL_CB_AUTHNAME:
            std::cout << "Setting username to " << username <<"\n";
            *result = username.c_str();
            *len = static_cast<unsigned>(username.size());
            break;
        default:
            return SASL_BADPARAM;
        }
        return SASL_OK;
    }

    static saslret login_pass(sasl_conn_t*, void*, int id, sasl_secret_t **psecret)
    {
        if(id != SASL_CB_PASS)
            return SASL_BADPARAM;
        struct my_secret {
            unsigned long len;
            unsigned char data[max_pass_len];		/* variable sized */
        };

        my_secret my_secret_struct;
        my_secret_struct.len = password.size()+1;
        memcpy(my_secret_struct.data, password.c_str(),my_secret_struct.len);
        *psecret = reinterpret_cast<sasl_secret_t*>(&my_secret_struct);
        return SASL_OK;
    }


    static std::array<sasl_callback_t, 4> callbacks = {{
        {SASL_CB_USER, reinterpret_cast<sasl_callback_ft>(&callbacks::login_name), nullptr},
        {SASL_CB_AUTHNAME, reinterpret_cast<sasl_callback_ft>(&callbacks::login_name), nullptr},
        {SASL_CB_PASS, reinterpret_cast<sasl_callback_ft>(&callbacks::login_pass), nullptr},
        {SASL_CB_LIST_END, nullptr, nullptr}
      }};

} // namespace callbacks

int main()
{
    melon::core::hello();

    // cyrus-SASL client
    saslret retcode = sasl_client_init(callbacks::callbacks.data());
    if(retcode != SASL_OK)
    {
      throw std::runtime_error("sasl inititalization: " + std::string(sasl_errstring(retcode, nullptr, nullptr)));
    }
    return 0;
}
