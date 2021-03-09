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
#include <boost/multiprecision/cpp_int.hpp>

#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>

namespace ce
{
    namespace
    {
        using bigint = boost::multiprecision::cpp_int;

        using socket_executor_t = ba::strand<ba::io_context::executor_type>;
        using tcp_socket = ba::basic_stream_socket<ba::ip::tcp,socket_executor_t>;
        using tcp_stream = bb::basic_stream<ba::ip::tcp,
            socket_executor_t,
            bb::simple_rate_policy>;

        class my_sasl_session final : public socket_session<my_sasl_session,tcp_stream>
        {
            constexpr static std::size_t number_limit_ = 1024,
                                         bytes_per_second_limit = 1024;
            constexpr static boost::asio::steady_timer::duration time_limit_ = std::chrono::seconds(15);
        public:
            my_sasl_session(ba::io_context::executor_type ex)
                : socket_session<my_sasl_session,tcp_stream>{std::move(ex)}
            {
                stream_.rate_policy().read_limit(bytes_per_second_limit);
            }

            void start_protocol()
            {            
                [[maybe_unused]] auto& server_singletone = melon::server::auth::SaslServerSingleton::get_instance();
                // This captures the pointer to session twice: non-owning as this,
                // and owning as s. We need the owning capture to keep the session
                // alive. Capturing this allows us to omit s-> before accessing
                // any session content at the cost of one additional pointer of state.
                spawn(this->executor(),[this,s=shared_from_this()](auto yc)
                {
                    using namespace boost::log::trivial;

                    namespace mca = melon::core::auth;
                    namespace msa = melon::server::auth;

                    msa::SaslServerConnection server("melon");

                    boost::system::error_code ec;

                    std::string_view supported_mechanisms = server.list_mechanisms();
                    out_buf_ = std::string(supported_mechanisms) + "\n";
                    stream_.expires_after(time_limit_);
                    async_write(stream_,ba::buffer(out_buf_),yc);
                    stream_.expires_after(time_limit_);
                    std::size_t n = async_read_until(stream_, ba::dynamic_string_buffer{in_buf_,number_limit_},'\n',yc[ec]);
                    if(ec)
                    {
                        if(ec!=boost::asio::error::eof)
                            throw boost::system::system_error{ec};
                        BOOST_LOG_SEV(log(),info) << "Connection closed";
                        return;
                    }
                    std::string wanted_mechanism = read_buffered_string(n);
                    if (supported_mechanisms.find(wanted_mechanism) == std::string_view::npos)
                        throw std::runtime_error("Wanted mechanism " + wanted_mechanism + " is not supported by server. Supported mechanisms: " + std::string(supported_mechanisms));
                    out_buf_ = wanted_mechanism + "\n";
                    stream_.expires_after(time_limit_);
                    async_write(stream_,ba::buffer(out_buf_),yc);

                    stream_.expires_after(time_limit_);
                    n = async_read_until(stream_, ba::dynamic_string_buffer{in_buf_,number_limit_},'\n',yc[ec]);
                    std::string client_response = read_buffered_string(n);
                    //BOOST_LOG_SEV(log(),info) << "Readstr ::"<< client_response <<"::";
                    auto [server_response, server_completness] = server.start(wanted_mechanism, client_response);
                            BOOST_LOG_SEV(log(),info) << "I WANTED TO SENT " << server_response <<"::";
                    out_buf_ = std::string(server_response) + "\n";
                    stream_.expires_after(time_limit_);
                    async_write(stream_,ba::buffer(out_buf_),yc);
                    while (server_completness == mca::AuthCompletness::INCOMPLETE)
                    {
                        stream_.expires_after(time_limit_);
                        n = async_read_until(stream_, ba::dynamic_string_buffer{in_buf_,number_limit_},'\n',yc[ec]);
                        //BOOST_LOG_SEV(log(),info) << "INBUF " << in_buf_ <<"::";
                        client_response = read_buffered_string(n);
                        //BOOST_LOG_SEV(log(),info) << "I GOT " << client_response <<"::";
                        mca::StepResult server_step_res = server.step(client_response);
                        server_response = server_step_res.response;
                        out_buf_ = std::string(server_response) + '\n';
                        stream_.expires_after(time_limit_);
                        async_write(stream_,ba::buffer(out_buf_),yc);
                        server_completness = server_step_res.completness;
                    }

                    out_buf_ = "Okay, Mr. Client, here's your token...\n";
                    stream_.expires_after(time_limit_);
                    async_write(stream_,ba::buffer(out_buf_),yc);

                },{},ba::bind_executor(this->cont_executor(),[](std::exception_ptr e)
                {
                    if(e)
                        std::rethrow_exception(e);
                }));
            }
        private:
            std::string in_buf_,out_buf_;
            std::string read_buffered_string(std::size_t n)
            {
                std::string x{in_buf_};
                x = x.substr(0, x.size()-1);
                in_buf_.erase(0,n);
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
                    "TimeStamp","%Y-%m-%d %H:%M:%S.%f"
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
        if(argc<2||argc>3)
            throw std::runtime_error(ce::format("Usage: ",argv[0]," <listen-port> [threads]"));
        auto port = ce::from_chars<std::uint16_t>(argv[1]);
        if(!port||!*port)
            throw std::runtime_error("Port must be in [1;65535]");
        unsigned threads;
        if(argc==3){
            auto t = ce::from_chars<unsigned>(argv[2]);
            if(!t||!*t)
                throw std::runtime_error("Threads must be a non-zero unsigned integer");
            threads = *t;
        }else
            threads = std::thread::hardware_concurrency();
        using namespace boost::log::trivial;
        BOOST_LOG_TRIVIAL(info) << "Using " << threads << " threads.";
        ce::ba::io_context ctx{int(threads)};
        ce::io_context_signal_interrupter iosi{ctx};
        ce::tcp_listener<ce::my_sasl_session, ce::ba::io_context::executor_type> tl{ctx.get_executor(),*port};
        ce::ba::static_thread_pool tp{threads-1};

        for(unsigned i=1;i<threads;++i)
            ce::bae::execute(tp.get_executor(),[&]{
                ctx.run();
            });
        ctx.run();
        return 0;
    }
    catch(...){
        BOOST_LOG_TRIVIAL(fatal) << boost::current_exception_diagnostic_information();
        return 1;
    }
}
catch(...){
    std::cerr << '\n' << boost::current_exception_diagnostic_information() << '\n';
    return 1;
}
