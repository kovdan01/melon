// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#include <ampi/piecewise_view.hpp>

#include <boost/algorithm/hex.hpp>

#include <iterator>
#include <ostream>

namespace ampi
{
    std::ostream& operator<<(std::ostream& stream,const piecewise_string& ps)
    {
        for(auto piece:ps)
            stream << piece;
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream,const piecewise_data& ps)
    {
        for(auto piece:ps)
            boost::algorithm::hex(reinterpret_cast<const uint8_t*>(&piece.front()),
                                    reinterpret_cast<const uint8_t*>(&piece.back())+1,
                                    std::ostream_iterator<char>{stream});
        return stream;
    }
}
