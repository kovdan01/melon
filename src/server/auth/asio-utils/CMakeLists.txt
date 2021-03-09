add_library(asio-utils
    include/ce/asio_ns.hpp
    include/ce/executor_wrapper.hpp
    include/ce/get_socket.hpp
    include/ce/io_context_signal_interrupter.hpp
    include/ce/socket_session.hpp
    include/ce/spawn.hpp
    include/ce/tcp_listener.hpp
    src/io_context_signal_interrupter.cpp
    src/socket_session.cpp
)

find_package(Boost 1.74 REQUIRED COMPONENTS exception context log)

target_link_libraries(asio-utils PUBLIC
    ce::utils
    Boost::context
    Boost::log
    Boost::exception
)

if(WIN32)
    target_link_libraries(asio-utils PUBLIC ws2_32 mswsock)
endif()

ntc_target(asio-utils ALIAS_NAME ce::asio-utils HEADER_PREFIX ce/asio-utils/)
