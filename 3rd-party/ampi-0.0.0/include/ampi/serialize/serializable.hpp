#ifndef UUID_E8B5A410_4A58_44DC_A61D_DECFB7448433
#define UUID_E8B5A410_4A58_44DC_A61D_DECFB7448433

#include <ampi/coro/coroutine.hpp>
#include <ampi/exception.hpp>
#include <ampi/export.h>
#include <ampi/msgpack.hpp>
#include <ampi/pmr/segmented_stack_resource.hpp>
#include <ampi/utils/tag_invoke.hpp>

#include <boost/asio/execution/allocator.hpp>
#include <boost/asio/require.hpp>
#include <boost/container/pmr/global_resource.hpp>
#include <boost/container/pmr/polymorphic_allocator.hpp>
#include <boost/type_index.hpp>

#include <limits>
#include <tuple>

namespace ampi
{
    template<typename T>
    struct type_tag_t
    {
        using type = T;
    };
    
    template<typename T>
    constexpr inline type_tag_t<T> type_tag;

    using stack_executor = boost::asio::require_result<
        boost::asio::system_executor,
        boost::asio::execution::allocator_t<boost::container::pmr::polymorphic_allocator<std::byte>>
    >::type;
    
    using event_generator = generator<event_result,stack_executor>;
    using noexcept_event_generator = noexcept_generator<event_result,stack_executor>;
    using delegating_event_generator = delegating_generator<event_result,stack_executor>;
    using event_consumer = coroutine<void,stack_executor>;

    class AMPI_EXPORT object_kind_set
    {
        struct any_tag {};
    public:
        const static object_kind_set any;

        template<typename... Kinds>
            requires (...&&std::is_same_v<Kinds,object_kind>)
        constexpr object_kind_set(Kinds... kinds) noexcept
        {
            (...,(set_ |= 1u<<static_cast<size_t>(kinds)));
        }

        constexpr explicit operator bool() const noexcept { return set_; }

        friend constexpr object_kind_set operator&(object_kind_set oks1,object_kind_set oks2)
        {
            object_kind_set result;
            result.set_ = oks1.set_&oks2.set_;
            return result;
        }

        friend std::ostream& operator<<(std::ostream& stream,object_kind_set oks);
    private:
        unsigned set_;

        static_assert(static_cast<size_t>(object_kind::kind_count_)<sizeof(set_)*8);

        constexpr object_kind_set(any_tag) noexcept
            : set_{(1u<<static_cast<size_t>(object_kind::kind_count_))-1u}
        {}
    };

    const inline object_kind_set object_kind_set::any = object_kind_set::any_tag{};

    class AMPI_EXPORT structure_error final : public exception
    {
    public:
        enum struct reason_t
        {
            unexpected_event,
            out_of_range,
            duplicate_key,
            unknown_key
        };

        structure_error(reason_t reason,boost::typeindex::type_index object_type,
                        object_kind_set expected_kinds = {},optional<event> event = {}) noexcept
            : reason_{reason},
              object_type_{object_type},
              expected_kinds_{expected_kinds},
              event_{std::move(event)}
        {}

        reason_t reason() const noexcept { return reason_; }
        boost::typeindex::type_index object_type() const noexcept { return object_type_; }
        object_kind_set expected_kinds() const noexcept { return expected_kinds_; }
        const optional<event>& event() const noexcept { return event_; }

        const char* what() const noexcept override;
    private:
        reason_t reason_;
        boost::typeindex::type_index object_type_;
        object_kind_set expected_kinds_;
        optional<class event> event_;
        mutable std::shared_ptr<std::string> what_;
    };

    template<typename T>
    event expect_event(event_result* result,object_kind_set expected = object_kind_set::any)
    {
        if(!result)
            throw structure_error{structure_error::reason_t::unexpected_event,
                                  boost::typeindex::type_id<T>(),expected};
        auto e = std::move(result->value());
        if(!(e.kind()&expected))
            throw structure_error{structure_error::reason_t::unexpected_event,
                                  boost::typeindex::type_id<T>(),expected,std::move(e)};
        return e;
    }

    namespace detail
    {
        template<typename T,typename U>
        concept serial_event_source_factory = requires(T sesf,stack_executor ex,const U& x){
            { sesf(ex,x) } -> generator_yielding_exactly<event_result>;
        };

