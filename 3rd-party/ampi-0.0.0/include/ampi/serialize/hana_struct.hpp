#ifndef UUID_13F2D7C4_24A6_4B8C_AF92_A2BA749D05E3
#define UUID_13F2D7C4_24A6_4B8C_AF92_A2BA749D05E3

#include <ampi/serialize/serializable.hpp>

#include <boost/hana/accessors.hpp>
#include <boost/hana/size.hpp>

#include <bitset>
#include <exception>

namespace ampi
{
    namespace detail
    {
        template<typename T>
        struct accessor_result_impl
        {
            using type = std::remove_cvref_t<T>;
        };

        template<typename T>
            requires requires { typename T::proxied_value_type; }
        struct accessor_result_impl<T>
        {
            using type = typename T::proxied_value_type;
        };
    }

    template<typename T>
    struct accessor_result : detail::accessor_result_impl<T> {};

    template<typename T>
    using accessor_result_t = typename accessor_result<T>::type;

    namespace detail
    {
        template<typename T>
            requires boost::hana::Struct<T>::value
        constexpr bool serializable_hana_struct() noexcept
        {
            auto a = boost::hana::accessors<T>();
            constexpr size_t n = boost::hana::size(a);
            if(n>0xffffffff)
                return false;
            return []<size_t... Indices>(std::index_sequence<Indices...>){
                return (...&&serializable<accessor_result_t<
                    decltype(boost::hana::second(a[boost::hana::size_c<Indices>])
                    (std::declval<T>()))>>);
            }(std::make_index_sequence<n>{});
        }

        template<typename T>
        concept deserializable_accessor = deserializable<accessor_result_t<T>> && requires(T x) {
            x = std::declval<accessor_result_t<T>>();
        };

        template<typename T>
            requires boost::hana::Struct<T>::value
        constexpr bool deserializable_hana_struct() noexcept
        {
            auto a = boost::hana::accessors<T>();
            constexpr size_t n = boost::hana::size(a);
            if(n>0xffffffff)
                return false;
            return []<size_t... Indices>(std::index_sequence<Indices...>){
                return (...&&deserializable_accessor<
                    decltype(boost::hana::second(a[boost::hana::size_c<Indices>])
                    (std::declval<T>()))>);
            }(std::make_index_sequence<n>{});
        }
    }

    template<typename T>
    concept serializable_hana_struct = detail::serializable_hana_struct<T>();

    template<typename T>
    concept deserializable_hana_struct = detail::serializable_hana_struct<T>();

    template<serializable_hana_struct T>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return []<size_t... Indices>(std::index_sequence<Indices...>){
            return [](stack_executor ex,const T& x) -> delegating_event_generator {
                auto a = boost::hana::accessors<T>();
                co_yield map_header{detail::check_size<T>(boost::hana::size(a).value)};
                auto get_name = [&](auto i){
                    return piecewise_string{std::string_view{boost::hana::first(a[i]).c_str()}};
                };
                auto get_ses = [&](auto i) -> decltype(auto) {
                    decltype(auto) v = boost::hana::second(a[i])(x);
                    return serial_event_source(type_tag<accessor_result_t<decltype(v)>>)(ex,v);
                };
                (...,(co_yield get_name(boost::hana::size_c<Indices>),
                      co_yield get_ses(boost::hana::size_c<Indices>)));
            };
        }(std::make_index_sequence<boost::hana::size(boost::hana::accessors<T>())>{});
    }

    template<deserializable_hana_struct T>
    auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<T>) noexcept
    {
        return []<size_t... Indices>(std::index_sequence<Indices...>){
            return [](stack_executor ex,event_source auto& source,T& x) -> event_consumer {
                auto a = boost::hana::accessors<T>();
                event e = expect_event<T>(co_await source,{object_kind::map});
                uint32_t n = e.get_if<map_header>()->size;
                constexpr size_t s = boost::hana::size(a);
                if(n>s)
                    throw structure_error{structure_error::reason_t::out_of_range,
                                          boost::typeindex::type_id<T>(),
                                          {object_kind::sequence},std::move(e)};
                std::bitset<s> seen;
                auto matched_name = [&](auto i,const piecewise_string& name){
                    if(name!=std::string_view{boost::hana::first(a[i]).c_str()})
                        return false;
                    if(seen[i])
                        throw structure_error{structure_error::reason_t::duplicate_key,
                                              boost::typeindex::type_id<T>()};
                    seen[i] = true;
                    return true;
                };
                auto get_ses = [&](auto i){
                    decltype(auto) v = boost::hana::second(a[i])(x);
                    using result_t = accessor_result_t<decltype(v)>;
                    if constexpr(std::is_same_v<std::remove_cvref_t<decltype(v)>,result_t>)
                        return serial_event_sink(type_tag<result_t>)(ex,source,v);
                    else{
                        auto ses = serial_event_sink(type_tag<result_t>);
                        using sink_t = decltype(ses(ex,source,std::declval<result_t&>()));
                        using awaiter_t = decltype(std::declval<sink_t>().operator co_await());
                        using wrapper_t = awaiter_wrapper<awaiter_t>;
                        struct setter_base
                        {
                            result_t x_;
                            sink_t sink_;

                            setter_base(decltype(ses) ses_,stack_executor ex_,
                                        decltype(source) source_)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
                                : sink_{ses_(ex_,source_,x_)}
#pragma clang diagnostic pop
                            {}
                        };
                        struct setter : setter_base,wrapper_t
                        {
                            decltype(v) v_;
                            int uc_ = std::uncaught_exceptions();

                            setter(decltype(ses) ses_,stack_executor ex_,decltype(source) source_,
                                   decltype(v) v_)
                                : setter_base{std::move(ses_),ex_,source_},
                                  wrapper_t{std::move(this->sink_).operator co_await()},
                                  v_{v_}
                            {}

                            ~setter() noexcept(noexcept(v_ = std::declval<result_t>()))
                            {
                                if(uc_==std::uncaught_exceptions())
                                    v_ = std::move(this->x_);
                            }
                        };
                        return setter{std::move(ses),ex,source,std::move(v)};
                    }
                };
                for(uint32_t i=0;i<n;++i){
                    auto e = expect_event<T>(co_await source,{object_kind::string});
                    auto name = *e.template get_if<piecewise_string>();
                    if(!(...||(matched_name(boost::hana::size_c<Indices>,name)&&
                           (co_await get_ses(boost::hana::size_c<Indices>),true))))
                        throw structure_error{structure_error::reason_t::unknown_key,
                                              boost::typeindex::type_id<T>(),{},std::move(e)};
                }
            };
        }(std::make_index_sequence<boost::hana::size(boost::hana::accessors<T>())>{});
    }
}

#endif
