#ifndef UUID_0422FEF5_A50E_4618_AB6C_57BA2F417A2E
#define UUID_0422FEF5_A50E_4618_AB6C_57BA2F417A2E

#include <ce/asio_ns.hpp>

#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/execution/execute.hpp>
#include <boost/asio/prefer.hpp>
#include <boost/asio/execution/outstanding_work.hpp>
#include <boost/asio/execution/relationship.hpp>
#include <boost/context/fiber.hpp>
#include <boost/system/system_error.hpp>

#include <exception>
#include <memory>
#include <tuple>
#include <utility>

namespace ce
{
    // This is an implementation of stackfull coroutines that are integrated with
    // ASIO completion token mechanism. The differences from boost::asio::spawn are:
    // - Boost.Context is used directly without relying on deprecated Boost.Coroutine1.
    //   Instead of coroutine attributes, stack allocators are used for customization.
    // - Like with newer boost::asio::co_spawn, ce::spawn is a proper async initiation
    //   function that takes a completion token to invoke when the coroutine is done,
    //   executors are mandatory, unhandled exceptions in coroutine are reported to
    //   the completion handler as std::exception_ptr. Unlike co_spawn though, we don't
    //   support normal return values from coroutine, coroutines are always void-returning.

    namespace detail
    {
        // Instance of fiber_data class template holds all the state of the coroutine.
        // It is split into several inherited bases to reduce duplication
        // among various instantiations. The state consists of:
        // - fiber object, that stores our coroutine when it's suspended
        //   and the resumption context (original thread stack of caller) otherwise.
        // - Executors of coroutine and completion handler with outstanding
        //   work tracking preferred.
        // - Completion handler itself.
        // Since simple executors and handlers are frequently empty classes,
        // we use [[no_unique_address]] attribute to reduce the size of state.

        struct fiber_data_base
        {
            boost::context::fiber f_;
        };

        template<typename Executor>
        struct fiber_data_executor_part
        {
            using work_t = typename ba::prefer_result<Executor,
                                                      bae::outstanding_work_t::tracked_t>::type;

            [[no_unique_address]] work_t work_;

            fiber_data_executor_part(Executor ex)
                : work_{ba::prefer(std::move(ex),bae::outstanding_work.tracked)}
            {}
        };

        template<typename Handler>
        struct fiber_data_handler_part
        {
            using work_executor_t = ba::associated_executor_t<Handler>;
            using work2_t = typename ba::prefer_result<work_executor_t,
                                                       bae::outstanding_work_t::tracked_t>::type;

            [[no_unique_address]] work2_t work2_;
            [[no_unique_address]] Handler h_;

            fiber_data_handler_part(Handler h)
                : work2_{ba::prefer(ba::get_associated_executor(h),bae::outstanding_work.tracked)},
                  h_{std::move(h)}
            {}
        };

        template<typename Executor,typename Handler>
        struct fiber_data : fiber_data_base,
                            fiber_data_executor_part<Executor>,
                            fiber_data_handler_part<Handler> {};

        // We will store fiber_data at the top of the stack that is allocated for
        // the coroutine. This allows to avoid any extra allocations. This makes for
        // an interesting case, where a fiber object owns its stack, but is itself
        // stored in it. As such, to properly destroy such construct we need
        // to just invoke the destructor of fiber_data: it will destroy all subobjects
        // in reverse declaration order, of which fiber is last. It will then deallocate
        // the stack with the structure itself.
        // To automate ownership of such data structure we use a unique_ptr with
        // a custom deleter that only invokes destructor of the object pointed to.

        constexpr inline auto destroy_at_f = [](auto p){
            // destroy_at makes explicit destructor call of the base type
            // of the pointer provided, which usually is easier to type than
            // an explicit destructor call.
            std::destroy_at(p);
        };

        template<typename T>
        using destroy_only_unique_ptr = std::unique_ptr<T,decltype(destroy_at_f)>;
    }

