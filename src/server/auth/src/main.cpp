#include <melon/core/log_configuration.hpp>
#include <melon/core/serialization.hpp>
#include <sasl_server_wrapper.hpp>

#include <ce/format.hpp>
#include <ce/io_context_signal_interrupter.hpp>
#include <ce/spawn.hpp>
#include <ce/tcp_listener.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/static_thread_pool.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>

namespace ba = boost::asio;
namespace bb = boost::beast;
namespace bae = boost::asio::execution;

using socket_executor_t = ba::strand<ba::io_context::executor_type>;
using tcp_socket = ba::basic_stream_socket<ba::ip::tcp, socket_executor_t>;
using tcp_stream = bb::basic_stream<ba::ip::tcp,
                                    socket_executor_t,
                                    bb::simple_rate_policy>;

class MySaslSession final : public ce::socket_session<MySaslSession, tcp_stream>
{
    constexpr static std::size_t NUMBER_LIMIT = 1024,
                                 BYTES_PER_SECOND_LIMIT = 1024;
    constexpr static boost::asio::steady_timer::duration TIME_LIMIT = std::chrono::seconds(15);
public:
    MySaslSession(const ba::io_context::executor_type& ex)
        : socket_session<MySaslSession, tcp_stream>{ex}
    {
        stream_.rate_policy().read_limit(BYTES_PER_SECOND_LIMIT);
    }

    void start_protocol()
    {
        namespace mca = melon::core::auth;
        namespace msa = melon::server::auth;

        [[maybe_unused]] auto& server_singletone = msa::SaslServerSingleton::get_instance();
        ce::spawn(this->executor(), [this, s=shared_from_this()](auto yc)
        {
            using namespace boost::log::trivial;
            boost::system::error_code ec;

            msa::SaslServerConnection server("melon");

            std::string_view supported_mechanisms = server.list_mechanisms();
            async_send_serialized(stream_, supported_mechanisms, yc);

            std::string wanted_mechanism = async_recieve_serialized(stream_, yc, NUMBER_LIMIT, ec);
            if (ec)
            {
                if (ec != boost::asio::error::eof)
                    throw boost::system::system_error{ec};
                BOOST_LOG_SEV(log(), info) << "Connection closed";
                return;
            }
            if (supported_mechanisms.find(wanted_mechanism) == std::string_view::npos)
                throw std::runtime_error("Wanted mechanism " + wanted_mechanism + " is not supported by server. Supported mechanisms: " + std::string(supported_mechanisms));
            m_out_buf = wanted_mechanism + "\n";
            stream_.expires_after(TIME_LIMIT);
            ba::async_write(stream_, ba::buffer(m_out_buf), yc, 0);

            stream_.expires_after(TIME_LIMIT);
            std::size_t n = ba::async_read_until(stream_, ba::dynamic_string_buffer{m_in_buf, NUMBER_LIMIT}, '\n', yc[ec], 0);
            std::string client_response = read_erase_in_buf(n);
            auto [server_response, server_completness] = server.start(wanted_mechanism, client_response);
            m_out_buf = std::string(server_response) + '\n';

            while (server_completness == mca::AuthState::INCOMPLETE)
            {
                stream_.expires_after(TIME_LIMIT);
                ba::async_write(stream_, ba::buffer(m_out_buf), yc, 0);
                stream_.expires_after(TIME_LIMIT);
                n = ba::async_read_until(stream_, ba::dynamic_string_buffer{m_in_buf, NUMBER_LIMIT}, '\n', yc[ec], 0);
                client_response = read_erase_in_buf(n);
                mca::StepResult server_step_result = server.step(client_response);
                server_response = server_step_result.response;
                server_completness = server_step_result.completness;

                if (server_completness != mca::AuthState::INCOMPLETE)
                    break;

                if (server_response.data() != nullptr)
                    m_out_buf = std::string(server_response) + '\n';
                else
                    m_out_buf = "";
            }
            assert(server_completness != mca::AuthState::INCOMPLETE);

            switch (server_completness)
            {
            case mca::AuthState::COMPLETE:
                m_out_buf = mca::AuthResultSingleton::get_instance().success() + '\n';
                BOOST_LOG_TRIVIAL(info) << "Issued a token";
                break;
            case mca::AuthState::USER_NOT_FOUND:
                m_out_buf = mca::AuthResultSingleton::get_instance().failure() + '\n';
                BOOST_LOG_TRIVIAL(info) << "User not found";
                break;
            case mca::AuthState::BAD_PROTOCOL:
                m_out_buf = mca::AuthResultSingleton::get_instance().failure() + '\n';
                BOOST_LOG_TRIVIAL(info) << "Protocol error";
                break;
            case mca::AuthState::AUTHENTICATION_FAILURE:
                m_out_buf = mca::AuthResultSingleton::get_instance().failure() + '\n';
                BOOST_LOG_TRIVIAL(info) << "Authentication failure";
                break;
            case mca::AuthState::AUTHORIZATION_FAILURE:
                m_out_buf = mca::AuthResultSingleton::get_instance().failure() + '\n';
                BOOST_LOG_TRIVIAL(info) << "Authorization failure";
                break;
            }

            stream_.expires_after(TIME_LIMIT);
            ba::async_write(stream_, ba::buffer(m_out_buf), yc, 0);

        }, {}, ba::bind_executor(this->cont_executor(), [](std::exception_ptr e)  // NOLINT (performance-unnecessary-value-param)
        {
            if (e)
                std::rethrow_exception(e);
        }));
    }

private:
    std::string m_in_buf;
    std::string m_out_buf;

