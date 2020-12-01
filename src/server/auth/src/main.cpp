#include <melon_core.hpp>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>
#include <sasl/saslutil.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

#define N_CALLBACKS (16)
#define SAMPLE_SEC_BUF_SIZE (2048)

static sasl_callback_t callbacks[N_CALLBACKS];

sasl_conn_t *conn = NULL;
const char *mech = "PLAIN";
unsigned len = (unsigned) strlen(mech);
int count = 1;
char mechanism_client_chose[SAMPLE_SEC_BUF_SIZE];


int main()
{
    int result;

    std::cout<<"Use default configure file sample.conf "<<std::endl;
    result = sasl_server_init(callbacks, "sample");
    if (result!=SASL_OK)
    {
        std::cout << "Fail server_init"<<std::endl;
    }

    //someservice?
     std::cout<<"Init server"<<std::endl;
     result = sasl_server_new("someservice", /*"localhost.localdomain."*/ NULL, NULL, NULL, NULL, NULL, 0, &conn);
     if (result != SASL_OK)
     {
         std::cout <<"Fail server_new"<<std::endl;
     }

     std::cout<<"Generating client mech list"<<std::endl;
     result = sasl_listmech(conn, NULL, "{", ", ", "}", &mech, &len, &count);
     if (result != SASL_OK)
     {
         std::cout <<"Fail mech"<<std::endl;
     }
     std::cout<<"Look what client chose as mech"<<std::endl;

    //why you are not waiting... please sleep?


    const char *out;
    unsigned outlen;

    result = sasl_server_start(conn,  mechanism_client_chose, NULL,  0, &out, &outlen);

    if ((result!=SASL_OK) && (result!=SASL_CONTINUE))
    {
        std::cout<<"fail to give client's mech"<<std::endl;
    }
    else if (result==SASL_OK)
    {
        std::cout<<"success"<<std::endl;
    }

     sasl_dispose(&conn);

}