    // yield_context works as a completion token used inside the coroutine with
    // async initiation functions to suspend the coroutine and make them return
    // result directly. It stores a pointer to our coroutine state and a pointer
    // to boost::system::error_code object to store the error code of operation,
    // if any, to avoid rethrowing it as boost::system::system_error.
    // The object to store error code is set with operator[] as with boost::asio::spawn.

    template<typename Executor,typename Handler>
    struct yield_context
    {
        detail::fiber_data<Executor,Handler>* fd_;
        boost::system::error_code* ec_ = nullptr;

        yield_context operator[](boost::system::error_code& ec) noexcept
        {
            return {fd_,&ec};
        }
    };

    template<typename Executor,typename Coroutine,typename StackAlloc =
    // Type of default stack allocator defaults to what boost::context::fiber does.
#if defined(BOOST_USE_SEGMENTED_STACKS)
        boost::context::segmented_stack,
#else
        boost::context::fixedsize_stack,
#endif
    // Latest CompletionToken protocol (see P1943R0) uses concepts to constrain token
    // types to those that satisfy CompletionToken requirements with handler of specified
    // signature. There can also be a default completion token type provided by the executor,
    // which we default to to allow it to not be specified in that case.
        ba::completion_token_for<void (std::exception_ptr)> CompletionToken =
            ba::default_completion_token_t<Executor>>
    auto spawn(Executor ex,Coroutine c,StackAlloc&& stack_alloc = {},CompletionToken&& token = {})
    {
        // Latest completion token protocol specifies async initiation function in terms
        // of call to async_initiate specialized by CompletionToken type and handler signature.
        // It must be passed the actual Initiation function and its arguments, of which
        // the first must be the completion token that is transformed to completion handler
        // upon call, whenever it is made by the concrete async_result specialization.
        // We're capturing all arguments we need instead of passing them through async_initiate
        // (except the completion token that needs transformation).
        // async_initiate will invoke initiate on the proper specialization of async_result
        // type or emulate it for the old variant of the protocol.
        return ba::async_initiate<CompletionToken,void (std::exception_ptr)>(
                [ex=std::move(ex),c=std::move(c),stack_alloc=std::move(stack_alloc)]
                (auto handler) mutable {
            using fiber_data_t = detail::fiber_data<Executor,decltype(handler)>;
            // Allocate the stack using provided stack allocator.
            auto stack_ctx = stack_alloc.allocate();
            // Stack groes downwards, and stack_ctx.sp is the initial stack pointer
            // value, that is, the address of byte one after the allocated stack.
            auto top = static_cast<char*>(stack_ctx.sp);
            // Calculate the topmost possible position of fiber_data on stack.
            // We subtract required size from sp and round it down to the alignment
            // of the structure.
            auto fd = reinterpret_cast<fiber_data_t*>(
                reinterpret_cast<std::uintptr_t>(top-sizeof(fiber_data_t))
                    &~(alignof(fiber_data_t)-1));
            // Since we've allocated and modified stack already, we
            // use an overload of fiber constructor that takes stack
            // as a preallocated structure with the new sp, size and original
            // stack context that has other platform-dependent fields.
            // We also gift it our stack allocator to own.
            // The last argument is the function to execute on new context.
            // Some implementations of fiber might throw, but in that case
            // they've already taken ownership of the stack and its allocator
            // and properly deallocate it before emitting the exception,
            // so there's no need to guard against this.
            new (fd) fiber_data_t{{boost::context::fiber{std::allocator_arg,
                boost::context::preallocated{
                    fd,
                    stack_ctx.size-size_t(top-reinterpret_cast<char*>(fd)),
                    stack_ctx},
                std::move(stack_alloc),
                [fd,c=std::move(c)](boost::context::fiber&& f){
                    // This is the first function that is executed on coroutine stack.
                    // First we store the original call context to fiber_data.
                    fd->f_ = std::move(f);
                    // All the actions after coroutine call, whether exceptional or not,
                    // will be handled by the destructor of this object.
                    struct finalize_t
                    {
                        fiber_data_t* fd_;
                        std::exception_ptr e_;
                        bool unwinding_ = false;

                        ~finalize_t() noexcept(false)
                        {
                            // We get here when the coroutine ends in some way.
                            // If we've noted before that this is a forced unwind,
                            // just destroy ourselves. This doesn't through exceptions
                            // which needed for the destructor to work properly during
                            // stack unwinding.
                            if(unwinding_){
                                std::destroy_at(fd_);
                                return;
                            }
                            // Otherwise the coroutine finished itself. We move the handler
                            // and its executor onto stack before destroying our state.
                            auto h = std::move(fd_->h_);
                            auto work2 = std::move(fd_->work2_);
                            std::destroy_at(fd_);
                            // And then we switch to the executor of the final handler.
                            // execute might throw, so we have to mark this destructor as
                            // throwing (which destructors are not by default), but
                            // we've made sure above we're not unwinding in this case.
                            bae::execute(ba::get_associated_executor(h),
                                    [h=std::move(h),e=std::move(e_),work2=std::move(work2)]
                                    () mutable {
                                // Before invoking the completion handler we stop the work
                                // by moving the saved outstaning_work.tracked executor into
                                // temporary which gets immediately destroyed.
                                decltype(work2){std::move(work2)};
                                h(std::move(e));
                            });
                        }
                    } finalizer{fd};
                    try{
                        // Try to execute the coroutine, providing it with
                        // a yield_context to suspend itself in async calls.
                        c(yield_context<Executor,decltype(handler)>{fd});
                    }
                    catch(const boost::context::detail::forced_unwind&){
                        // forced_unwind is a special exception used to unwind
                        // the stack of the suspended coroutine if the fiber
                        // referring to it is destroyed. We must not catch it
                        // for Boost.Context to work correctly, but we notify
                        // our finalizer that this is not a normal return.
                        finalizer.unwinding_ = true;
                        throw;
                    }
                    catch(...){
                        // All other exceptions are caught and stored for the completion handler.
                        finalizer.e_ = std::current_exception();
                    }
                    // If we're not being destroyed externally, we get here and
                    // return original context to return to.
                    // For forced unwinds the same information is carried by the
                    // forced_unwind exception instance itself.
                    return std::move(fd->f_);
                }
            }},{ex},{std::move(handler)}};
            // After coroutine creation is done, we execute its startup on
            // its executor. To prevent its leak if the handler is destroyed
            // without execution, we use a special smart pointer we prepared before.
            bae::execute(ex,[fdo=detail::destroy_only_unique_ptr<fiber_data_t>{fd}]() mutable {
                auto f = std::move(fdo->f_).resume();
                // Since fiber value that identifies the suspended coroutine is only
                // available to the caller, we need to store it back to fiber_data here,
                // but only if it's not already done.
                if(f)
                    fdo->f_ = std::move(f);
                else
                    // If it has already finished, that means it's already deallocated
                    // itself and fdo owns a dangling pointer. To prevent double-free
                    // we release it here into the void.
                    static_cast<void>(fdo.release());
            });
        },token);
    }

