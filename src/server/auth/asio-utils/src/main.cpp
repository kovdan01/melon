#include <ce/asio-main.hpp>

namespace ce
{
    int main(std::span<const char* const>);
}

int main(int argc,char* argv[])
{
    ce::asio_main(argc,argv,ce::main);
}
