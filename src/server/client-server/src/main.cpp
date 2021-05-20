#include <melon/core.hpp>
#include <melon/core/commands.hpp>
#include <melon/core/entities.hpp>
#include <melon/core/log_configuration.hpp>
#include <melon/core/session.hpp>
#include <melon/core/token.hpp>

#include <ce/tcp_listener.hpp>

#include <boost/asio/static_thread_pool.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/functional/hash.hpp>
#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <thread>

#include <ctime>
#include <limits>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mc = melon::core;
namespace ba = boost::asio;

class User : public mc::User
{
public:
    User(std::string username, mc::id_t domain_id)
        : mc::User(std::move(username), domain_id)
    {
    }
};

static bool operator==(const User& lhs, const User& rhs)
{
    // TODO: use either string or numeric identifiers
    return std::tuple(lhs.username(), lhs.domain_id()) == std::tuple(rhs.username(), rhs.domain_id());
}

struct UserHasher
{
    std::size_t operator()(const mc::User& u) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine<std::string>(seed, u.username());
        boost::hash_combine<mc::id_t>(seed, u.domain_id());
        return seed;
    }
};

static std::unordered_map<User, std::unordered_set<mc::Token, mc::TokenHasher>, UserHasher> g_session_tracker =
{
    {
        { "igor", 42 },
        { mc::Token(),
          mc::Token
          ({
              0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
              0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
              0x20, 0x21, 0x22, 0x23, 0x24, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
              0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
          })
        }
    },
    {
        { "lana", 42 },
        { mc::Token
          ({
              0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
              0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
              0x20, 0x21, 0x22, 0x23, 0x24, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
              0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
          })
        }
    },
};

#define MELON_CHECK_BA_ERROR_CODE(ec)                       \
    if (ec)                                                 \
    {                                                       \
        if ((ec) != ba::error::eof)                         \
            throw boost::system::system_error{ec};          \
        BOOST_LOG_SEV(log(), info) << "Connection closed";  \
        return;                                             \
    }                                                       \

class ClientServerSession final : public mc::AsyncSession
{
public:
    ClientServerSession(const ba::io_context::executor_type& ex)
        : mc::AsyncSession(ex)
    {
    }

    void start_protocol() final
    {
        ce::spawn(this->executor(), [this, s=shared_from_this()](auto yc)
        {
            using namespace boost::log::trivial;
            boost::system::error_code ec;

            for (;;)
            {
                auto code = static_cast<mc::ClientCode>(async_recieve_code(yc, ec));
                MELON_CHECK_BA_ERROR_CODE(ec)

                mc::StringViewOverBinary username = async_recieve(NUMBER_LIMIT, yc, ec);
                auto domain_id = async_recieve<mc::id_t>(NUMBER_LIMIT, yc, ec);
                User user(std::string(username.view().data(), username.view().size()), domain_id);

                switch (code)
                {
                case mc::ClientCode::SEND_TOKEN:
                {
                    auto token = async_recieve<mc::Token::token_t>(NUMBER_LIMIT, yc, ec);
                    MELON_CHECK_BA_ERROR_CODE(ec);

                    auto it = g_session_tracker.find(user);
                    if (it != g_session_tracker.end())
                    {
                        if (std::find_if(it->second.begin(), it->second.end(),
                                         [&](mc::Token const& t){ return t.token() == token; }) != it->second.end())
                        {
                            BOOST_LOG_SEV(log(), info) << "Recieved nice token for user " << username.view() << '@' << domain_id;
                        }
                        else
                        {
                            BOOST_LOG_SEV(log(), info) << "Invalid token for user " << username.view() << '@' << domain_id;
                            return;
                        }
                    }
                    else
                    {
                        BOOST_LOG_SEV(log(), info) << "No tokens found for user " << username.view() << '@' << domain_id;
                        return;
                    }

                    break;
                }
                case mc::ClientCode::REQUEST_TOKEN:
                {
                    mc::SyncSession auth("localhost", "6666");
                    mc::ServerCode auth_code;
                    for (;;)
                    {
                        auth_code = static_cast<mc::ServerCode>(auth.receive_code());
                        if (auth_code != mc::ServerCode::SEND_SASL_DATA)
                            break;

                        auto server_data = auth.receive();
                        async_send(server_data, yc, ec);
                        MELON_CHECK_BA_ERROR_CODE(ec);

                        auto client_data = async_recieve(NUMBER_LIMIT, yc, ec);
                        MELON_CHECK_BA_ERROR_CODE(ec);
                        auth.send(client_data);
                    }

                    switch (auth_code)
                    {
                    case mc::ServerCode::AUTH_PROBLEM:
                    {
                        BOOST_LOG_SEV(log(), info) << "Auth problem";
                        return;
                    }
                    case mc::ServerCode::ISSUED_A_TOKEN:
                    {
                        BOOST_LOG_SEV(log(), info) << "Issued a token";
                        async_send_code(static_cast<std::uint32_t>(mc::ServerCode::ISSUED_A_TOKEN), yc, ec);
                        MELON_CHECK_BA_ERROR_CODE(ec);
                        mc::Token token;
                        g_session_tracker[user].emplace(token);
                        async_send<mc::Token::token_t>(token.token(), yc, ec);
                        MELON_CHECK_BA_ERROR_CODE(ec);
                        break;
                    }
                    default:
                    {
                        BOOST_LOG_SEV(log(), info) << "Unexpected code";
                        return;
                    }
                    }

                    break;
                }
                }
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
        ce::tcp_listener<ClientServerSession, ba::io_context::executor_type> tl{ctx.get_executor(), port};
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