        template<typename T,typename U>
        concept serial_event_sink_factory = requires(T sesf,stack_executor ex,
                                                        async_generator<event_result>& es,U& x) {
            { sesf(ex,es,x) } -> coroutine_returning<void>;
        };
    }

    constexpr inline struct serial_event_source_fn
    {
        template<typename T>
            requires detail::serial_event_source_factory<
                tag_invoke_result_t<serial_event_source_fn,type_tag_t<T>>,
                T
            >
        auto operator()(type_tag_t<T> tt) const noexcept
        {
            return ampi::tag_invoke(*this,tt);
        }
    } serial_event_source;

    constexpr inline struct serial_event_sink_fn
    {
        template<typename T>
            requires detail::serial_event_sink_factory<
                tag_invoke_result_t<serial_event_sink_fn,type_tag_t<T>>,
                T
            >
        auto operator()(type_tag_t<T> tt) const noexcept
        {
            return ampi::tag_invoke(*this,tt);
        }
    } serial_event_sink;

    template<typename T>
    concept serializable = is_tag_invocable_v<tag_t<serial_event_source>,type_tag_t<T>>;

    template<typename T>
    concept deserializable = is_tag_invocable_v<tag_t<serial_event_sink>,type_tag_t<T>>;

    template<typename T>
    concept tuple_like = requires {
        { std::tuple_size<T>::value } -> std::same_as<const size_t>;
    };

    template<typename T>
    struct container_value_type;

    template<typename T>
        requires requires { typename T::value_type; }
    struct container_value_type<T>
    {
        using type = typename T::value_type;
    };

    template<typename T>
        requires std::is_array_v<T>
    struct container_value_type<T>
    {
        using type = std::remove_extent_t<T>;
    };

    template<typename T>
    using container_value_type_t = typename container_value_type<T>::type;

    template<typename T>
    concept container_like = requires(T c) {
        { *std::begin(c) } -> std::convertible_to<typename container_value_type<T>::type>;
        std::end(c);
        { std::size(c) } -> integral;
    };

    namespace detail
    {
        template<tuple_like T>
            requires (!container_like<T>)
        constexpr bool serializable_tuple() noexcept
        {
            constexpr auto n = std::tuple_size_v<T>;
            if(n>0xffffffff)
                return false;
            return []<size_t... Indices>(std::index_sequence<Indices...>){
                return (...&&serializable<std::tuple_element_t<Indices,T>>);
            }(std::make_index_sequence<n>{});
        }

        template<tuple_like T>
            requires (!container_like<T>)
        constexpr bool deserializable_tuple() noexcept
        {
            constexpr auto n = std::tuple_size_v<T>;
            if(n>0xffffffff)
                return false;
            return []<size_t... Indices>(std::index_sequence<Indices...>){
                return (...&&deserializable<std::tuple_element_t<Indices,T>>);
            }(std::make_index_sequence<n>{});
        }
    }

    template<typename T>
    concept serializable_tuple_like = detail::serializable_tuple<T>();

    template<typename T>
    concept deserializable_tuple_like = detail::deserializable_tuple<T>();

    template<typename T>
    concept overwritable_container_like = container_like<T> && requires(T x) {
        { *x.begin() } -> std::convertible_to<typename T::value_type&>;
    };

    template<typename T>
    concept resizeable_container_like = overwritable_container_like<T> && requires(T x) {
        x.resize(std::size(x));
    };

    template<typename T>
    concept emplace_back_container_like = container_like<T> && requires(T x) {
        x.clear();
        { x.emplace_back() } -> std::convertible_to<typename T::value_type&>;
    };

    template<typename T>
    concept set_container_like = container_like<T> && requires(T x) {
        x.clear();
        x.emplace(std::declval<typename T::value_type>());
    };

    template<typename T>
    concept map_like = container_like<T> && requires(T m,typename T::value_type v) {
        typename std::enable_if<std::tuple_size<typename T::value_type>::value==2>::type;
    };

    template<typename T>
    using map_like_key_type = std::tuple_element_t<0,typename T::value_type>;

    template<typename T>
    using map_like_mapped_type = std::tuple_element_t<1,typename T::value_type>;

    template<typename T>
    concept emplaceable_map_like = map_like<T> && requires(T m,
            typename std::tuple_element_t<0,T> k,typename std::tuple_element_t<1,T> v) {
        m.emplace(std::move(k),std::move(v));
    };

