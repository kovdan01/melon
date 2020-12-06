#include <sasl/saslutil.h>

#include <iostream>
#include <cstddef>
#include <cstring>
#include <unistd.h>




class Sasl_conn
{

public:

    ~Sasl_conn();
    sasl_conn_t* get_conn();
    sasl_conn_t** get_pconn();

private:
    sasl_conn_t* conn;
    sasl_conn_t** pconn=&conn;

};

Sasl_conn::~Sasl_conn(){sasl_dispose(&conn);}

sasl_conn_t* Sasl_conn::get_conn(){return conn;}

sasl_conn_t** Sasl_conn::get_pconn(){return pconn;}






class SaslServer
{

public:
    //SaslServer(){};
    ~SaslServer();
    std::string get_service();
    std::string get_chosenmech();
    sasl_conn_t* get_conn();
    sasl_conn_t** get_pconn();
    unsigned* get_len();
    unsigned* get_count();
    //void set_data(const char* string);
    void set_service(std::string);
    int sasl_server_start_wrapper();
    int sasl_server_new_wrapper();

private:
    unsigned *len, *count;
    //std::string_view data;
    std::string chosenmech;
    std::string service;
    Sasl_conn conn;

};

int SaslServer::sasl_server_start_wrapper()
{
    const char* clientin;
    unsigned clientinlen = 0;
    return sasl_server_start(this->get_conn(), this->get_chosenmech().c_str(), clientin, clientinlen, &clientin, &clientinlen);
}

int SaslServer::sasl_server_new_wrapper()
{
    return sasl_server_new(this->get_service().c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, this->get_pconn());
}

SaslServer::~SaslServer(){sasl_server_done();}

std::string SaslServer::get_service(){return service;}

std::string SaslServer::get_chosenmech(){return chosenmech;}

sasl_conn_t* SaslServer::get_conn(){return conn.get_conn();}

sasl_conn_t** SaslServer::get_pconn(){return conn.get_pconn();}

unsigned* SaslServer::get_len(){return len;}

unsigned* SaslServer::get_count(){return count;}


//void SaslServer::set_data(const char *string)
//{
//    data = string;
//}

void SaslServer::set_service(std::string string)
{
    service = string;
}


//int sasl_server_new_wrapper(SaslServer &server)
//{
//    return sasl_server_new(server.get_service().c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, 0, server.get_pconn());
//}


int sasl_listmech_wrapper(SaslServer server)
{
    const char* data;
    int ret = sasl_listmech(server.get_conn(), nullptr, nullptr, " ", nullptr, &data, (unsigned int *) server.get_len(), (int *) server.get_count());
    //server.set_data(data);
    return ret;
}


//int sasl_server_start_wrapper(SaslServer server)
//{
//    const char* clientin;
//    unsigned clientinlen = 0;
//    return sasl_server_start(server.get_conn(), server.get_chosenmech().c_str(), clientin, clientinlen, &clientin, &clientinlen);
//}



int sasl_getprop_username_wrapper(SaslServer server)
{
    const void** userid;
    int ret =  sasl_getprop(server.get_conn(), SASL_USERNAME, (const void **) &userid);
    std::cout << "Username: " << userid;
    return ret;
}





class SaslClient
{
public:
    SaslClient(){};
    ~SaslClient();
    std::string get_service();
    std::string get_chosenmech();
    sasl_conn_t* get_conn();
    sasl_conn_t** get_pconn();
    unsigned* get_len();
    unsigned* get_count();
    void set_data(const char* string);

private:
    unsigned *len, *count;
    //std::string_view data;
    std::string chosenmech;
    std::string service;
    Sasl_conn conn;
};




SaslClient::~SaslClient()
{
     sasl_client_done();
}

std::string SaslClient::get_service()
{
    return service;
}

std::string SaslClient::get_chosenmech()
{
    return chosenmech;
}

sasl_conn_t* SaslClient::get_conn()
{
    return conn.get_conn();
}

sasl_conn_t** SaslClient::get_pconn()
{
    return conn.get_pconn();
}


unsigned* SaslClient::get_len()
{
    return len;
}

unsigned* SaslClient::get_count()
{
    return count;
}


//void SaslClient::set_data(const char *string)
//{
//    data = string;
//}




int sasl_client_new_wrapper(SaslClient client)
{
    return sasl_client_new(client.get_service().c_str(), nullptr, nullptr, nullptr, nullptr, 0, client.get_pconn());
}


int sasl_client_start_wrapper(SaslClient client)
{
    const char * clientout;
    unsigned clientoutlen;
    const char* mech;
    return sasl_client_start(client.get_conn(), client.get_chosenmech().c_str(), nullptr, &clientout, (unsigned int *) &clientoutlen, &mech);
}

int main()
{

    //this is done once
    int result = sasl_server_init(nullptr, "sample");
    if (result != SASL_OK)
    {
        std::cout<<"Fail sasl_server_init()"<<std::endl;
    }


    SaslClient client;
    SaslServer server;
    server.set_service("fake");

    std::cout << "Service: " << server.get_service() <<std::endl;

    result = server.sasl_server_new_wrapper();
    {
        std::cout<<"Fail sasl_server_new()"<<std::endl;
    }

}



















//void snd_str(std::string serverbuf, std::string &clientbuf)
//{
//    clientbuf.assign(serverbuf);
//    //std::cout<<clientbuf<<std::endl;
//}



//int main()
//{




//    mechclient = "PLAIN"; //chosen by client - we should ask to enter this




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
