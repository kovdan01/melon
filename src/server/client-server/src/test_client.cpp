#include <melon/core/serialization.hpp>
#include <melon/core/session.hpp>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <iostream>

namespace mc = melon::core;
namespace ba = boost::asio;

int main()
{
    const std::string ip = "localhost";
    const std::string port = "6667";
    mc::SyncSession session(ip, port);
    std::string to_send, to_recieve;
    uint64_t uintosend;
    for(;;)
    {
        std::cin >> to_send;
        session.send<std::string>(to_send);
        std::cin >> uintosend;
        session.send(uintosend);
        std::cin >> uintosend;
        session.send(uintosend);
    }

    return 0;
}
