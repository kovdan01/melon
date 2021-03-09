// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#include <ampi/utf8_validator.hpp>

namespace ampi
{
    const char* utf8_validator::validate(string_view sv) noexcept
    {
        utf8_validator v;
        if(auto p = v(sv))
            return p;
        if(!v)
            return sv.data()+sv.size();
        return nullptr;
    }

    const char* utf8_validator::operator()(string_view sv) noexcept
    {
        // https://www.unicode.org/versions/Unicode12.0.0/UnicodeStandard-12.0.pdf , table 3-7.
        static const uint8_t classes[256] = {
            // 0: 00..7f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            // 1:  80..8f
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            // 2:  90..9f
            2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
            // 3:  a0..bf
            3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
            3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
            // 4:  c0..c1
            4,4,
            // 5:  c2..df
                5,5,5,5,5,5,5,5,5,5,5,5,5,5,
            5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
            // 6:  e0
            6,
            // 7:  e1..ec
              7,7,7,7,7,7,7,7,7,7,7,7,
            // 8:  ed
                                      8,
            // 9:  ee..ef
                                        9,9,
            // a: f0
            0xa,
            // b: f1..f3
               0xb,0xb,0xb,
            // c: f4
                           0xc,
            // d: f5..ff
                               0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd
        };
        // states:
        // 0x0:  initial
        // 0x10: final 80..bf
        // 0x20: a0..bf that goes to 0x10
        // 0x30: 80..bf that goes to 0x10
        // 0x40: 80..9f that goes to 0x10
        // 0x50: 90..bf that goes to 0x30
        // 0x60: 80..bf that goes to 0x30
        // 0x70: 80..8f that goes to 0x30
        // 0xff: invalid
        static const uint8_t transitions[128] = {
            0x00,0xff,0xff,0xff,0xff,0x10,0x20,0x30,0x40,0x30,0x50,0x60,0x70,0xff,0xff,0xff,
            0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
            0xff,0xff,0xff,0x10,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
            0xff,0x10,0x10,0x10,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
            0xff,0x10,0x10,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
            0xff,0xff,0x30,0x30,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
            0xff,0x30,0x30,0x30,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
            0xff,0x30,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
        };
        uint8_t s = state_;
        for(size_t i=0;i<sv.size();++i){
            s = transitions[s+classes[uint8_t(sv[i])]];
            if(s==0xff)
                return &sv[i];
        }
        state_ = s;
        return nullptr;
    }
}
