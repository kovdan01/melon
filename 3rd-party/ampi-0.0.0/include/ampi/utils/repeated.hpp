// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_C7F0B0A4_F813_446D_BB2A_1145BB5881F0
#define UUID_C7F0B0A4_F813_446D_BB2A_1145BB5881F0

#include <ostream>
#include <utility>

namespace ampi
{
    namespace detail
    {
        template<typename T>
        struct repeater
        {
            std::size_t n;
            T x;

            friend std::ostream& operator<<(std::ostream& stream,const repeater& r)
            {
                for(std::size_t i=0;i<r.n;++i)
                    stream << r.x;
                return stream;
            }
        };
    }

    template<typename T = char>
    detail::repeater<T> repeated(std::size_t n,T x = ' ')
    {
        return {n,std::move(x)};
    }
}

#endif
