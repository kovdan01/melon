# Copyright 2020 Pavel A. Lebedev
# Licensed under the Apache License, Version 2.0.
# (See accompanying file LICENSE.txt or copy at
#  http://www.apache.org/licenses/LICENSE-2.0)
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19)

project(ampi
        VERSION 0.0.0
        DESCRIPTION "Asynchronous MessagePack Implementation"
        LANGUAGES CXX
)

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

add_library(${PROJECT_NAME}
    include/ampi/buffer.hpp
    include/ampi/coro/awaiter_wrapper.hpp
    include/ampi/coro/coro_handle_owner.hpp
    include/ampi/coro/coroutine.hpp
    include/ampi/coro/stdcoro.hpp
    include/ampi/coro/traits.hpp
    include/ampi/coro/use_coroutine.hpp
    include/ampi/execution/executor_wrapper.hpp
    include/ampi/execution/prefer.hpp
    include/ampi/execution/query.hpp
    include/ampi/execution/require.hpp
    include/ampi/execution/traits.hpp
    include/ampi/hash/flat_map.hpp
    include/ampi/hash/span.hpp
    include/ampi/hash/time_point.hpp
    include/ampi/hash/vector.hpp
    include/ampi/memory.hpp
    include/ampi/msgpack.hpp
    include/ampi/parser.hpp
    include/ampi/piecewise_view.hpp
    include/ampi/pmr/detail/block_list_resource.hpp
    include/ampi/pmr/reusable_monotonic_buffer_resource.hpp
    include/ampi/pmr/segmented_stack_resource.hpp
    include/ampi/pmr/shared_polymorphic_allocator.hpp
    include/ampi/span_sinks/ostream_span_sink.hpp
    include/ampi/span_sources/async_stream_span_source.hpp
    include/ampi/span_sources/istream_span_source.hpp
    include/ampi/span_sources/one_span_source.hpp
    include/ampi/span_sources/span_source.hpp
    include/ampi/utils/archetypes.hpp
    include/ampi/utils/bit.hpp
    include/ampi/utils/empty_subobject.hpp
    include/ampi/utils/flags.hpp
    include/ampi/utils/repeated.hpp
    include/ampi/utils/stdtypes.hpp
    include/ampi/utils/tag_invoke.hpp
    include/ampi/utils/tagged_pointer.hpp
    include/ampi/utils/transparent_return.hpp
    include/ampi/value.hpp
    include/ampi/utf8_validator.hpp
    src/buffer.cpp
    src/emitter.cpp
    src/msgpack.cpp
    src/parser.cpp
    src/piecewise_view.cpp
    src/pmr/detail/block_list_resource.cpp
    src/pmr/reusable_monotonic_buffer_resource.cpp
    src/pmr/segmented_stack_resource.cpp
    src/pmr/shared_polymorphic_allocator.cpp
    src/utf8_validator.cpp
    src/value.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

find_package(Boost 1.75 REQUIRED COMPONENTS container)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::container)

ntc_target(${PROJECT_NAME})

set(DOXYGEN_HTML_COLORSTYLE_HUE 40)

find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(Doxygen_FOUND)
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_SHOW_INCLUDE_FILES NO)
    set(DOXYGEN_SORT_MEMBERS_CTORS_1ST YES)
    set(DOXYGEN_STRIP_FROM_PATH "${CMAKE_SOURCE_DIR}/include")
    set(DOXYGEN_DOT_IMAGE_FORMAT svg)
    set(DOXYGEN_INTERACTIVE_SVG YES)
    if(CMAKE_CXX_COMPILER_ID MATCHES Clang AND CMAKE_EXPORT_COMPILE_COMMANDS)
        set(DOXYGEN_CLANG_ASSISTED_PARSING YES)
        set(DOXYGEN_CLANG_DATABASE_PATH "${CMAKE_BINARY_DIR}")
    endif()
    doxygen_add_docs(${PROJECT_NAME}-docs
        "${CMAKE_SOURCE_DIR}/include"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    if(NOT TARGET docs)
        add_custom_target(docs)
    endif()
    add_dependencies(docs ${PROJECT_NAME}-docs)
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
