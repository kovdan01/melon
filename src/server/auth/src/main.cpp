#include <melon/core.hpp>
#include <melon/core/commands.hpp>
#include <melon/core/log_configuration.hpp>
#include <melon/core/session.hpp>
#include <sasl_server_wrapper.hpp>

#include <ce/tcp_listener.hpp>

#include <boost/asio/static_thread_pool.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <thread>

namespace mc = melon::core;
namespace mca = melon::core::auth;
namespace msa = melon::server::auth;
namespace ba = boost::asio;

#define MELON_CHECK_BA_ERROR_CODE(ec)                       \
    if (ec)                                                 \
    {                                                       \
        if ((ec) != ba::error::eof)                         \
            throw boost::system::system_error{ec};          \
        BOOST_LOG_SEV(log(), info) << "Connection closed";  \
        return;                                             \
    }                                                       \

class MySaslSession final : public mc::AsyncSession
{
public:
    MySaslSession(const ba::io_context::executor_type& ex)
        : mc::AsyncSession(ex)
    {
        [[maybe_unused]] auto& server_singletone = msa::SaslServerSingleton::get_instance();
    }

    void start_protocol() final
    {
        ce::spawn(this->executor(), [this, s=shared_from_this()](auto yc)
        {
            using namespace boost::log::trivial;
            boost::system::error_code ec;

            msa::SaslServerConnection server("melon");

            // TODO:
            // 1) should ec be passed to all async_* functions?
            // 2) if yes, should ec be checked after every async_* function?

            std::string_view supported_mechanisms = server.list_mechanisms();
            async_send_code(static_cast<std::uint32_t>(mc::ServerCode::SEND_SASL_DATA), yc, ec);
            async_send(supported_mechanisms, yc, ec);
            MELON_CHECK_BA_ERROR_CODE(ec);

            mc::StringViewOverBinary wanted_mechanism(async_recieve(NUMBER_LIMIT, yc, ec));
            MELON_CHECK_BA_ERROR_CODE(ec);
            if (supported_mechanisms.find(wanted_mechanism.view()) == std::string_view::npos)
            {
                throw std::runtime_error("Wanted mechanism " + std::string(wanted_mechanism.view()) + " is not supported by server. "
                                         "Supported mechanisms: " + std::string(supported_mechanisms));
            }

            async_send_code(static_cast<std::uint32_t>(mc::ServerCode::SEND_SASL_DATA), yc, ec);
            async_send(wanted_mechanism.view(), yc, ec);
            MELON_CHECK_BA_ERROR_CODE(ec);

            auto client_response = async_recieve(NUMBER_LIMIT, yc, ec);
            MELON_CHECK_BA_ERROR_CODE(ec);
            auto [server_response, server_completness] = server.start(wanted_mechanism.view(), { client_response.data(), client_response.size() });
            async_send_code(static_cast<std::uint32_t>(mc::ServerCode::SEND_SASL_DATA), yc, ec);
            async_send(server_response, yc, ec);
            MELON_CHECK_BA_ERROR_CODE(ec);

            for (;;)
            {
                client_response = async_recieve(NUMBER_LIMIT, yc, ec);
                MELON_CHECK_BA_ERROR_CODE(ec);

                mca::StepResult server_step_result = server.step({ client_response.data(), client_response.size() });
                server_response = server_step_result.response;
                server_completness = server_step_result.completness;

                if (server_completness == mca::AuthState::INCOMPLETE)
                {
                    async_send_code(static_cast<std::uint32_t>(mc::ServerCode::SEND_SASL_DATA), yc, ec);
                    async_send(server_response, yc, ec);
                    MELON_CHECK_BA_ERROR_CODE(ec);
                }
                else
                {
                    break;
                }
            }
            assert(server_completness != mca::AuthState::INCOMPLETE);

            switch (server_completness)
            {
            case mca::AuthState::COMPLETE:
                BOOST_LOG_SEV(log(), info) << "Issued a token";
                break;
            case mca::AuthState::USER_NOT_FOUND:
                BOOST_LOG_SEV(log(), info) << "User not found";
                break;
            case mca::AuthState::BAD_PROTOCOL:
                BOOST_LOG_SEV(log(), info) << "Protocol error";
                break;
            case mca::AuthState::AUTHENTICATION_FAILURE:
                BOOST_LOG_SEV(log(), info) << "Authentication failure";
                break;
            case mca::AuthState::AUTHORIZATION_FAILURE:
                BOOST_LOG_SEV(log(), info) << "Authorization failure";
                break;
            }

            switch (server_completness)
            {
            case mca::AuthState::COMPLETE:
                async_send_code(static_cast<std::uint32_t>(mc::ServerCode::ISSUED_A_TOKEN), yc, ec);
                break;
            default:
                async_send_code(static_cast<std::uint32_t>(mc::ServerCode::AUTH_PROBLEM), yc, ec);
                break;
            }

        }, {}, ba::bind_executor(this->cont_executor(), [](std::exception_ptr e)  // NOLINT (performance-unnecessary-value-param)
        {
            if (e)
                std::rethrow_exception(e);
        }));
    }
};

int main(int argc, char* argv[]) try
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
        ce::tcp_listener<MySaslSession, ba::io_context::executor_type> tl{ctx.get_executor(), port};
        ba::static_thread_pool tp{threads - 1};

        for (unsigned i = 1; i < threads; ++i)
        {
            ba::execution::execute(tp.get_executor(), [&ctx]{ ctx.run(); });
        }
        ctx.run();
        return 0;
    }
    catch (...)
    {
        BOOST_LOG_TRIVIAL(fatal) << boost::current_exception_diagnostic_information();
        return 1;
    }
}
catch (...)
{
    BOOST_LOG_TRIVIAL(fatal) << boost::current_exception_diagnostic_information();
    return 1;
}
