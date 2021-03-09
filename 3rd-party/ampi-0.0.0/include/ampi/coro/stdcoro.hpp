// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_2B9E0A06_885B_41B9_B4DB_2D184249766E
#define UUID_2B9E0A06_885B_41B9_B4DB_2D184249766E

#if __has_include(<coroutine>)
#include <coroutine>
namespace ampi { namespace stdcoro = std; }
#else
#include <experimental/coroutine>
namespace ampi { namespace stdcoro = std::experimental; }
#endif

#endif
