#include <sasl/sasl.h>
#include <sasl/saslplug.h>
#include <sasl/saslutil.h>

#include <iostream>
#include <cstddef>
#include <cstring>
#include <unistd.h>



class Sasl_conn
{
public:
    Sasl_conn();
    ~Sasl_conn();
    sasl_conn_t* get_conn();
    sasl_conn_t** get_pconn();

private:
    sasl_conn_t* conn;
    sasl_conn_t** pconn=&conn;
};

Sasl_conn::~Sasl_conn()
{
    sasl_dispose(&conn);
}

sasl_conn_t* Sasl_conn::get_conn()
{
    return conn;
}

sasl_conn_t** Sasl_conn::get_pconn()
{
    return pconn;
}


int sasl_server_new_wrapper(std::string service, Sasl_conn context)
{
    return sasl_server_new(service.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, context.get_pconn());
}

//int sasl_client_new_wrapper(std::string service, sasl_conn_t context)
//{
//    return sasl_client_new(service.c_str(), nullptr, nullptr, nullptr, nullptr, 0, context.get_pconn());
//}





//char buffordata[1000];

//static char *
//getpassphrase(const char *prompt)
//{
//  return getpass(prompt);
//}



//static int simple(void *context, int id, const char **result, unsigned *len)
//{
//  const char *value = (const char *)context;

//  if (! result)
//    return SASL_BADPARAM;

//  switch (id) {
//  case SASL_CB_USER:
//    *result = value;
//    if (len)
//      *len = value ? (unsigned) strlen(value) : 0;
//    break;
//  case SASL_CB_AUTHNAME:
//    *result = value;
//    if (len)
//      *len = value ? (unsigned) strlen(value) : 0;
//    break;
//  default:
//    return SASL_BADPARAM;
//  }

//  printf("returning OK: %s\n", *result);

//  return SASL_OK;
//}


//static int
//getsecret(sasl_conn_t *conn, void *context __attribute__((unused)), int id, sasl_secret_t **psecret)
//{
//  char *password;
//  unsigned len;

//  if (! conn || ! psecret || id != SASL_CB_PASS)
//    return SASL_BADPARAM;

//  password = getpassphrase("Password: ");
//  if (! password)
//    return SASL_FAIL;

//  len = (unsigned) strlen(password);

//  *psecret = (sasl_secret_t *) malloc(sizeof(sasl_secret_t) + len);

//  if (! *psecret) {
//    memset(password, 0, len);
//    return SASL_NOMEM;
//  }

//  (*psecret)->len = len;
//  strcpy((char *)(*psecret)->data, password);
//  memset(password, 0, len);

//  return SASL_OK;
//}


//static int
//prompt(void *context __attribute__((unused)),
//       int id,
//       const char *challenge,
//       const char *prompt,
//       const char *defresult,
//       const char **result,
//       unsigned *len)
//{
//  if ((id != SASL_CB_ECHOPROMPT && id != SASL_CB_NOECHOPROMPT)
//      || !prompt || !result || !len)
//    return SASL_BADPARAM;

//  if (! defresult)
//    defresult = "";

//  fputs(prompt, stdout);
//  if (challenge)
//    printf(" [challenge: %s]", challenge);
//  printf(" [%s]: ", defresult);
//  fflush(stdout);

//  if (id == SASL_CB_ECHOPROMPT) {
//    char *original = getpassphrase("");
//    if (! original)
//      return SASL_FAIL;
//    if (*original)
//      *result = strdup(original);
//    else
//      *result = strdup(defresult);
//    memset(original, 0L, strlen(original));
//  } else {
//    char buf[1024];
//    fgets(buf, 1024, stdin);
//    if (buf[0]) {
//      *result = strdup(buf);
//    } else {
//      *result = strdup(defresult);
//    }
//    memset(buf, 0L, sizeof(buf));
//  }
//  if (! *result)
//    return SASL_NOMEM;

//  *len = (unsigned) strlen(*result);

//  return SASL_OK;
//}




//void snd_str(std::string serverbuf, std::string &clientbuf)
//{
//    clientbuf.assign(serverbuf);
//    //std::cout<<clientbuf<<std::endl;
//}



//int main()
//{


//    static sasl_callback_t callbacks[8], *callback;
//    char *userid = NULL, *authid = NULL;

//    userid = (char*)"user";
//    authid = (char*)"user";


//    callback = callbacks;

//    /* user */
//    if (userid) {
//      callback->id = SASL_CB_USER;
//      callback->proc = (sasl_callback_ft)&simple;
//      callback->context = userid;
//      ++callback;
//    }

//    /* authname */
//    if (authid) {
//      callback->id = SASL_CB_AUTHNAME;
//      callback->proc = (sasl_callback_ft)&simple;
//      callback->context = authid;
//      ++callback;
//    }

//    /* password */
//    callback->id = SASL_CB_PASS;
//    callback->proc = (sasl_callback_ft)&getsecret;
//    callback->context = NULL;
//    ++callback;

