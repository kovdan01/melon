#include <sasl_server_wrapper.hpp>

#include <ce/charconv.hpp>
#include <ce/format.hpp>
#include <ce/io_context_signal_interrupter.hpp>
#include <ce/socket_session.hpp>
#include <ce/spawn.hpp>
#include <ce/tcp_listener.hpp>

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/execution/execute.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/static_thread_pool.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/program_options.hpp>

#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>

namespace ce
{
    namespace
    {
        using socket_executor_t = ba::strand<ba::io_context::executor_type>;
        using tcp_socket = ba::basic_stream_socket<ba::ip::tcp, socket_executor_t>;
        using tcp_stream = bb::basic_stream<ba::ip::tcp,
            socket_executor_t,
            bb::simple_rate_policy>;

        class MySaslSession final : public socket_session<MySaslSession, tcp_stream>
        {
            constexpr static std::size_t NUMBER_LIMIT = 1024,
                                         BYTES_PER_SECOND_LIMIT = 1024;
            constexpr static boost::asio::steady_timer::duration time_limit_ = std::chrono::seconds(15);
        public:
            MySaslSession(ba::io_context::executor_type ex)
                : socket_session<MySaslSession, tcp_stream>{std::move(ex)}
            {
                stream_.rate_policy().read_limit(BYTES_PER_SECOND_LIMIT);
            }

            void start_protocol()
            {            
                [[maybe_unused]] auto& server_singletone = melon::server::auth::SaslServerSingleton::get_instance();
                // This captures the pointer to session twice: non-owning as this,
                // and owning as s. We need the owning capture to keep the session
                // alive. Capturing this allows us to omit s-> before accessing
                // any session content at the cost of one additional pointer of state.
                spawn(this->executor(), [this, s=shared_from_this()](auto yc)
                {
                    using namespace boost::log::trivial;

                    namespace mca = melon::core::auth;
                    namespace msa = melon::server::auth;

                    msa::SaslServerConnection server("melon");

                    boost::system::error_code ec;

                    std::string_view supported_mechanisms = server.list_mechanisms();
                    m_out_buf = std::string(supported_mechanisms) + "\n";
                    stream_.expires_after(time_limit_);
                    async_write(stream_, ba::buffer(m_out_buf), yc);
                    stream_.expires_after(time_limit_);
                    std::size_t n = async_read_until(stream_, ba::dynamic_string_buffer{m_in_buf, NUMBER_LIMIT}, '\n', yc[ec]);
                    if (ec)
                    {
                        if (ec!=boost::asio::error::eof)
                            throw boost::system::system_error{ec};
                        BOOST_LOG_SEV(log(), info) << "Connection closed";
                        return;
                    }
                    std::string wanted_mechanism = read_buffered_string(n);
                    if (supported_mechanisms.find(wanted_mechanism) == std::string_view::npos)
                        throw std::runtime_error("Wanted mechanism " + wanted_mechanism + " is not supported by server. Supported mechanisms: " + std::string(supported_mechanisms));
                    m_out_buf = wanted_mechanism + "\n";
                    stream_.expires_after(time_limit_);
                    async_write(stream_, ba::buffer(m_out_buf), yc);

                    stream_.expires_after(time_limit_);
                    n = async_read_until(stream_, ba::dynamic_string_buffer{m_in_buf, NUMBER_LIMIT}, '\n', yc[ec]);
                    std::string client_response = read_buffered_string(n);
                    auto [server_response, server_completness] = server.start(wanted_mechanism, client_response);
                    m_out_buf = std::string(server_response) + "\n";
                    stream_.expires_after(time_limit_);
                    async_write(stream_, ba::buffer(m_out_buf), yc);
                    while (server_completness == mca::AuthCompletness::INCOMPLETE)
                    {
                        stream_.expires_after(time_limit_);
                        n = async_read_until(stream_, ba::dynamic_string_buffer{m_in_buf, NUMBER_LIMIT}, '\n', yc[ec]);
                        client_response = read_buffered_string(n);
                        mca::StepResult server_step_res = server.step(client_response);
                        server_response = server_step_res.response;
                        server_completness = server_step_res.completness;
                        if (server_response.data() != nullptr)
                            m_out_buf = std::string(server_response) + '\n';
                        else
                            m_out_buf = "";
                        if (server_completness == mca::AuthCompletness::COMPLETE)
                            break;
                        stream_.expires_after(time_limit_);
                        async_write(stream_, ba::buffer(m_out_buf), yc);
                    }
                    m_out_buf = "Okay, Mr. Client, here's your token...\n";
                    stream_.expires_after(time_limit_);
                    async_write(stream_, ba::buffer(m_out_buf), yc);
                    BOOST_LOG_TRIVIAL(info) << "Issued a token.";
                }, {}, ba::bind_executor(this->cont_executor(), [](std::exception_ptr e)
                {
                    if (e)
                        std::rethrow_exception(e);
                }));
            }
        private:
            std::string m_in_buf, m_out_buf;
            std::string read_buffered_string(std::size_t n)
            {
                std::string x = m_in_buf.substr(0, n-1);
                m_in_buf.erase(0, n);
                return x;
            }
        };
    }
}

