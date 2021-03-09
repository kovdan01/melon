// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_8EA05296_BC62_4F61_AA25_17A7CA2B6A24
#define UUID_8EA05296_BC62_4F61_AA25_17A7CA2B6A24

#include <ampi/msgpack.hpp>
#include <ampi/utf8_validator.hpp>
#include <ampi/utils/empty_subobject.hpp>
#include <ampi/utils/flags.hpp>

#include <cstring>
#include <system_error>
#include <variant>

namespace ampi
{
    enum struct parser_result
    {
        success,
        incomplete,
        invalid_object_type,
        invalid_timestamp_length,
        invalid_utf8
    };

    AMPI_EXPORT boost::system::error_code make_error_code(parser_result ec) noexcept;
    AMPI_EXPORT boost::system::error_condition make_error_condition(parser_result ec) noexcept;

    enum struct parser_option : uint8_t
    {
        skip_utf8_validation = 0b1
    };

    using parser_options = flags<parser_option>;

    template<typename BufferFactory = void>
    class AMPI_EXPORT parser
    {
        constexpr static bool external_buffer_factory = !std::is_same_v<BufferFactory,void>;
    public:
        parser(parser_options options = {}) noexcept
                requires (!external_buffer_factory)
            : options_{options}
        {}

        buffer get_buffer(size_t min_size = 0)
        {
            return {};
        }

        template<span_source SpanSource>
        async_generator<boost::outcome_v2::boost_result<event>> operator()(SpanSource& ss)
            [[clang::lifetimebound]]
        {
            co_return;
        }
    private:
        parser_options options_;
        [[no_unique_address]] subobject_if<external_buffer_factory,BufferFactory*> bf_;

        auto get_buffer_factory() noexcept
        {
            if constexpr(external_buffer_factory)
                return bf_;
            else
                return *this;
        }
    };
}

template<>
struct boost::system::is_error_code_enum<ampi::parser_result> : std::true_type {};

#endif
