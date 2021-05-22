#include <melon/core.hpp>
#include <melon/core/log_configuration.hpp>
#include <melon/core/session.hpp>

#include <boost/asio/static_thread_pool.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <thread>

namespace mc = melon::core;
namespace ba = boost::asio;

#define MELON_CHECK_BA_ERROR_CODE(ec)                       \
    if (ec)                                                 \
    {                                                       \
        if ((ec) != ba::error::eof)                         \
            throw boost::system::system_error{ec};          \
        BOOST_LOG_SEV(log(), info) << "Connection closed";  \
        return;                                             \
    }                                                       \


enum class OpCode : std::uint8_t
{
    ADD = 0,
    UPDATE = 1,
    DELETE = 2,
    //SMTHELSE = 3,
};

enum class TypeCode : std::uint8_t
{
    DOMAIN = 0,
    USER = 1,
    CHAT = 2,
    MESSAGE = 3,
};

class StorageSession final: public mc::AsyncSession
{
public:
    StorageSession(const ba::io_context::executor_type& ex)
        : mc::AsyncSession(ex)
    {
    }

    void start_protocol()
    {
        ce::spawn(this->executor(), [this, s=shared_from_this()](auto yc)
        {
            using namespace boost::log::trivial;
            boost::system::error_code ec;

            //for (;;)
            //{
                std::string_view send_op = "0";
                async_send(send_op, yc, ec);
                auto op = async_recieve(NUMBER_LIMIT, yc, ec); // add, update, ..
                MELON_CHECK_BA_ERROR_CODE(ec);

                std::string_view send_type = "2";
                async_send(send_type, yc, ec);
                auto type = async_recieve(NUMBER_LIMIT, yc, ec); // user, message, ...
                MELON_CHECK_BA_ERROR_CODE(ec);

//                std::tuple send_body = std::tuple(1, 2, "new_chatname");
//                async_send(send_body, yc, ec);
//                auto body = async_recieve(NUMBER_LIMIT, yc, ec); // "5, 2, "new_chatname"", ...
//                MELON_CHECK_BA_ERROR_CODE(ec);
            //}

        }, {}, ba::bind_executor(this->cont_executor(), [](std::exception_ptr e)  // NOLINT (performance-unnecessary-value-param)
        {
            if (e)
            std::rethrow_exception(e);
        }));
    }
};

int main(int argc, char* argv[])
{
    std::ios_base::sync_with_stdio(false);
    mc::log::setup();

    try
    {
        namespace po =  boost::program_options;
        po::options_description desc("Allowed options");

        desc.add_options()
                ("help", "produce help message")
                ("listen-port", po::value<std::uint16_t>()->required(),                                     "port to listen on")
                ("threads",     po::value<unsigned>()->default_value(std::thread::hardware_concurrency()),  "number of threads to use");
        po::variables_map vm;
        po::positional_options_description p;
        p.add("listen-port", 1);
        p.add("threads", 2);

        std::string usage = ce::format("Usage: ", argv[0], " <listen-port> [threads]");
        try
        {
            po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
            if (vm.contains("help"))
            {
                std::cout << desc << '\n' << usage << '\n';
                return 0;
            }
            po::notify(vm);
        }

        catch (const po::error& error)
        {
            std::cerr << "Error while parsing command-line arguments: "
                        << error.what() << "\nPlease use --help to see help message\n";
            return 1;
        }

        std::uint16_t port  = vm["listen-port"].as<std::uint16_t>();
        unsigned threads    = vm["threads"].as<unsigned>();
        if (port == 0)
        {
            throw std::runtime_error("Port must be in [1;65535]");
        }
        if (threads == 0)
        {
            throw std::runtime_error("Threads must be a non-zero unsigned integer");
        }

        using namespace boost::log::trivial;
        BOOST_LOG_TRIVIAL(info) << "Using " << threads << " threads.";

        ba::io_context ctx{int(threads)};
        ce::io_context_signal_interrupter iosi{ctx};
        ce::tcp_listener<StorageSession, ba::io_context::executor_type> tl{ctx.get_executor(), port};
        ba::static_thread_pool tp{threads - 1};

        for (unsigned i = 1; i < threads; ++i)
        {
            ba::execution::execute(tp.get_executor(), [&ctx]{ ctx.run(); });
        }
        ctx.run();
        return 0;

    } catch (...) {}
}
