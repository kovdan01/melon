// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_B9560858_2AF8_477A_97E4_064EB10A3E2A
#define UUID_B9560858_2AF8_477A_97E4_064EB10A3E2A

#include <ampi/buffer.hpp>
#include <ampi/hash/span.hpp>

#include <boost/container/pmr/small_vector.hpp>
#include <boost/container_hash/hash_fwd.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/variant2/variant.hpp>

#include <cassert>
#include <compare>
#include <iosfwd>

namespace ampi
{
    template<typename View,typename Container>
    class piecewise_view
    {
        using piece_vector_t = boost::container::small_vector<
            buffer,2,shared_polymorphic_allocator<buffer>>;
        using variant_t = boost::variant2::variant<Container,piece_vector_t>;

        variant_t v_;

        size_t piece_size() const noexcept
        {
            std::size_t s = 0;
            for(const auto& view:*boost::variant2::get_if<piece_vector_t>(&v_))
                s += view.size();
            return s;
        }

        Container merge_impl(typename Container::allocator_type alloc) const
        {
            Container cont(std::move(alloc));
            cont.reserve(piece_size());
            for(const auto& p:*boost::variant2::get_if<piece_vector_t>(&v_))
                cont.insert(cont.end(),p.view().begin(),p.view().end());
            return cont;
        }
    public:
        class iterator : public boost::stl_interfaces::proxy_iterator_interface<
            iterator,std::contiguous_iterator_tag,View>
        {
        public:
            iterator() noexcept = default;

            View operator*() const noexcept
            {
                if(auto single = boost::variant2::get_if<Container>(v_)){
                    assert(!index_);
                    return {single->data(),single->size()};
                }
                auto& buf = (*boost::variant2::get_if<piece_vector_t>(v_))[size_t(index_)];
                return {reinterpret_cast<typename View::const_pointer>(buf.data()),buf.size()};
            }

            iterator& operator+=(ptrdiff_t n) noexcept
            {
                index_ += n;
                assert(index_>=0);
                if(auto single = boost::variant2::holds_alternative<Container>(*v_))
                    assert(index_<=1);
                else
                    assert(size_t(index_)<=boost::variant2::get_if<piece_vector_t>(v_)->size());
                return *this;
            }

            ptrdiff_t operator-(const iterator& other) const noexcept
            {
                assert(v_==other.v_);
                return index_-other.index_;
            }

            // We don't want operator== synthesized as operator- == 0,
            // as that asserts same view, which comparing for equality doesn't need.
            // = default doesn't work as bases are not comparable.
            bool operator==(const iterator& other) const noexcept
            {
                return v_==other.v_&&index_==other.index_;
            }

            bool operator!=(const iterator& other) const noexcept
            {
                return !(*this==other);
            }
        private:
            friend piecewise_view;

            const variant_t* v_ = {};
            ptrdiff_t index_ = 0;

            iterator(const variant_t* v,ptrdiff_t index) noexcept
                : v_{v},
                  index_{index}
            {}
        };

        piecewise_view() noexcept = default;

        piecewise_view(Container cont) noexcept
            : v_{std::move(cont)}
        {}

        piecewise_view(buffer buf,shared_polymorphic_allocator<buffer> spa) noexcept
            : v_{boost::variant2::in_place_type<piece_vector_t>,std::move(spa)}
        {
            push_back(std::move(buf));
        }

        void push_back(buffer buf)
        {
            assert(boost::variant2::holds_alternative<piece_vector_t>(v_));
            boost::variant2::get_if<piece_vector_t>(v_)->push_back(std::move(buf));
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return !size();
        }

        size_t size() const noexcept
        {
            if(auto single = boost::variant2::get_if<Container>(&v_))
                return single->size();
            return piece_size();
        }

        Container merge(typename Container::allocator_type alloc = {}) const&
        {
            if(auto single = boost::variant2::get_if<Container>(&v_))
                return {*single,std::move(alloc)};
            return merge_impl(std::move(alloc));
        }

        Container merge(typename Container::allocator_type alloc = {}) &&
        {
            if(auto single = boost::variant2::get_if<Container>(&v_))
                return {std::move(*single),std::move(alloc)};
            return merge_impl(std::move(alloc));
        }

        iterator begin() const noexcept
        {
            return {&v_,0};
        }

        iterator end() const noexcept
        {
            if(boost::variant2::holds_alternative<Container>(v_))
                return {&v_,1};
            else
                return {&v_,ptrdiff_t(boost::variant2::get_if<piece_vector_t>(&v_)->size())};
        }

        bool operator==(const piecewise_view& other) const noexcept
        {
            return size()==other.size()&&*this<=>other==std::strong_ordering::equal;
        }

        std::strong_ordering operator<=>(const piecewise_view& other) const noexcept
        {
            // FIXME: libc++ doesn't provide std::ranges::join.
            auto i1 = begin(),e1 = end(),
                 i2 = other.begin(),e2 = other.end();
            View v1,v2;
            if(i1!=e1)
                v1 = *i1;
            if(i2!=e2)
                v2 = *i2;
            for(;;){
                if(i1==e1)
                    return i2==e2?std::strong_ordering::equal:std::strong_ordering::less;
                if(i2==e2)
                    return std::strong_ordering::greater;
                auto n = std::max(v1.size(),v2.size());
                // FIXME: missing std::lexicographical_compare_three_way
                for(std::size_t i=0;i<n;++i)
                    if(auto o = v1[i]<=>v2[i];o!=std::strong_ordering::equal)
                        return o;
                if(n==v1.size()&&++i1!=e1)
                    v1 = *i1;
                if(n==v2.size()&&++i2!=e2)
                    v2 = *i2;
            }
        }

        friend size_t hash_value(const piecewise_view& pv) noexcept
        {
            return boost::hash_range(pv.begin(),pv.end());
        }
    };

    using piecewise_string = piecewise_view<string_view,string>;
    using piecewise_data = piecewise_view<binary_cview_t,vector<byte>>;

    AMPI_EXPORT std::ostream& operator<<(std::ostream& stream,const piecewise_string& ps);
    AMPI_EXPORT std::ostream& operator<<(std::ostream& stream,const piecewise_data& ps);
}

template<typename View,typename Container>
struct std::hash<ampi::piecewise_view<View,Container>>
{
    std::size_t operator()(const ampi::piecewise_view<View,Container>& pv) const noexcept
    {
        return hash_value(pv);
    }
};

#endif
