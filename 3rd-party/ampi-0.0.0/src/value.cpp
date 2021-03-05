// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#include <ampi/value.hpp>

#include <ampi/utils/repeated.hpp>

#include <boost/io/ios_state.hpp>

#include <iomanip>
#include <ostream>

namespace ampi
{
    namespace detail
    {
        std::strong_ordering value_three_way::operator()(const sequence& s1,const sequence& s2) const noexcept
        {
            // FIXME: no std::lexicographical_compare_three_way
            auto it1=s1.begin(),it2=s2.begin(),e1=s1.end(),e2=s2.end();
            for(;it1!=e1&&it2!=e2;++it1,++it2)
                if(auto o = *it1<=>*it2;o!=std::strong_ordering::equal)
                    return o;
            return it1!=e1?std::strong_ordering::greater:
                   it2!=e2?std::strong_ordering::less:
                           std::strong_ordering::equal;
        }

        std::strong_ordering value_three_way::operator()(const map& m1,const map& m2) const noexcept
        {
            // FIXME: no std::lexicographical_compare_three_way
            auto it1=m1.begin(),it2=m2.begin(),e1=m1.end(),e2=m2.end();
            for(;it1!=e1&&it2!=e2;++it1,++it2){
                // FIXME: no operator<=> for std::pair
                if(auto o = it1->first<=>it2->first;o!=std::strong_ordering::equal)
                    return o;
                if(auto o = it1->second<=>it2->second;o!=std::strong_ordering::equal)
                    return o;
            }
            return it1!=e1?std::strong_ordering::greater:
                   it2!=e2?std::strong_ordering::less:
                           std::strong_ordering::equal;
        }
    }

    struct value::print_visitor
    {
        constexpr static std::size_t indent = 4;

        std::ostream& stream_;
        size_t current_indent_ = 0;

        void operator()(auto x) const
        {
            stream_ << x;
        }

        void operator()(std::nullptr_t) const
        {
            stream_ << "null";
        }

        void operator()(const sequence& x) const
        {
            stream_ << '[';
            if(!x.empty()){
                auto new_indent = current_indent_+indent;
                stream_ << '\n' << repeated(new_indent);
                auto it = x.begin(),e = x.end();
                boost::variant2::visit(print_visitor{stream_,new_indent},it++->v_);
                for(;it!=e;++it){
                    stream_ << ",\n" << repeated(new_indent);
                    boost::variant2::visit(print_visitor{stream_,new_indent},it->v_);
                }
                stream_ << '\n' << repeated(new_indent);
            }
            stream_ << ']';
        }

        void operator()(const map& x) const
        {
            stream_ << '{';
            if(!x.empty()){
                auto new_indent = current_indent_+indent;
                stream_ << '\n' << repeated(new_indent);
                auto it = x.begin(),e = x.end();
                boost::variant2::visit(print_visitor{stream_,new_indent},it->first.v_);
                stream_ << " : ";
                boost::variant2::visit(print_visitor{stream_,new_indent},it++->second.v_);
                for(;it!=e;++it){
                    stream_ << ",\n" << repeated(new_indent);
                    boost::variant2::visit(print_visitor{stream_,new_indent},it->first.v_);
                    stream_ << " : ";
                    boost::variant2::visit(print_visitor{stream_,new_indent},it->second.v_);
                }
                stream_ << '\n' << repeated(current_indent_);
            }
            stream_ << '}';
        }

        void operator()(const piecewise_string& x) const
        {
            // FIXME: libc++ has no std::ranges::join
            stream_ << '"';
            for(auto piece:x)
                for(char c:piece){
                    if(c=='"'||c=='\\')
                        stream_ << '\\';
                    stream_ << c;
                }
            stream_ << '"';
        }

        void operator()(const piecewise_data& x) const
        {
            stream_ << "Binary(" << x << ')';
        }

        void operator()(const timestamp_t& t) const
        {
            // FIXME:: no operator<< for std::chrono::sys_time.
            auto dp = std::chrono::floor<std::chrono::days>(t);
            std::chrono::year_month_day ymd{dp};
            std::chrono::hh_mm_ss hms{t-dp};
            boost::io::ios_fill_saver ifs{stream_};
            stream_ << std::setfill('0')
                    << std::setw(4) << int(ymd.year()) << '-'
                    << std::setw(2) << unsigned(ymd.month()) << '-'
                    << std::setw(2) << unsigned(ymd.day()) << ' '
                    << std::setw(2) << hms.hours().count() << ':'
                    << std::setw(2) << hms.minutes().count() << ':'
                    << std::setw(2) << hms.seconds().count();
        }
    };

    std::ostream& operator<<(std::ostream& stream,const value& v)
    {
        boost::io::ios_flags_saver ifs{stream};
        stream << std::boolalpha;
        boost::variant2::visit(value::print_visitor{stream},v.v_);
        return stream;
    }
}