    namespace detail
    {
        // This is a helper class to transform a tuple of result args into proper
        // return value of the asynchronous initiation function in the coroutine,
        // given the (possibly null) pointer to the error_code instance.
        struct unwrap_coro_return_tuple
        {
            boost::system::error_code* ec_;

            // If result args start with an error_code, handle it.
            template<typename... Args>
            auto operator()(boost::system::error_code ec,Args... args)
            {
                // If we have where to store error_code, do it.
                if(ec_)
                    *ec_ = ec;
                // Otherwise if there is error, we throw it as exception.
                else if(ec)
                    throw boost::system::system_error{ec};
                // The rest of the arguments are processed as usual.
                return unpack_args(std::move(args)...);
            }

            // If results don't start with an error code, process all of them.
            template<typename... Args>
            auto operator()(Args... args)
            {
                return unpack_args(std::move(args)...);
            }

            // An empty result list is returned as a void value.
            static void unpack_args() noexcept {}

            // A single result is returned as is.
            template<typename Arg>
            static Arg unpack_args(Arg x)
            {
                return std::move(x);
            }

            // Multiple results are returned as a tuple.
            template<typename... Args>
            static std::tuple<Args...> unpack_args(Args... args)
            {
                return {std::move(args)...};
            }
        };
    }    
}

