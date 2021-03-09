// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#include <ampi/pmr/reusable_monotonic_buffer_resource.hpp>

namespace ampi
{
    reusable_monotonic_buffer_resource::~reusable_monotonic_buffer_resource() = default;

    void reusable_monotonic_buffer_resource::do_deallocate(void* /*p*/,size_t /*bytes*/,
                                                           size_t /*alignment*/)
    {}
}
