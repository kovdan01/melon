#ifndef UUID_19B40254_9008_410C_8A33_95B207448929
#define UUID_19B40254_9008_410C_8A33_95B207448929

#include <ampi/serialize/hana_struct.hpp>

#include <boost/pfr/core.hpp>
#include <boost/pfr/tuple_size.hpp>

namespace ampi
{
    template<typename T>
    concept pfr_tuple = std::is_aggregate_v<T>&&
                        !container_like<T>&&
                        !boost::hana::Struct<T>::value;

    namespace detail
    {
        template<pfr_tuple T>
        constexpr bool serializable_pfr_tuple() noexcept
        {
            constexpr auto n = boost::pfr::tuple_size_v<T>;
            if(n>0xffffffff)
                return false;
            return []<size_t... Indices>(std::index_sequence<Indices...>){
                return (...&&serializable<boost::pfr::tuple_element_t<Indices,T>>);
            }(std::make_index_sequence<n>{});
        }

        template<pfr_tuple T>
        constexpr bool deserializable_pfr_tuple() noexcept
        {
            constexpr auto n = boost::pfr::tuple_size_v<T>;
            if(n>0xffffffff)
                return false;
            return []<size_t... Indices>(std::index_sequence<Indices...>){
                return (...&&deserializable<boost::pfr::tuple_element_t<Indices,T>>);
            }(std::make_index_sequence<n>{});
        }
    }

    template<typename T>
    concept serializable_pfr_tuple = detail::serializable_pfr_tuple<T>();

    template<typename T>
    concept deserializable_pfr_tuple = detail::deserializable_pfr_tuple<T>();

    template<serializable_pfr_tuple T>
    auto tag_invoke(tag_t<serial_event_source>,type_tag_t<T>) noexcept
    {
        return []<size_t... Indices>(std::index_sequence<Indices...>){
            return [](stack_executor ex,const T& x) -> delegating_event_generator {
                co_yield sequence_header{uint32_t(boost::pfr::tuple_size_v<T>)};
                (...,(co_yield serial_event_source(type_tag<boost::pfr::tuple_element_t<Indices,T>>)
                    (ex,boost::pfr::get<Indices>(x))));
            };
        }(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});
    }

    template<deserializable_pfr_tuple T>
    auto tag_invoke(tag_t<serial_event_sink>,type_tag_t<T>) noexcept
    {
        constexpr auto n = boost::pfr::tuple_size_v<T>;
        return []<size_t... Indices>(std::index_sequence<Indices...>){
            return [](stack_executor ex,event_source auto& source,T& x) -> event_consumer {
                event e = expect_event<T>(co_await source,{object_kind::sequence});
                if(e.get_if<sequence_header>()->size!=n)
                    throw structure_error{structure_error::reason_t::out_of_range,
                                          boost::typeindex::type_id<T>(),
                                          {object_kind::sequence},std::move(e)};
                (...,(co_await serial_event_sink(type_tag<boost::pfr::tuple_element_t<Indices,T>>)
                    (ex,source,boost::pfr::get<Indices>(x))));
            };
        }(std::make_index_sequence<n>{});
    }
}

#endif
