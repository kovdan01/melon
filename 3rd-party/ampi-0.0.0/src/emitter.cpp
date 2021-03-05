// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#include <ampi/emitter.hpp>

#include <bit>
#include <cstring>

namespace ampi
{
    bool iovec_emitter::reset_iovecs() noexcept
    {
        used_iovecs_ = total_bytes_ = 0;
        if(!deferred_[0].empty())
            return write_internal(std::exchange(deferred_[0],{}));
        if(!deferred_[1].empty()){
            bool allow_not_copied;
            return write(std::exchange(deferred_[1],{}),
                         deferred1_allow_no_copy?&allow_not_copied:nullptr);
        }
        return true;
    }

    bool iovec_emitter::write_internal(binary_cview_t data) noexcept
    {
        bool merge_with_last = used_iovecs_&&
            static_cast<const byte*>(iovecs_.back().data())+iovecs_.back().size()==data.data();
        if(!merge_with_last&&used_iovecs_==iovecs_.size()){
            deferred_[0] = data;
            return false;
        }
        size_t remaining = settings_.max_total_io_size-total_bytes_,
               n = std::min(remaining,data.size());
        if(merge_with_last)
            iovecs_.back() = {iovecs_.back().data(),iovecs_.back().size()+n};
        else
            iovecs_[used_iovecs_++] = {data.data(),n};
        total_bytes_ += n;
        if(n==data.size())
            return true;
        deferred_[0] = data.last(data.size()-n);
        return false;
    }

    bool iovec_emitter::write(binary_cview_t data,bool* copied) noexcept
    {
        if(data.empty()){
            if(copied)
                *copied = true;
            return true;
        }
        // Don't copy if we can signal we didn't and data is too big or
        // copying would require another allocation.
        if(copied&&(data.size()>=settings_.min_iovec_size||mr_.remaining_storage()<data.size()))
            *copied = false;
        else{
            auto new_data = allocate_internal(data.size());
            std::memcpy(new_data.data(),data.data(),data.size());
            data = new_data;
            if(copied)
                *copied = true;
        }
        if(!write_internal(data)){
            deferred_[1] = data;
            // FIXME: This disables no-copy for small data when write() below would have done that.
            // This only pessimizes small buffers when the header doesn't fit, which is rare,
            // and giving a more precise answer requires simulating the whole limit logic,
            // including memory resource details which is not worth it likely.
            deferred1_allow_no_copy = copied&&data.size()>=settings_.min_iovec_size;
            if(copied)
                *copied = deferred1_allow_no_copy;
            return false;
        }
        return true;
    }
}
