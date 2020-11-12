#include <melon_core.hpp>
#include <sasl/sasl.h>
#include <sasl/saslplug.h>
#include <sasl/saslutil.h>
#include <iostream>
#include <cstring>

#define N_CALLBACKS (16)
#define SAMPLE_SEC_BUF_SIZE (2048)

static sasl_callback_t callbacks[N_CALLBACKS];

sasl_conn_t *conn = NULL;
const char *data = "plain";
unsigned len = (unsigned) strlen(data);
int count = 1;
char mechanism_client_chose[SAMPLE_SEC_BUF_SIZE];


static unsigned samp_recv()
{
  unsigned len;
  int result;

  if (! fgets(mechanism_client_chose, SAMPLE_SEC_BUF_SIZE, stdin)) {
    std::cout<<"Unable to parse input\n";
  }

  if (strncmp(mechanism_client_chose, "C: ", 3) != 0) {
    std::cout<<"Line must start with 'C: '\n";
  }

  len = strlen(mechanism_client_chose);
  if (len > 0 && mechanism_client_chose[len-1] == '\n') {
      mechanism_client_chose[len-1] = '\0';
  }

  result = sasl_decode64(mechanism_client_chose + 3, (unsigned) strlen(mechanism_client_chose + 3), mechanism_client_chose, SAMPLE_SEC_BUF_SIZE, &len);
  if (result != SASL_OK)
    std::cout<< "Decoding data from base64\n";
  mechanism_client_chose[len] = '\0';
  std::cout <<"Got "<< mechanism_client_chose <<"\n";
  return len;
}



int main()
{
    int result;

    std::cout<<"Create TestServer\n";
    result = sasl_server_init(callbacks, "TestServer");
    if (result!=SASL_OK)
    {
        std::cout << "Fail init\n";
    }

    std::cout<<"Init server\n";

     result = sasl_server_new("kitty", NULL, NULL, NULL, NULL, NULL, 0, &conn);
     if (result != SASL_OK)
     {
         std::cout <<"Fail server_new\n";
     }

     std::cout<<"Listen to connection\n";

     result = sasl_listmech(conn, NULL, "{", ", ", "}", &data, &len, &count);
     if (result != SASL_OK)
     {
         std::cout <<"Fail mech\n";
     }

     std::cout<<"Look what client chose as mech\n";

     std::cout << "Send list of auth mechanism\n";

     const char *out;
     unsigned outlen;

     std::cout<<"Waiting for client mechanism...\n";
     len = samp_recv();

     result = sasl_server_start(conn, mechanism_client_chose, NULL, 0, &out, &outlen);
     if ((result!=SASL_OK) && (result!=SASL_CONTINUE))
         std::cout<<"Fail\n";

     std::cout<<"Success\n";
     sasl_dispose(&conn);

}
