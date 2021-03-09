// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_3E6FEBBC_9311_4E08_B97B_7F7D2835D2B5
#define UUID_3E6FEBBC_9311_4E08_B97B_7F7D2835D2B5

#include <ampi/export.h>
#include <ampi/hash/flat_map.hpp>
#include <ampi/hash/time_point.hpp>
#include <ampi/hash/vector.hpp>
#include <ampi/msgpack.hpp>
#include <ampi/piecewise_view.hpp>
#include <ampi/utils/compare.hpp>

#include <boost/variant2/variant.hpp>

#include <cmath>
#include <initializer_list>
#include <functional>
#include <type_traits>
#include <utility>

namespace ampi
{
    class value;

    using sequence = vector<value>;
    using map = boost::container::flat_map<value,value,std::less<>,
                                           shared_polymorphic_allocator<std::pair<value,value>>>;

    namespace detail
    {
        struct value_equal
        {
            template<typename T,typename U>
            bool operator()(const T&,const U&) const noexcept
            {
                return false;
            }

            template<typename T>
                requires (!std::is_arithmetic_v<T>)
            bool operator()(const T& x,const T& y) const noexcept
            {
                return x==y;
            }

            template<typename T,typename U>
                requires std::is_arithmetic_v<T>&&std::is_arithmetic_v<U>
            bool operator()(T x,T y) const noexcept
            {
                if constexpr(requires { std::cmp_equal(x,y); })
                    return std::cmp_equal(x,y);
                else{
                    if(auto o = x<=>y;o!=std::partial_ordering::unordered)
                        return o==std::partial_ordering::equivalent;
                    return std::isnan(x)==std::isnan(y);
                }
            }
        };

        struct AMPI_EXPORT value_three_way
        {
            size_t i1,i2;

            template<typename T,typename U>
            std::strong_ordering operator()(const T&,const U&) const noexcept
            {
                return i1<=>i2;
            }

            template<typename T>
                requires (!std::is_arithmetic_v<T>)
            std::strong_ordering operator()(const T& x,const T& y) const noexcept
            {
                return x<=>y;
            }

            std::strong_ordering operator()(std::nullptr_t,std::nullptr_t) const noexcept
            {
                return std::strong_ordering::equal;
            }

            template<typename T,typename U>
                requires std::is_arithmetic_v<T>&&std::is_arithmetic_v<U>
            std::strong_ordering operator()(T x,T y) const noexcept
            {
                if constexpr(requires { std::cmp_equal(x,y); })
                    return std::cmp_equal(x,y)?std::strong_ordering::equal:
                           std::cmp_less(x,y)?std::strong_ordering::less:
                                              std::strong_ordering::greater;
                else{
                    if(auto o = x<=>y;o!=std::partial_ordering::unordered)
                        return make_strong_ordering(o);
                    return std::isnan(x)<=>std::isnan(y);
                }
            }

            std::strong_ordering operator()(const sequence& s1,const sequence& s2) const noexcept;
            std::strong_ordering operator()(const map& s1,const map& s2) const noexcept;

            std::strong_ordering operator()(timestamp_t t1,timestamp_t t2) const noexcept
            {
                // FIXME: no operator<=> for std::chrono::time_point.
                return t1==t2?std::strong_ordering::equal:
                       t1<t2 ?std::strong_ordering::less:
                              std::strong_ordering::greater;
            }
        };
    }

    class AMPI_EXPORT value
    {
        struct print_visitor;

        boost::variant2::variant<std::nullptr_t,bool,uint64_t,int64_t,float,double,map,sequence,
                                 piecewise_data,extension,piecewise_string,timestamp_t> v_;

        template<typename T>
        constexpr static bool is_value_type_v = boost::mp11::mp_contains<decltype(v_),T>{};
    public:
        value() noexcept = default;

        value(bool v) noexcept
            : v_{v}
        {}

        template<std::integral T>
        value(T v)
            : v_{[v](){
                     if constexpr(std::signed_integral<T>)
                         return int64_t(v);
                     else
                         return uint64_t(v);
                 }()}
        {}

        value(float v) noexcept
            : v_{v}
        {}

        value(double v) noexcept
            : v_{v}
        {}

        value(sequence seq) noexcept
            : v_{std::move(seq)}
        {}

        value(std::initializer_list<value> il)
            : v_{boost::variant2::in_place_type<sequence>,il.begin(),il.end()}
        {}

        value(map m) noexcept
            : v_{std::move(m)}
        {}

        value(std::initializer_list<map::value_type> il)
            : v_{boost::variant2::in_place_type<map>,il.begin(),il.end()}
        {}

        value(piecewise_data pd) noexcept
            : v_{std::move(pd)}
        {}

        value(extension e) noexcept
            : v_{std::move(e)}
        {}

        value(piecewise_string ps) noexcept
            : v_{std::move(ps)}
        {}

        value(timestamp_t t) noexcept
            : v_{t}
        {}

        object_kind kind() const noexcept
        {
            return object_kind(v_.index());
        }

        template<typename T>
            requires is_value_type_v<T>
        T* get_if() const
        {
            return boost::variant2::get_if<T>(v_);
        }

        template<typename T>
            requires is_value_type_v<T>
        T* get_if()
        {
            return boost::variant2::get_if<T>(v_);
        }

        bool operator==(const value& other) const noexcept
        {
            return boost::variant2::visit(detail::value_equal{},v_,other.v_);
        }

        std::strong_ordering operator<=>(const value& other) const noexcept
        {
            return boost::variant2::visit(detail::value_three_way{v_.index(),other.v_.index()},
                                          v_,other.v_);
        }

        friend size_t hash_value(const value& v) noexcept
        {
            return hash_value(v.v_);
        }

        friend std::ostream& operator<<(std::ostream& stream,const value& v);
    };
}

template<>
struct std::hash<ampi::value>
{
    std::size_t operator()(const ampi::value& v) const noexcept
    {
        return hash_value(v);
    }
};

#endif
