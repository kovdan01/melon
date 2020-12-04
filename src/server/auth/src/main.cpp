#include <sasl/sasl.h>
#include <sasl/saslplug.h>
#include <sasl/saslutil.h>

#include <iostream>
#include <cstddef>
#include <cstring>



static int ask_for_authID(void *context __attribute__((unused)), int id, const char **result, unsigned *len)
{
    static char bufA[1024];

    switch (id)
    {
        case SASL_CB_AUTHNAME:
            printf("please enter an authentication id: ");
            break;
        default:
            return SASL_BADPARAM;
      }

    fgets(bufA, 1024, stdin);
    *result = bufA;
     if (len) *len = strlen(bufA);
          return SASL_OK;
}

static sasl_callback_t callbacks[] = {
  {
    SASL_CB_GETREALM, NULL, NULL
  }, {
    SASL_CB_USER, NULL , NULL
  }, {
    SASL_CB_AUTHNAME, (sasl_callback_ft)&ask_for_authID, NULL
  }, {
    SASL_CB_PASS, NULL, NULL
  }, {
    SASL_CB_LIST_END, NULL, NULL
  }
};


void snd_str(std::string serverbuf, std::string &clientbuf)
{
    clientbuf.assign(serverbuf);
    //std::cout<<clientbuf<<std::endl;
}

int sasl_server_new_wrapper(std::string service, sasl_conn_t** context)
{
    return sasl_server_new(service.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, context);
}

int sasl_client_new_wrapper(std::string service, sasl_conn_t** context)
{
    return sasl_client_new(service.c_str(), nullptr, nullptr, nullptr, nullptr, 0, context);
}

int main()
{

    std::string mechclient;
    std::string mechserver;

    mechclient = "PLAIN"; //chosen by client - we should ask to enter this

    int result = sasl_server_init(nullptr, "sample");
    if (result != SASL_OK)
    {
        std::cout<<"Fail sasl_server_init()"<<std::endl;
    }

    //server context
    sasl_conn_t *connserver;

    result = sasl_server_new_wrapper("fake", &connserver);
    if (result != SASL_OK)
    {
        std::cout<<"Fail sasl_server_new()"<<std::endl;
    }

    //in callbacks defined that future authID will be asked
    result = sasl_client_init(callbacks);
    if (result != SASL_OK)
    {
        std::cout<<"Fail client_init"<<std::endl;
    }

    //client context
    sasl_conn_t *connclient;

    result = sasl_client_new_wrapper("fake", &connclient);
    if (result != SASL_OK)
    {
        std::cout<<"Fail client_new"<<std::endl;
    }

    //currently chosen mech by client - no mech yet
    std::string clientbuf = "NO MECH NOW";

    //negotiation server to client: server prints which mechanisms are available
    printf("generating available  sasl mechanisms... \n");
    const char* data;
    int len;
    int count;

    result  = sasl_listmech(connserver, nullptr, nullptr, " ", nullptr, &data, (unsigned int *) &len, &count);
    if (result  != SASL_OK)
            std::cout<<"Fail generating mechanism list"<<std::endl;
    std::cout<<"Number of mechanisms: " << count <<std::endl;
    std::cout<<"Mechanisms: "<<data<<std::endl;
    snd_str(data, mechserver);

    //server send mechs to client
    std::cout<<"Send possible mechanisms to client"<<std::endl;
    snd_str(data, clientbuf);

    //client recieve mechs from server
    std::cout<<clientbuf<<std::endl;


    //client checks that mech, that it wants, is in the list
    if (clientbuf.find(mechclient) != std::string::npos)
     {
           std::cout<<mechclient<<" is found, so can be used "<<std::endl;
     }

     //client do: sasl_client_start
      const char *dataclient;
      const char *chosenmechclient;
      int lenclient;

      //client should start authatification depending on its callback, but smth fails and there are problems with memory
      result = sasl_client_start(connclient, mechclient.c_str(), nullptr, &dataclient, (unsigned int *) &lenclient, &chosenmechclient);
      if(result == SASL_OK)
      {
            std::cout<<"SASL_OK"<<std::endl;
      }

        //result=sasl_client_step(connclient, in,   inlen, &client_interact,  &out, &outlen); /* filled in on success */


        //result  = sasl_server_start(connserver, mechclient.c_str(), buf, len, &data, (unsigned int *) &len);

//        result = sasl_server_step(connserver, buf, len, &data, (unsigned int *) &len);

//        result = sasl_getprop(connserver, SASL_USERNAME, (const void **) &userid);

        if(result == SASL_CONTINUE)
        {
             std::cout<<"wainting for some steps"<<std::endl;
        }

        if (result != SASL_OK && result != SASL_CONTINUE) {
                std::cout<<"error"<<std::endl;
            }

        std::cout<<"done"<<std::endl;


        sasl_dispose(&connserver);
        sasl_dispose(&connclient);
        sasl_client_done();


}
