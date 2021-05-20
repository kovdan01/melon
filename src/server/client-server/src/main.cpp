#include <melon/core.hpp>
#include <melon/core/entities.hpp>

#include <melon/core.hpp>
#include <melon/core/log_configuration.hpp>
#include <melon/core/session.hpp>

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

class Token
{
public:
    using token_t = std::array<mc::byte_t, 64>;

    Token()
    {
        for (mc::byte_t& byte : m_token)
        {
            byte = m_byte_dist(m_prng);
        }
    }

    const token_t& token() const
    {
        return m_token;
    }

    bool friend operator==(const Token& lhs, const Token& rhs) noexcept;

private:
    std::time_t timestamp{std::time(nullptr)};
    token_t m_token;

    static std::mt19937_64 m_prng;
    static std::uniform_int_distribution<mc::byte_t> m_byte_dist;
};

std::mt19937_64 Token::m_prng{std::random_device{}()};
std::uniform_int_distribution<mc::byte_t> Token::m_byte_dist{std::numeric_limits<mc::byte_t>::min(),
                                                             std::numeric_limits<mc::byte_t>::max()};

bool operator==(const Token& lhs, const Token& rhs) noexcept
{
    return lhs.m_token == rhs.m_token;
}

struct TokenHasher
{
    std::size_t operator()(const Token& x) const
    {
        std::size_t seed = 0;
        for (mc::byte_t byte : x.token())
            boost::hash_combine<mc::byte_t>(seed, byte);
        return seed;
    }
};

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

static std::unordered_map<User, std::unordered_set<Token, TokenHasher>, UserHasher> g_session_tracker =
{
    {
        { "igor", 42 },
        { Token() }
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

            mc::StringViewOverBinary username = async_recieve(NUMBER_LIMIT, yc, ec);
            auto domain_id = async_recieve<mc::id_t>(NUMBER_LIMIT, yc, ec);
            User user(std::string(username.view().data(), username.view().size()), domain_id);

            auto token = async_recieve<Token::token_t>(NUMBER_LIMIT, yc, ec);
            MELON_CHECK_BA_ERROR_CODE(ec);

            auto it = g_session_tracker.find(user);
            if (it != g_session_tracker.end())
            {
                if (std::find_if(it->second.begin(), it->second.end(),
                                 [&](Token const& t){ return t.token() == token; }) != it->second.end())
                {
                    // Token is good, token is nice
                    BOOST_LOG_TRIVIAL(info) << "Recieved nice token";
                }
                else
                {
                    // No token? That's problematic sweetie
                    BOOST_LOG_TRIVIAL(info) << "Giving a token";
                }
            }
            else
            {
                // No token? That's problematic sweetie
                BOOST_LOG_TRIVIAL(info) << "Adding user to table and giving a token";
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