int main(int argc, char* argv[]) try
{
    // Setup logging and failsafe exception handlers.
    std::ios_base::sync_with_stdio(false);
    namespace bl = boost::log;
    bl::add_console_log(std::cerr,
                        bl::keywords::format = (
                bl::expressions::stream
                << bl::expressions::format_date_time<boost::posix_time::ptime>(
                    "TimeStamp", "%Y-%m-%d %H:%M:%S.%f"
                )
                << " [" << bl::trivial::severity << "] T"
                << bl::expressions::attr<bl::attributes::current_thread_id::value_type>("ThreadID")
                << " @" << ce::remote
                << " : " << bl::expressions::smessage
                ),
                        bl::keywords::auto_flush = true
            );
    bl::add_common_attributes();
    try
    {
        namespace po =  boost::program_options;
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("listen-port", po::value<unsigned short>(), "port to listen on")
            ("threads", po::value<unsigned>(),"number of threads to use");
        po::variables_map vm;
        po::positional_options_description p;
        p.add("listen-port", 1);
        p.add("threads", 2);
        try
        {
            po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
            po::notify(vm);
        }
        catch (const po::error& error)
        {
            std::cerr << "Error while parsing command-line arguments: "
                      << error.what() << "\nPlease use --help to see help message\n";
            return 1;
        }

        std::string usage = ce::format("Usage: ", argv[0], " <listen-port> [threads]");

        if (vm.count("help"))
        {
            std::cout << desc << '\n' << usage << '\n';
            return 0;
        }
        std::optional<unsigned short> port;
        unsigned threads;
        if (vm.count("listen-port"))
        {
            port = vm["listen-port"].as<unsigned short>();
            if (!port||!*port)
                throw std::runtime_error("Port must be in [1;65535]");
            if (vm.count("threads"))
            {
                std::optional<unsigned> t = vm["threads"].as<unsigned>();
                if (!t||!*t)
                    throw std::runtime_error("Threads must be a non-zero unsigned integer");
                threads = *t;
            }
            else
                threads = std::thread::hardware_concurrency();
        }
        else
        {
            throw std::runtime_error(usage);
        }

        using namespace boost::log::trivial;
        BOOST_LOG_TRIVIAL(info) << "Using " << threads << " threads.";
        ce::ba::io_context ctx{int(threads)};
        ce::io_context_signal_interrupter iosi{ctx};
        ce::tcp_listener<ce::MySaslSession, ce::ba::io_context::executor_type> tl{ctx.get_executor(), *port};
        ce::ba::static_thread_pool tp{threads-1};

        for(unsigned i=1;i<threads;++i)
            ce::bae::execute(tp.get_executor(), [&]{
                ctx.run();
            });
        ctx.run();
        return 0;
    }
    catch(...)
    {
        BOOST_LOG_TRIVIAL(fatal) << boost::current_exception_diagnostic_information();
        return 1;
    }
}
catch(...){
    std::cerr << '\n' << boost::current_exception_diagnostic_information() << '\n';
    return 1;
}