template<typename Executor,typename Handler,typename... Args>
class ce::ba::async_result<ce::yield_context<Executor,Handler>,void (Args...)>
{
public:
    // Latest completion token protocol is simpler and more feature-rich.
    // It requires a specialization of async_result for the CompletionToken type
    // and handler signature to provide a single static function called initiate.
    // It takes an Initiation function that actually starts the asynchronous operation
    // and its arguments, of which the first one is always a CompletionToken.
    // Its job is to arrange for the Initiation to be called with its arguments
    // whenever is appropriate (immediately or delayed) and transform its first
    // argument from CompletionToken into completion handler. The return of the
    // initiate function becomes the return value of the asynchronous initiation function.
    // We will only implement this new variant with which all ASIO operations
    // are compatible.
    template<typename Initiation,typename... InitArgs>
    static auto initiate(Initiation initiation,ce::yield_context<Executor,Handler> yc,
                         InitArgs&&... init_args)
    {
        std::tuple<Args...>* result_ptr;
        // We suspend the current coroutine by calling resume_with on a fiber
        // that currently refers to original stack context of the executor of our
        // coroutine. Unlike resume, it doesn't transfer the control back directly,
        // but makes the provided function execute on top of that stack context, as
        // if it was called originally from resume() of our coroutine at that site.
        yc.fd_->f_ = std::move(yc.fd_->f_).resume_with([&](boost::context::fiber&& f){
            // We're on the original stack frame and our coroutine is suspended as f,
            // store it into the state.
            yc.fd_->f_ = std::move(f);
            // Initiate the asynchronous operation and provide it a handler
            // that has an associated executor of our coroutine with a continuation
            // relation property preferred, since that will give control back
            // to the coroutine, continuing its execution. We store the fiber_data
            // pointer from yield_context in a smart pointer to own it in case
            // we'll be destroyed without invoking.
            std::forward<Initiation>(initiation)(ce::ba::bind_executor(
                ce::ba::prefer(yc.fd_->work_,ce::bae::relationship.continuation),
                [&,fdo=ce::detail::destroy_only_unique_ptr<
                        ce::detail::fiber_data<Executor,Handler>>{yc.fd_}]
                        (Args... args) mutable {
                    // Async operation is done and args hold the result.
                    // Store it in a single tuple.
                    std::tuple<Args...> result{std::move(args)...};
                    // Save the addres of this tuple in the variable on the coroutine stack.
                    // It was suspended all this time, so capturing a reference to
                    // this variable is correct.
                    result_ptr = &result;
                    // We release ownership of fiber_data so that it doesn't get freed
                    // on return from this function and resume the coroutine.
                    std::move(fdo.release()->f_).resume();
                }),
                std::forward<InitArgs>(init_args)...
            );
            // After initiation operation, we return the null fiber to switch to.
            // It's ok to not switch anywhere, since we're on the original thread stack,
            // so we continue execution from the point where we were last switched to from.
            return boost::context::fiber{};
        });
        // We're back from the finished asynchronous operation, on the proper executor,
        // the original context of our caller already is stored in fiber_data, and it's suspended
        // with the tuple of result args on stack, which we have the pointer to.
        // Move results from that tuple and transform it for return from async initiation
        // function in the coroutine.
        return std::apply(ce::detail::unwrap_coro_return_tuple{yc.ec_},std::move(*result_ptr));
    }
};

#endif