    template<typename T>
    concept unique_associative_container_like = container_like<T> && requires(T m,
            typename T::value_type v) {
        { get<1>(m.emplace(std::move(v))) } -> std::convertible_to<bool>;
    };

    namespace detail
    {
        template<typename T>
        uint32_t check_size(integral auto n)
        {
            if(std::cmp_greater(n,0xffffffff))
                throw structure_error{structure_error::reason_t::out_of_range,
                                      boost::typeindex::type_id<T>()};
            return uint32_t(n);
        }

        template<typename T,typename Res>
        void check_unique_key(Res res)
        {
            if constexpr(unique_associative_container_like<T>)
                if(!get<1>(res))
                    throw structure_error{structure_error::reason_t::duplicate_key,
                                          boost::typeindex::type_id<T>()};
        }
    }

    template<typename T>
        requires std::is_same_v<T,std::nullptr_t>||
                 std::is_same_v<T,bool>||
                 std::is_same_v<T,float>||
                 std::is_same_v<T,double>||
                 std::is_same_v<T,timestamp_t>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return [](stack_executor,T x) -> noexcept_event_generator {
            co_yield x;
        };
    }

    template<integral T>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return [](stack_executor,T x) -> noexcept_event_generator {
            if(x<0)
                co_yield int64_t(x);
            else
                co_yield uint64_t(x);
        };
    }

    template<std::convertible_to<string_view> T>
        requires (!std::is_same_v<T,std::nullptr_t>)
    inline auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return [](stack_executor,string_view x) -> noexcept_event_generator {
            co_yield piecewise_string{x};
        };
    }

    template<std::convertible_to<binary_cview_t> T>
    inline auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return [](stack_executor,binary_cview_t x) -> noexcept_event_generator {
            co_yield piecewise_data{x};
        };
    }
    
    template<serializable T>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<optional<T>>) noexcept
    {
        return [](stack_executor ex,const optional<T>& x) -> delegating_event_generator {
            bool has_data = bool(x);
            co_yield has_data;
            if(has_data)
                co_yield tail_yield_to(serial_event_source(type_tag<T>)(ex,*x));
        };
    }

    template<serializable... Ts>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<variant<Ts...>>) noexcept
    {
        return []<size_t... Indices>(std::index_sequence<Indices...>){
            return [](stack_executor ex,const variant<Ts...>& x) -> delegating_event_generator {
                auto i = x.index();
                co_yield i;
                static_cast<void>((...||(i==Indices&&(co_yield tail_yield_to(
                    serial_event_source(type_tag<Ts>)(ex,*get_if<Indices>(&x))),true))));
            };
        }(std::index_sequence_for<Ts...>{});
    }

    template<serializable_tuple_like T>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return []<size_t... Indices>(std::index_sequence<Indices...>){
            return [](stack_executor ex,const T& x) -> delegating_event_generator {
                co_yield sequence_header{uint32_t(std::tuple_size_v<T>)};
                (...,(co_yield serial_event_source(type_tag<std::tuple_element_t<Indices,T>>)
                    (ex,get<Indices>(x))));
            };
        }(std::make_index_sequence<std::tuple_size_v<T>>{});
    }

    template<typename T>
    concept unique_map_like = map_like<T> && requires(T x,typename T::value_type kv) {
        { get<1>(x.emplace(std::declval<std::remove_const_t<map_like_key_type<T>>>(),
                           std::declval<map_like_mapped_type<T>>())) }
            -> std::convertible_to<bool>;
    };

    template<container_like T>
        requires (!std::convertible_to<T,string_view>)&&
                 (!std::convertible_to<T,binary_cview_t>)&&
                 (!map_like<T>)&&
                 serializable<typename T::value_type>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return [](stack_executor ex,const T& x) -> delegating_event_generator {
            co_yield sequence_header{detail::check_size<T>(std::size(x))};
            auto ses = serial_event_source(type_tag<container_value_type_t<T>>);
            for(auto& e:x)
                co_yield ses(ex,e);
        };
    }

    template<map_like T>
        requires serializable<std::remove_const_t<map_like_key_type<T>>>&&
                 serializable<map_like_mapped_type<T>>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return [](stack_executor ex,const T& x) -> delegating_event_generator {
            co_yield map_header{detail::check_size<T>(std::size(x))};
            auto sesk = serial_event_source(type_tag<std::remove_const_t<map_like_key_type<T>>>);
            auto sesv = serial_event_source(type_tag<map_like_mapped_type<T>>);
            for(auto& [k,v]:x){
                co_yield sesk(ex,k);
                co_yield sesv(ex,v);
            }
        };
    }

    inline auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<std::nullptr_t>) noexcept
    {
        return [](stack_executor,event_source auto& source,std::nullptr_t& /*x*/) -> event_consumer {
            expect_event<std::nullptr_t>(co_await source,{object_kind::null});
        };
    }

    inline auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<bool>) noexcept
    {
        return [](stack_executor,event_source auto& source,bool& x) -> event_consumer {
            x = *expect_event<bool>(co_await source,{object_kind::bool_}).template get_if<bool>();
        };
    }

    template<integral T>
    auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<T>) noexcept
    {
        return [](stack_executor,event_source auto& source,T& x) -> event_consumer {
            event e = expect_event<T>(co_await source,
                                      {object_kind::unsigned_int,object_kind::signed_int});
            auto throw_out_of_range = [&]{
                throw structure_error{structure_error::reason_t::out_of_range,
                                    boost::typeindex::type_id<T>(),{},std::move(e)};
            };
            if(e.kind()==object_kind::unsigned_int){
                uint64_t v = *e.get_if<uint64_t>();
                if(std::cmp_greater(v,std::numeric_limits<T>::max()))
                    throw_out_of_range();
                x = T(v);
            }else{
                int64_t v = *e.get_if<int64_t>();
                if(std::cmp_greater(v,std::numeric_limits<T>::max())||
                        std::cmp_less(v,std::numeric_limits<T>::lowest()))
                    throw_out_of_range();
                x = T(v);
            }
        };
    }

    inline auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<float>) noexcept
    {
        return [](stack_executor,event_source auto& source,float& x) -> event_consumer {
            x = *expect_event<float>(co_await source,{object_kind::float_}).template get_if<float>();
        };
    }

    inline auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<double>) noexcept
    {
        return [](stack_executor,event_source auto& source,double& x) -> event_consumer {
            x = *expect_event<double>(co_await source,{object_kind::double_}).template get_if<double>();
        };
    }

    inline auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<timestamp_t>) noexcept
    {
        return [](stack_executor,event_source auto& source,timestamp_t& x) -> event_consumer {
            x = *expect_event<timestamp_t>(co_await source,{object_kind::timestamp}).
                    template get_if<timestamp_t>();
        };
    }

    template<typename Allocator>
    inline auto tag_invoke(tag_t<serial_event_sink>,
                           type_tag_t<std::basic_string<char,std::char_traits<char>,Allocator>>) noexcept
    {
        return [](stack_executor,event_source auto& source,auto& x) -> event_consumer {
            auto ps = *expect_event<decltype(x)>(co_await source,{object_kind::string}).
                          template get_if<piecewise_string>();
            if constexpr(requires{ ps.merge(x.get_allocator()); })
                x = std::move(ps).merge(x.get_allocator());
            else
                x = ps.merge();
        };
    }

    template<typename Allocator>
    inline auto tag_invoke(tag_t<serial_event_sink>,
                           type_tag_t<boost::container::vector<byte,Allocator>>) noexcept
    {
        return [](stack_executor,event_source auto& source,auto& x) -> event_consumer {
            auto ps = *expect_event<decltype(x)>(co_await source,{object_kind::binary}).
                          template get_if<piecewise_data>();
            if constexpr(requires{ ps.merge(x.get_allocator()); })
                x = std::move(ps).merge(x.get_allocator());
            else
                x = ps.merge();
        };
    }

    template<deserializable T>
    auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<optional<T>>) noexcept
    {
        return [](stack_executor ex,event_source auto& source,optional<T>& x) -> event_consumer {
            if(*expect_event<optional<T>>(co_await source,{object_kind::bool_}).
                    template get_if<bool>())
                co_await serial_event_sink(type_tag<T>)(ex,source,x.emplace());
            else
                x.reset();
        };
    }

    template<deserializable... Ts>
    auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<variant<Ts...>>) noexcept
    {
        return []<size_t... Indices>(std::index_sequence<Indices...>){
            return [](stack_executor ex,event_source auto& source,variant<Ts...>& x) -> event_consumer {
                event e = expect_event<variant<Ts...>>(co_await source,{object_kind::unsigned_int});
                uint64_t i = *e.get_if<uint64_t>();
                if(i>=sizeof...(Ts))
                    throw structure_error{structure_error::reason_t::out_of_range,
                                          boost::typeindex::type_id<variant<Ts...>>(),
                                          {object_kind::unsigned_int},std::move(e)};
                static_cast<void>((...||(i==Indices&&(co_await serial_event_sink(type_tag<Ts>)
                    (ex,source,x.template emplace<Indices>()),true))));
            };
        }(std::index_sequence_for<Ts...>{});
    }

    template<deserializable_tuple_like T>
    auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<T>) noexcept
    {
        constexpr auto n = std::tuple_size_v<T>;
        return []<size_t... Indices>(std::index_sequence<Indices...>){
            return [](stack_executor ex,event_source auto& source,T& x) -> event_consumer {
                event e = expect_event<T>(co_await source,{object_kind::sequence});
                if(e.get_if<sequence_header>()->size!=n)
                    throw structure_error{structure_error::reason_t::out_of_range,
                                          boost::typeindex::type_id<T>(),
                                          {object_kind::sequence},std::move(e)};
                (...,(co_await serial_event_sink(type_tag<std::tuple_element_t<Indices,T>>)
                    (ex,source,get<Indices>(x))));
            };
        }(std::make_index_sequence<n>{});
    }

    template<container_like T>
        requires (!map_like<T>)&&(
                      overwritable_container_like<T>||
                      resizeable_container_like<T>||
                      emplace_back_container_like<T>||
                      set_container_like<T>)
    auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<T>) noexcept
    {
        return [](stack_executor ex,event_source auto& source,T& x) -> event_consumer {
            event e = expect_event<T>(co_await source,{object_kind::sequence});
            uint32_t s = e.get_if<sequence_header>()->size;
            using size_type = decltype(std::size(x));
            auto throw_out_of_range = [&]{
                throw structure_error{structure_error::reason_t::out_of_range,
                                      boost::typeindex::type_id<T>(),
                                      {object_kind::unsigned_int},std::move(e)};
            };
            if(std::cmp_greater(s,std::numeric_limits<size_type>::max()))
                throw_out_of_range();
            auto n = size_type(s);
            auto ses = serial_event_sink(type_tag<container_value_type_t<T>>);
            if constexpr(resizeable_container_like<T>||
                    (!emplace_back_container_like<T>&&!set_container_like<T>)){
                if constexpr(resizeable_container_like<T>)
                    x.resize(n);
                else if(n!=x.size())
                    throw_out_of_range();
                for(auto& e:x)
                    co_await ses(ex,source,e);
            }else{
                x.clear();
                for(size_type i=0;i<n;++i)
                    if constexpr(emplace_back_container_like<T>)
                        co_await ses(ex,source,x.emplace_back());
                    else{
                        typename T::value_type v;
                        co_await ses(ex,source,v);
                        detail::check_unique_key<T>(x.emplace(std::move(v)));
                    }
            }
        };
    }

    template<map_like T>
        requires deserializable<std::remove_const_t<map_like_key_type<T>>>&&
                 deserializable<map_like_mapped_type<T>>
    auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<T>) noexcept
    {
        return [](stack_executor ex,event_source auto& source,T& x) -> event_consumer {
            event e = expect_event<T>(co_await source,{object_kind::map});
            uint32_t s = e.get_if<map_header>()->size;
            using size_type = decltype(std::size(x));
            if(std::cmp_greater(s,std::numeric_limits<size_type>::max()))
                throw structure_error{structure_error::reason_t::out_of_range,
                                      boost::typeindex::type_id<T>(),
                                      {object_kind::unsigned_int},std::move(e)};
            auto n = size_type(s);
            using key_type = std::remove_const_t<map_like_key_type<T>>;
            using mapped_type = map_like_mapped_type<T>;
            auto sesk = serial_event_sink(type_tag<key_type>);
            auto sesv = serial_event_sink(type_tag<mapped_type>);
            x.clear();
            for(size_type i=0;i<n;++i){
                key_type k;
                co_await sesk(ex,source,k);
                mapped_type v;
                co_await sesv(ex,source,v);
                detail::check_unique_key<T>(x.emplace(std::move(k),std::move(v)));
            }
        };
    }
}

#endif
