#include <sasl_server_wrapper.hpp>

#include <ce/asio-main.hpp>
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
#include <boost/multiprecision/cpp_int.hpp>

#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>
//////////////////////////////////Add to class: function that takes client output and processes it, with counter
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
            constexpr static boost::asio::steady_timer::duration time_limit_ =
                std::chrono::seconds(15);
        public:
            my_sasl_session(ba::io_context::executor_type ex)
                : socket_session<my_sasl_session,tcp_stream>{std::move(ex)}
            {
                stream_.rate_policy().read_limit(bytes_per_second_limit);
            }

            void start_protocol()
            {            

                // This captures the pointer to session twice: non-owning as this,
                // and owning as s. We need the owning capture to keep the session
                // alive. Capturing this allows us to omit s-> before accessing
                // any session content at the cost of one additional pointer of state.
                spawn(this->executor(),[this,s=shared_from_this()](auto yc){
                    using namespace boost::log::trivial;          
                    for(;;){
                        boost::system::error_code ec;
                        melon::server::auth::SaslServerConnection server("melon");
                        std::string_view supported_mechanisms = server.list_mechanisms();
                        out_buf_ = std::string(supported_mechanisms) + '\n';
                        stream_.expires_after(time_limit_);
                        async_write(stream_,ba::buffer(out_buf_),yc);
                        stream_.expires_after(time_limit_);
                        std::size_t n = async_read_until(stream_,
                                                    ba::dynamic_string_buffer{in_buf_,number_limit_},'\n',yc[ec]);
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
                        out_buf_ = wanted_mechanism;
                        //async_write(stream_,ba::buffer(out_buf_),yc);
                        /*std::size_t n = async_read_until(stream_,
                            ba::dynamic_string_buffer{in_buf_,number_limit_},'\n',yc[ec]);

                        stream_.expires_after(time_limit_);
                        n = async_read_until(stream_,
                            ba::dynamic_string_buffer{in_buf_,number_limit_},'\n',yc);


                        stream_.expires_after(time_limit_);
                        async_write(stream_,ba::buffer(out_buf_),yc);*/
                    }
                },{},ba::bind_executor(this->cont_executor(),[](std::exception_ptr e){
                    // We're executing on the executor that properly logs
                    // unhandled exceptions, so just rethrow.
                    if(e)
                        std::rethrow_exception(e);
                }));
            }
        private:
            std::string in_buf_,out_buf_;
            std::string read_buffered_string(std::size_t n)
            {
                in_buf_[n-1] = '\0';
                std::string x{in_buf_};
                x = x.substr(0, x.size()-1);
                in_buf_.erase(0,n);
                return x;
            }
        };
    }

    int main(std::span<const char* const> args)
    {
        if(args.size()<2||args.size()>3)
            throw std::runtime_error(format("Usage: ",args[0]," <listen-port> [threads]"));
        auto port = from_chars<std::uint16_t>(args[1]);
        if(!port||!*port)
            throw std::runtime_error("Port must be in [1;65535]");
        unsigned threads;
        if(args.size()==3){
            auto t = from_chars<unsigned>(args[2]);
            if(!t||!*t)
                throw std::runtime_error("Threads must be a non-zero unsigned integer");
            threads = *t;
        }else
            threads = std::thread::hardware_concurrency();
        using namespace boost::log::trivial;
        BOOST_LOG_TRIVIAL(info) << "Using " << threads << " threads.";
        ba::io_context ctx{int(threads)};
        io_context_signal_interrupter iosi{ctx};
        tcp_listener<my_sasl_session,ba::io_context::executor_type> tl{ctx.get_executor(),*port};
        ba::static_thread_pool tp{threads-1};

        namespace msa = melon::server::auth;
        [[maybe_unused]] auto& server_singletone = msa::SaslServerSingleton::get_instance();

        for(unsigned i=1;i<threads;++i)
            bae::execute(tp.get_executor(),[&]{
                ctx.run();
            });
        ctx.run();
        return 0;
    }
}


/*#include <cassert>
#include <iostream>

int main() try
{
    namespace mca = melon::core::auth;
    namespace msa = melon::server::auth;

    [[maybe_unused]] auto& server_singletone = msa::SaslServerSingleton::get_instance();

    auto& client_singletone = mca::SaslClientSingleton::get_instance();
    mca::Credentials credentials = { "john", "doe" };
    client_singletone.set_credentials(&credentials);

    msa::SaslServerConnection server("melon");
    std::string_view supported_mechanisms = server.list_mechanisms();

    mca::SaslClientConnection client("melon");
    const std::string wanted_mechanism = "SCRAM-SHA-256";
    if (supported_mechanisms.find(wanted_mechanism) == std::string_view::npos)
        throw std::runtime_error("Wanted machanism " + wanted_mechanism + " is not supported by server. Supported mechanisms: " + std::string(supported_mechanisms));

    auto [client_response,  selected_mechanism] = client.start(wanted_mechanism);
    assert(selected_mechanism == wanted_mechanism);

    auto [server_response, server_completness] = server.start(selected_mechanism, client_response);

    while (server_completness == mca::AuthCompletness::INCOMPLETE)
    {
        std::cout << "Performing extra auth step..." << std::endl;
        mca::StepResult client_step_res = client.step(server_response);
        mca::StepResult server_step_res = server.step(client_step_res.response);
        server_response = server_step_res.response;
        server_completness = server_step_res.completness;
    }

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
*/
