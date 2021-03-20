// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_E9A6C78D_E1CE_44CD_AB9F_10833086C33B
#define UUID_E9A6C78D_E1CE_44CD_AB9F_10833086C33B

#include <ampi/coro/coroutine.hpp>
#include <ampi/piecewise_view.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/outcome/boost_result.hpp>

#include <chrono>
#include <concepts>
#include <cstdint>
#include <limits>

namespace ampi
{
    static_assert(std::numeric_limits<float>::is_iec559&&sizeof(float)==4,
                  "float is not IEEE 754 single precision floating point type");
    static_assert(std::numeric_limits<double>::is_iec559&&sizeof(double)==8,
                  "double is not IEEE 754 double precision floating point type");

    enum struct object_kind : uint8_t
    {
        null,
        bool_,
        unsigned_int,
        // Only negative values, positive signed are passed as unsigned.
        signed_int,
        float_,
        double_,
        // sequence=map+1 is used in parsing 0x80-0x9f and 0xdc-0xdf
        map,
        sequence,
        // extension=binary+1 is used in parsing 0xc4-0xc9
        // binary-string is a range that is read in chunks is used in read_bytes
        binary,
        extension,
        string,
        timestamp,
        kind_count_
    };

    AMPI_EXPORT std::ostream& operator<<(std::ostream& stream,object_kind kind);

    template<typename T>
    concept integral = std::integral<T>&&
                       (!std::is_same_v<T,bool>)&&
                       (!std::is_same_v<T,char8_t>)&&
                       (!std::is_same_v<T,char16_t>)&&
                       (!std::is_same_v<T,char32_t>)&&
                       (!std::is_same_v<T,wchar_t>);

    using timestamp_t = std::chrono::sys_time<std::chrono::nanoseconds>;

    struct sequence_header
    {
        uint32_t size;
    };

    struct map_header
    {
        uint32_t size;
    };

    struct extension
    {
        int8_t type;
        piecewise_data data;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
        std::strong_ordering operator<=>(const extension& other) const noexcept = default;
#pragma clang diagnostic pop
    };

    inline size_t hash_value(const extension& e) noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed,e.type);
        boost::hash_combine(seed,e.data);
        return seed;
    }

    AMPI_EXPORT std::ostream& operator<<(std::ostream& stream,const extension& ext);

    AMPI_EXPORT std::ostream& operator<<(std::ostream& stream,timestamp_t ts);

    class event
    {
        variant<
            std::nullptr_t,bool,uint64_t,int64_t,float,double,
            map_header,sequence_header,piecewise_data,extension,piecewise_string,timestamp_t
        > v_;

        template<typename T>
        constexpr static bool is_event_type_v = boost::mp11::mp_find<decltype(v_),T>{}<
                                                boost::mp11::mp_size<decltype(v_)>{};
    public:
        event(std::convertible_to<decltype(v_)> auto x)
            : v_{std::move(x)}
        {}

        object_kind kind() const noexcept
        {
            return static_cast<object_kind>(v_.index());
        }

        template<typename T>
            requires is_event_type_v<T>
        T* get_if() noexcept
        {
            return ampi::get_if<T>(&v_);
        }

        template<typename T>
            requires is_event_type_v<T>
        const T* get_if() const noexcept
        {
            return ampi::get_if<T>(&v_);
        }

        friend std::ostream& operator<<(std::ostream& stream,const event& e);
    };

    using event_result = boost::outcome_v2::boost_result<event>;

    template<typename T>
    concept event_source = async_generator_yielding<T,event_result>;
}

template<>
struct std::hash<ampi::extension>
{
    std::size_t operator()(const ampi::extension& e) const noexcept
    {
        return hash_value(e);
    }
};

#endif
