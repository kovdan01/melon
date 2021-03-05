// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#include <ampi/parser.hpp>

#include <boost/config.hpp>
#include <boost/endian/conversion.hpp>

#include <algorithm>
#include <bit>
#include <chrono>
#include <cstddef>
#include <system_error>

namespace ampi
{
    namespace
    {
        struct parser_error_category : boost::system::error_category
        {
            const char* name() const noexcept override;
            std::string message(int e) const override;
            boost::system::error_condition default_error_condition(int e) const noexcept override;
        };

        const char* parser_error_category::name() const noexcept
        {
            return "ampi::parser";
        }

        std::string parser_error_category::message(int e) const
        {
            switch(parser_result{e}){
                case parser_result::success:
                    return "success";
                case parser_result::incomplete:
                    return "object incomplete";
                case parser_result::invalid_object_type:
                    return "invalid object type";
                case parser_result::invalid_timestamp_length:
                    return "invalid timestamp length";
                case parser_result::invalid_utf8:
                    return "invalid utf8 string data";
                default:
                    return "unknown error";
            }
        }

        boost::system::error_condition parser_error_category::default_error_condition(int e) const noexcept
        {
            switch(parser_result{e}){
                case parser_result::success:
                    return {};
                case parser_result::incomplete:
                    return boost::system::errc::resource_unavailable_try_again;
                case parser_result::invalid_object_type:
                case parser_result::invalid_timestamp_length:
                case parser_result::invalid_utf8:
                    return boost::system::errc::illegal_byte_sequence;
                default:
                    return {e,*this};
            }
        }

        const parser_error_category& get_parser_error_category() noexcept
        {
            static const parser_error_category pec;
            return pec;
        }
    }

    boost::system::error_code make_error_code(parser_result ec) noexcept
    {
        return {static_cast<int>(ec),get_parser_error_category()};
    }

    boost::system::error_condition make_error_condition(parser_result ec) noexcept
    {
        return {static_cast<int>(ec),get_parser_error_category()};
    }


#if 0

    inline void parser::consume(size_t n) noexcept
    {
        assert(n<=span_.size());
        span_ = span_.last(span_.size()-n);
    }

    namespace
    {
        template<typename T>
        inline T load_be(const byte* p)
        {
            return boost::endian::endian_load<T,sizeof(T),boost::endian::order::big>(
                reinterpret_cast<const unsigned char*>(p));
        }

        template<typename T>
        inline void swap_be(byte* p)
        {
            T x = load_be<T>(p);
            std::memcpy(p,&x,sizeof(T));
        }
    }