    // The function copies string read before the delimiter and erases that part of buffer
    std::string read_erase_in_buf(std::size_t n)
    {
        std::string before_separator = std::move(m_in_buf);
        m_in_buf = std::string(before_separator.c_str() + n + 1, before_separator.size() - n);
        before_separator.erase(n - 1,  before_separator.size() - 1);
        return before_separator;
    }

    template<typename Stream, typename YC>
    std::string async_recieve_serialized(Stream& stream, YC& yc, std::size_t limit, boost::system::error_code ec)
    {
        std::uint32_t recieve_size;
        ba::read(stream, boost::asio::buffer(&recieve_size, sizeof(recieve_size)), boost::asio::transfer_exactly(sizeof(recieve_size)), 0);
        stream_.expires_after(TIME_LIMIT);
        std::size_t n = ba::async_read(stream, boost::asio::dynamic_string_buffer{m_in_buf, limit}, boost::asio::transfer_exactly(recieve_size), yc[ec], 0);
        std::string reply = melon::core::serialization::deserialize<std::string>(m_in_buf);
        m_in_buf.erase(0, n);
        return reply;
    }

    template<typename Stream, typename What, typename YC>
    void async_send_serialized(Stream& stream, const What& what, YC& yc)
    {
        stream_.expires_after(TIME_LIMIT);
        auto [send_size, serialized_data] = melon::core::serialization::serialize(what);
        ba::write(stream, ba::buffer(&send_size, sizeof(send_size)));
        ba::async_write(stream, ba::buffer(serialized_data), yc, 0);
    }

};

int main(int argc, char* argv[]) try
{
    std::ios_base::sync_with_stdio(false);
    melon::core::log::setup();

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

        namespace ba = boost::asio;

        ba::io_context ctx{int(threads)};
        ce::io_context_signal_interrupter iosi{ctx};
        ce::tcp_listener<MySaslSession, ba::io_context::executor_type> tl{ctx.get_executor(), port};
        ba::static_thread_pool tp{threads - 1};

        for (unsigned i = 1; i < threads; ++i)
        {
            bae::execute(tp.get_executor(), [&ctx]{ ctx.run(); });
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
    std::cerr << '\n' << boost::current_exception_diagnostic_information() << '\n';
    return 1;
}
