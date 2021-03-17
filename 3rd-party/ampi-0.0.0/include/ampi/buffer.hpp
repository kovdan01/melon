// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_49537D96_1C5A_4CAC_985E_5DE7C3FA0D59
#define UUID_49537D96_1C5A_4CAC_985E_5DE7C3FA0D59

#include <ampi/memory.hpp>
#include <ampi/utils/archetypes.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <cassert>
#include <new>
#include <span>
#include <utility>

namespace ampi
{
    namespace detail
    {
        struct buffer_header : public boost::intrusive_ref_counter<buffer_header>
        {
            size_t capacity_;
            shared_polymorphic_allocator<> spa_;

            buffer_header(size_t n,shared_polymorphic_allocator<>&& spa) noexcept
                : capacity_{n},
                  spa_{std::move(spa)}
            {}

            void operator delete(buffer_header* p,std::destroying_delete_t)
            {
                auto spa = std::move(p->spa_);
                // After move of spa, destruction is trivial.
                // p->~buffer_header();
                spa.deallocate_bytes(p,sizeof(buffer_header)+p->capacity_);
            }
        };
    }

    class cbuffer
    {
    public:
        cbuffer() noexcept = default;

        cbuffer(const cbuffer& other) noexcept = default;
        cbuffer& operator=(const cbuffer& other) noexcept = default;

        cbuffer(cbuffer&& other) noexcept
            : header_{std::move(other.header_)},
              view_{std::exchange(other.view_,{})}
        {}

        cbuffer& operator=(cbuffer&& other) noexcept
        {
            if(this!=&other){
                header_ = std::move(other.header_);
                view_ = std::exchange(other.view_,{});
            }
            return *this;
        }

        cbuffer(binary_cview_t bv) noexcept
            : header_{},
              view_{const_cast<std::byte*>(bv.data()),bv.size()}
        {}

        cbuffer(cbuffer& other,size_t offset,size_t count = std::dynamic_extent) noexcept
            : header_{other.header_},
              view_{other.view_.subspan(offset,count)}
        {}

        cbuffer(cbuffer&& other,size_t offset,size_t count = std::dynamic_extent) noexcept
            : header_{std::move(other.header_)},
              view_{other.view_.subspan(offset,count)}
        {}

        const shared_polymorphic_allocator<>* allocator() const noexcept
        {
            return header_?&header_->spa_:nullptr;
        }

        explicit operator bool() const noexcept
        {
            return view_.data();
        }

        size_t size() const noexcept
        {
            return view_.size();
        }

        const byte* data() const noexcept
        {
            return view_.data();
        }

        binary_cview_t view() const noexcept
        {
            return view_;
        }

        operator boost::asio::const_buffer() const noexcept
        {
            return {data(),size()};
        }
    protected:
        boost::intrusive_ptr<detail::buffer_header> header_;
        binary_view_t view_;

        cbuffer(boost::intrusive_ptr<detail::buffer_header> header) noexcept
            : header_{std::move(header)},
              view_{reinterpret_cast<byte*>(header_.get())+sizeof(detail::buffer_header),
                    header_->capacity_}
        {}
    };

    class buffer : public cbuffer
    {
    public:
        constexpr static size_t default_size = 4*1024*1024;

        buffer(binary_view_t bv) noexcept
            : cbuffer{bv}
        {}

        explicit buffer(size_t n,shared_polymorphic_allocator<> spa = {})
            : cbuffer{::new (spa.allocate_bytes(sizeof(detail::buffer_header)+n,
                  alignof(detail::buffer_header))) detail::buffer_header{n,std::move(spa)}}
        {}

        buffer(buffer& other,size_t offset,size_t count = std::dynamic_extent) noexcept
            : cbuffer{other,offset,count}
        {}

        buffer(buffer&& other,size_t offset,size_t count = std::dynamic_extent) noexcept
            : cbuffer{std::move(other),offset,count}
        {}

        byte* data() const noexcept
        {
            return view_.data();
        }

        binary_view_t view() noexcept
        {
            return view_;
        }

        operator boost::asio::mutable_buffer() noexcept
        {
            return {data(),size()};
        }
    };

    namespace archetypes
    {
        struct buffer_factory : nothing
        {
            buffer get_buffer(size_t = 0);
        };
    }

    template<typename T>
    concept buffer_factory = requires(T& bf) {
        { bf.get_buffer() } -> std::convertible_to<buffer>;
        { bf.get_buffer(size_t{}) } -> std::convertible_to<buffer>;
    };
}

#endif
