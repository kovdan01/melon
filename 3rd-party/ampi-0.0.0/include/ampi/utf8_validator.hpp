// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_0B2A1F3D_D8D9_4FD2_9DE0_CF75D3A69F9E
#define UUID_0B2A1F3D_D8D9_4FD2_9DE0_CF75D3A69F9E

#include <ampi/export.h>
#include <ampi/memory.hpp>

#include <cstdint>

namespace ampi
{
    class AMPI_EXPORT utf8_validator
    {
    public:
        static const char* validate(string_view sv) noexcept;

        utf8_validator()
        {
            reset();
        }

        void reset()
        {
            state_ = 0;
        }

        explicit operator bool() const noexcept
        {
            return !state_;
        }

        const char* operator()(string_view sv) noexcept;
    private:
        uint8_t state_;
    };
}

#endif
