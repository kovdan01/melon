// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_130BF8D1_54F5_4D28_A1B1_3AF65AA8E8DA
#define UUID_130BF8D1_54F5_4D28_A1B1_3AF65AA8E8DA

#include <ampi/pmr/detail/block_list_resource.hpp>

namespace ampi
{
    // Like boost::container::monotonic_buffer_resource, but:
    // - Provides reuse() that starts using all allocated buffers anew, assuming
    //   all contents has been trivially destructed.
    // - Doesn't provide construction from existing buffer, access to current buffer and
    //   next buffer size.
    class AMPI_EXPORT reusable_monotonic_buffer_resource final
        : public detail::block_list_resource<reusable_monotonic_buffer_resource>
    {
    public:
        using base_t = detail::block_list_resource<reusable_monotonic_buffer_resource>;

        using base_t::block_list_resource;

        ~reusable_monotonic_buffer_resource() override;

        void release() noexcept
        {
            free_blocks();
            first_ = current_ = nullptr;
            p_ = nullptr;
        }

        void reuse() noexcept
        {
            current_ = first_;
            p_ = current_?reinterpret_cast<byte*>(current_)+sizeof(detail::block_header):nullptr;
        }
    protected:
        void do_deallocate(void* p,size_t bytes,size_t alignment) override;
    private:
        friend base_t;

        void* p_ = nullptr;

        void* current_p() const noexcept
        {
            return p_;
        }

        void set_current_p(void* new_p) noexcept
        {
            p_ = new_p;
        }
    };
}

#endif