//    /* echoprompt */
//     callback->id = SASL_CB_ECHOPROMPT;
//     callback->proc = (sasl_callback_ft)&prompt;
//     callback->context = NULL;
//     ++callback;


//     /* noechoprompt */
//      callback->id = SASL_CB_NOECHOPROMPT;
//      callback->proc = (sasl_callback_ft)&prompt;
//      callback->context = NULL;
//      ++callback;

//      /* termination */
//       callback->id = SASL_CB_LIST_END;
//       callback->proc = NULL;
//       callback->context = NULL;
//       ++callback;

//    std::string mechclient;
//    std::string mechserver;

//    mechclient = "PLAIN"; //chosen by client - we should ask to enter this

//    //this is done once
//    int result = sasl_server_init(nullptr, "sample");
//    if (result != SASL_OK)
//    {
//        std::cout<<"Fail sasl_server_init()"<<std::endl;
//    }

//    //server context
//    sasl_conn_t *connserver;

//    result = sasl_server_new_wrapper("fake", &connserver);
//    if (result != SASL_OK)
//    {
//        std::cout<<"Fail sasl_server_new()"<<std::endl;
//    }

//    //in callbacks defined that future authID will be asked
//    result = sasl_client_init(callbacks);
//    if (result != SASL_OK)
//    {
//        std::cout<<"Fail client_init"<<std::endl;
//    }

//    //client context
//    sasl_conn_t *connclient;

//    result = sasl_client_new_wrapper("fake", &connclient);
//    if (result != SASL_OK)
//    {
//        std::cout<<"Fail client_new"<<std::endl;
//    }

//    //currently chosen mech by client - no mech yet
//    std::string clientbuf = "NO MECH NOW";

//    //negotiation server to client: server prints which mechanisms are available
//    printf("generating available  sasl mechanisms... \n");
//    const char* data;
//    int len;
//    int count;

//    result  = sasl_listmech(connserver, nullptr, nullptr, " ", nullptr, &data, (unsigned int *) &len, &count);
//    if (result  != SASL_OK)
//            std::cout<<"Fail generating mechanism list"<<std::endl;
//    std::cout<<"Number of mechanisms: " << count <<std::endl;
//    std::cout<<"Generating mechanisms list...\nMechanisms: "<<data<<std::endl;
//    snd_str(data, mechserver);

//    //server send mechs to client
//    std::cout<<"Send possible mechanisms to client"<<std::endl;
//    snd_str(data, clientbuf);

//    //client recieve mechs from server
//    std::cout<<clientbuf<<std::endl;


//    //server: Waiting for client mechanism... recieve mech from client and do: sasl_server_start


//    //client checks that mech, that it wants, is in the list
//    if (clientbuf.find(mechclient) != std::string::npos)
//     {
//           std::cout<<mechclient<<" is found, so can be used "<<std::endl;
//     }

//     //client do: sasl_client_start
//      const char *dataclient;
//      const char *chosenmechclient;
//      int lenclient;

//      //client should start authatification depending on its callback, but smth fails and there are problems with memory
//      result = sasl_client_start(connclient, mechclient.c_str(), nullptr, &dataclient, (unsigned int *) &lenclient, &chosenmechclient);
//      if (result != SASL_OK && result != SASL_CONTINUE) {
//          std::cout<<"Fail starting SASL negotiation" << std::endl;
//      }

//        //result=sasl_client_step(connclient, in,   inlen, &client_interact,  &out, &outlen); /* filled in on success */


//        //result  = sasl_server_start(connserver, mechclient.c_str(), buf, len, &data, (unsigned int *) &len);

////        result = sasl_server_step(connserver, buf, len, &data, (unsigned int *) &len);

////        result = sasl_getprop(connserver, SASL_USERNAME, (const void **) &userid);

//        printf("Using mechanism %s\n", chosenmechclient);



//        std::string chosenmechserv;
//        snd_str(chosenmechserv, clientbuf);

//        const char *out;
//        unsigned outlen;
//        std::string datamech = "PLAIN";




//        result = sasl_server_start(connserver, chosenmechserv.c_str(), datamech.c_str(), datamech.size(), &out, (unsigned int *)&outlen);
//        if (result != SASL_OK && result != SASL_CONTINUE) {
//            std::cout<<"Fail starting SASL negotiation from server" << std::endl;
//        }

//        if (result == SASL_CONTINUE)
//        {
//            std::cout<<"Continue SASL negotiation"<<std::endl;
//        }

//        result = sasl_getprop(connserver, SASL_USERNAME, (const void **) &userid);
//        printf("Username: %s\n", userid ? userid : "(NULL)");
//        //std::cout<<"done"<<std::endl;


//        sasl_dispose(&connserver);
//        sasl_dispose(&connclient);
//        sasl_client_done();
//        sasl_server_done();


//}
