// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_1306076B_EC1A_4919_9B7C_1E05348FCF06
#define UUID_1306076B_EC1A_4919_9B7C_1E05348FCF06

#include <ampi/msgpack.hpp>
#include <ampi/pmr/reusable_monotonic_buffer_resource.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/container/pmr/memory_resource.hpp>

#include <type_traits>

namespace ampi
{
    class emitter_base
    {
    public:
    };

    template<typename Derived>
    class emitter_impl : public emitter_base
    {
    public:
        bool operator()(std::nullptr_t /*value*/)
        {
             return write_byte(byte{0xc0});
        }

        bool operator()(bool value)
        {
             return write_byte(byte{uint8_t(0xc2+value)});
        }

        template<typename T,typename = std::enable_if_t<std::is_integral_v<T>>>
        bool operator()(T value)
        {
            if(value>=0){
                if(value<=0x7f)
                    return write_byte(byte{uint8_t(value)});
                if(value<=0xff)
                    return write_big_endian(byte{0xcc},uint8_t(value));
                if(value<=0xffff)
                    return write_big_endian(byte{0xcd},uint16_t(value));
                if(value<=0xffffffff)
                    return write_big_endian(byte{0xce},uint32_t(value));
                return write_big_endian(byte{0xcf},uint32_t(value));
            }
            if(value>=-0x20)
                return write_byte(byte{uint8_t(value)});
            if(value>=-0x80)
                return write_big_endian(byte{0xd0},uint8_t(value));
            if(value>=-0x8000)
                return write_big_endian(byte{0xd1},uint16_t(value));
            if(value>=-0x80000000)
                return write_big_endian(byte{0xd2},uint32_t(value));
            return write_big_endian(byte{0xd3},uint64_t(value));
        }

        bool operator()(float value)
        {
            return write_big_endian(byte{0xca},value);
        }

        bool operator()(double value)
        {
            return write_big_endian(byte{0xcb},value);
        }

        bool operator()(object_kind seq_or_map_kind,uint32_t n)
        {
            bool is_map = seq_or_map_kind==object_kind::map;
            assert(seq_or_map_kind==object_kind::sequence||is_map);
            if(n<=0xf)
                return write_byte(byte{uint8_t((0x90^uint8_t(is_map<<4))|n)});
            if(n<=0xffff)
                return write_big_endian(byte{uint8_t(0xdc|(is_map<<1))},uint16_t(n));
            return write_big_endian(byte{uint8_t(0xdd|(is_map<<1))},n);
        }

        bool operator()(string_view value,bool* copied)
        {
            if(value.size()<=0x1f){
                auto buf = this_().allocate_internal(1);
                buf[0] = byte{uint8_t(0xa0+value.size())};
                return write_composite(buf,{reinterpret_cast<const byte*>(value.data()),
                    value.size()},copied);
            }
            return write_124(0xd9,{reinterpret_cast<const byte*>(value.data()),value.size()},copied);
        }

        bool operator()(binary_cview_t value,bool* copied = nullptr)
        {
            return write_124(0xc4,value,copied);
        }

        bool operator()(int8_t extension,binary_cview_t value,bool* copied)
        {
            binary_view_t buf;
            if(std::has_single_bit(value.size())&&value.size()<=16){
                buf = this_().allocate_internal(1+1);
                buf[0] = byte{uint8_t(0xd3+std::bit_width(value.size()))};
            }else if(value.size()<=0xff){
                buf = this_().allocate_internal(1+1+1);
                buf[0] = byte{0xc7};
                buf[1] = byte{uint8_t(value.size())};
            }else if(value.size()<=0xffff){
                buf = this_().allocate_internal(1+2+1);
                buf[0] = byte{0xc8};
                put_big_endian(buf.data()+1,uint16_t(value.size()));
            }else{
                buf = this_().allocate_internal(1+4+1);
                buf[0] = byte{0xc9};
                put_big_endian(buf.data()+1,uint32_t(value.size()));
            }
            buf[buf.size()-1] = byte{uint8_t(extension)};
            return this_().write_composite(buf,value,copied);  
        }

