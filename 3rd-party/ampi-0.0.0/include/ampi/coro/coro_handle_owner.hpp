// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_729EEA85_3C22_4CF1_92BB_A533E2280164
#define UUID_729EEA85_3C22_4CF1_92BB_A533E2280164

#include <ampi/coro/stdcoro.hpp>

#include <utility>

namespace ampi
{
    template<typename Promise = void>
    class coro_handle_owner
    {
    public:
        using handle_t = stdcoro::coroutine_handle<Promise>;

        coro_handle_owner(handle_t handle = {}) noexcept
            : handle_{handle}
        {}

        coro_handle_owner(coro_handle_owner&& other) noexcept
            : handle_{std::exchange(other.handle_,{})}
        {}

        coro_handle_owner& operator=(coro_handle_owner&& other) noexcept
        {
            if(this!=&other){
                destroy();
                handle_ = std::exchange(other.handle_,{});
            }
            return *this;
        }

        ~coro_handle_owner()
        {
            destroy();
        }

        explicit operator bool() const noexcept
        {
            return handle_&&!handle_.done();
        }

        handle_t get() const noexcept
        {
            return handle_;
        }

        Promise* operator->() const noexcept
        {
            return &handle_.promise();
        }

        [[nodiscard]] handle_t release() && noexcept
        {
            return std::exchange(handle_,{});
        }
    private:
        handle_t handle_;

        void destroy() noexcept
        {
            if(handle_)
                handle_.destroy();
        }
    };
}

#endif