    auto parser::operator()() noexcept -> result
    {
        assert(kind()!=object_kind::invalid);
        switch(state_){
            case state_t::before_object:
                {
#ifndef NDEBUG
                    kind_ = object_kind::invalid;
#endif
                    if(span_.empty())
                        return result::incomplete;
                    uint8_t first_byte = to_integer<uint8_t>(span_[0]);
                    consume(1);
                    if(first_byte<0x80||first_byte>=0xe0){
                        kind_ = object_kind(unsigned(object_kind::unsigned_int)+(first_byte>>7));
                        buffer_[0] = byte{first_byte};
                        goto done;
                    }
                    if(first_byte<0xa0){
                        unsigned t = (first_byte>>4)&1;
                        kind_ = object_kind(unsigned(object_kind::map)+t);
                        length_ = first_byte&0xf;
                        goto done;
                    }
                    if(first_byte==0xc0){
                        kind_ = object_kind::null;
                        goto done;
                    }
                    if((first_byte|1)==0xc3){
                        kind_ = object_kind::bool_;
                        buffer_[0] = byte{uint8_t(first_byte&1u)};
                        goto done;
                    }
                    if(first_byte==0xc1) [[unlikely]] {
                        kind_ = object_kind::invalid;
                        return result::invalid_object_type;
                    }
                    first_byte_ = first_byte;
                    if((first_byte>>5)==0x5){
                        kind_ = object_kind::string;
                        length_ = first_byte&0x1f;
                        goto start_reading_bytes;
                    }
                    if(first_byte<=0xc9||(first_byte>=0xd9&&first_byte<=0xdb)){
                        kind_ = first_byte>=0xd9?object_kind::string:
                                object_kind(std::uint8_t(object_kind::binary)+(first_byte>=0xc7));
                        length_ = 1u<<((first_byte_-0xc4)%3);
                        goto start_reading_length;
                    }
                    if(first_byte<=0xcb){
                        unsigned t = first_byte-0xca;
                        kind_ = object_kind(unsigned(object_kind::float_)+t);
                        length_ = 4u<<t;
                        goto start_reading_bytes;
                    }
                    if(first_byte<=0xd3){
                        kind_ = object_kind(unsigned(object_kind::unsigned_int)+((first_byte>>4)&1));
                        length_ = 1u<<(first_byte&0x7);
                        std::fill(buffer_,buffer_+8,byte{});
                        goto start_reading_bytes;
                    }
                    if(first_byte<=0xd8){
                        kind_ = object_kind::extension;
                        length_ = 1u<<(first_byte-0xd4);
                        goto reading_ext_type;
                    }
                    // 0xdc-0xdf
                    kind_ = object_kind(unsigned(object_kind::map)-((first_byte>>1)&1));
                    length_ = 2u<<(first_byte&1);
                }
start_reading_length:
                partial_read_done_ = 0;
                std::fill(buffer_,buffer_+4,byte{});
                [[fallthrough]] ;
            case state_t::reading_length:
                if(span_.empty())
                    goto exit_reading_length;
                {
                    uint32_t n = uint32_t(std::min(size_t(length_),span_.size()));
                    std::memcpy(buffer_+partial_read_done_,span_.data(),n);
                    consume(n);
                    partial_read_done_ += n;
                }
                if(length_!=partial_read_done_){
exit_reading_length:
                    state_ = state_t::reading_length;
                    return result::incomplete;
                }
                length_ = load_be<uint32_t>(buffer_);
                if(first_byte_>=0xdc)
                    goto done;
                if(first_byte_<0xc7||first_byte_>0xc9)
                    goto start_reading_bytes;
                [[fallthrough]] ;
            case state_t::reading_ext_type:
reading_ext_type:
                if(span_.empty()){
                    state_ = state_t::reading_ext_type;
                    return result::incomplete;
                }
                extension_type_ = std::to_integer<int8_t>(span_[0]);
                consume(1);
                if(extension_type_==-1){
                    if(length_!=4&&length_!=8&&length_!=12){
                        kind_ = object_kind::invalid;
                        return result::invalid_timestamp_length;
                    }
                    kind_ = object_kind::timestamp;
                }
start_reading_bytes:
                partial_read_done_ = 0;
                state_ = state_t::reading_bytes;
                [[fallthrough]] ;
            case state_t::reading_bytes:
                if(span_.empty())
                    return result::incomplete;
                {
                    uint32_t n = uint32_t(std::min(size_t(length_),span_.size()));
                    auto d = span_.data();
                    consume(n);
                    partial_read_done_ += n;
                    if((first_byte_>=0xca&&first_byte_<=0xd3)||
                            kind_==object_kind::timestamp){
                        std::memcpy(buffer_+partial_read_done_,d,n);
                        if(partial_read_done_<length_)
                            return result::incomplete;
                        if(length_==1)
                            ;
                        else if(length_==2)
                            swap_be<uint16_t>(buffer_);
                        else if(kind_==object_kind::timestamp){
                            using namespace std::chrono;
                            nanoseconds ns;
                            if(length_==4)
                                ns = nanoseconds{load_be<uint32_t>(buffer_)};
                            else if(length_==8){
                                uint64_t t = load_be<uint64_t>(buffer_);
                                ns = nanoseconds{t>>34}+seconds{(t&0x3ffffffff)};
                            }else // length_==12
                                ns = nanoseconds{load_be<uint32_t>(buffer_)}+
                                     seconds{load_be<uint64_t>(buffer_+4)};
                            timestamp_t ts{ns};
                            std::memcpy(buffer_,&ts,sizeof ts);
                        }else if(length_==4)
                            swap_be<uint32_t>(buffer_);
                        else // length==8
                            swap_be<uint64_t>(buffer_);
                    }else if(kind_==object_kind::string){
                        string_view sv{reinterpret_cast<const char*>(d),n};
                        if(validate_utf8_){
                            if(auto p = reinterpret_cast<const byte*>(uv_(sv))){
                                span_ = {p,size_t(&*span_.end()-p)};
                                return result::invalid_utf8;
                            }
                            if(partial_read_done_==length_&&!uv_)
                                return result::invalid_utf8;
                        }
                        memcpy(buffer_,&sv,sizeof sv);
                    }else{
                        binary_cview_t bv{d,n};
                        std::memcpy(buffer_,&bv,sizeof bv);
                    }
                }
        };
done:
        state_ = state_t::before_object;
        return result::success;
    }
#endif
}