        bool operator()(timestamp_t value)
        {
            int64_t ns = value.time_since_epoch().count(),
                    s = ns/1'000'000'000;
            ns %= 1'000'000'000;
            binary_view_t span;
            if(s>>34){
                span = this_().allocate_internal(1+1+1+4+8);
                span[0] = byte{0xc7};
                span[1] = byte{12};
                span[2] = byte{uint8_t(-1)};
                put_big_endian(span.data()+1+1+1,uint32_t(ns));
                put_big_endian(span.data()+1+1+1+4,s);
            }else{
                uint64_t d = (uint64_t(ns)<<34)|uint64_t(s);
                if(d>>32){
                    span = this_().allocate_internal(1+1+8);
                    span[0] = byte{0xd7};
                    span[1] = byte{uint8_t(-1)};
                    put_big_endian(span.data()+1+1,d);
                }else{
                    span = this_().allocate_interal(1+1+4);
                    span[0] = byte{0xd6};
                    span[1] = byte{uint8_t(-1)};
                    put_big_endian(span.data()+1+1,uint32_t(d));
                }
            }
            return this_().write_internal(span);
        }
    private:
        Derived& this_() noexcept
        {
            return static_cast<Derived&>(*this);
        }

        bool write_byte(byte value)
        {
            auto buf = this_().allocate_internal(1);
            buf[0] = value;
            return this_().write_internal(buf);
        }

        template<typename T>
        void put_big_endian(byte* p,T value) noexcept
        {
            boost::endian::endian_store<T,sizeof(T),boost::endian::order::big>(
                reinterpret_cast<unsigned char*>(p),value);
        }

        template<typename T>
        bool write_big_endian(byte prefix,T value)
        {
            auto buf = this_().allocate_internal(1+sizeof(T));
            buf[0] = prefix;
            put_big_endian(buf.data()+1,value);
            return this_().write_internal(buf);
        }

        bool write_124(uint8_t prefix_base,binary_cview_t value,bool* copied)
        {
            binary_view_t span;
            if(value.size()<=0xff){
                span = this_().allocate_internal(1+1);
                span[0] = byte{prefix_base};
                span[1] = byte{uint8_t(value.size())};
            }else if(value.size()<=0xffff){
                span = this_().allocate_internal(1+2);
                span[0] = byte{uint8_t(prefix_base+1)};
                put_big_endian(span.data()+1,uint16_t(value.size()));
            }else{
                span = this_().allocate_internal(1+4);
                span[0] = byte{uint8_t(prefix_base+2)};
                put_big_endian(span.data()+1,uint32_t(value.size()));
            }
            return write_composite(span,{reinterpret_cast<const byte*>(value.data()),
                value.size()},copied);
        }

        bool write_composite(binary_cview_t header,binary_cview_t value,bool* copied)
        {
            assert(value.size()<=0xffffffff);
            return this_().write_internal(header)&&this_().write(value,copied);
        }
    };

    template<typename Writer>
    class emitter : public emitter_impl<emitter<Writer>>
    {
    public:
        emitter(Writer& writer)
            : writer_{writer}
        {}        
    private:
        Writer& writer_;
        byte buf_[15];

        binary_view_t allocate_internal(size_t n)
        {
            assert(n<=sizeof buf_);
            return {buf_,n};
        }

        bool write_internal(binary_cview_t data)
        {
            writer_.write(reinterpret_cast<const char*>(data.data()),data.size());
            return true;
        }

        bool write(binary_cview_t data,bool* copied)
        {
            if(copied)
                *copied = true;
            return write_internal(data);
        }
    };

    using stream_emitter = emitter<std::ostream>;

    struct iovec_emitter_settings_t
    {
        size_t min_iovec_size = 16,
               max_total_io_size = size_t(-1);
    };

    class AMPI_EXPORT iovec_emitter : public emitter_impl<iovec_emitter>
    {
    public:
        iovec_emitter(const iovec_emitter_settings_t& settings,
                      boost::container::pmr::memory_resource* mr) noexcept
            : settings_{settings},
              mr_{mr}
        {}

        span<boost::asio::const_buffer> iovecs() const noexcept
        {
            return iovecs_;
        }

        bool reset_iovecs() noexcept;

        bool set_iovecs(span<boost::asio::const_buffer> iovecs) noexcept
        {
            iovecs_ = iovecs;
            return reset_iovecs();
        }

        size_t used_iovecs() const noexcept
        {
            return used_iovecs_;
        }
    private:
        friend class emitter_impl<iovec_emitter>;

        iovec_emitter_settings_t settings_;
        reusable_monotonic_buffer_resource mr_;
        byte* p_ = nullptr;
        size_t used_iovecs_ = 0,total_bytes_ = 0;
        span<boost::asio::const_buffer> iovecs_;
        binary_cview_t deferred_[2];
        bool deferred1_allow_no_copy;

        binary_view_t allocate_internal(size_t n)
        {
            return {static_cast<byte*>(mr_.allocate(n,1)),n};
        }

        bool write_internal(binary_cview_t data) noexcept;
        bool write(binary_cview_t data,bool* copied) noexcept;
    };
}

#endif
