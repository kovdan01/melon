add_library(ce-utils
    include/ce/aligned_allocator.hpp
    include/ce/charconv.hpp
    include/ce/errno.hpp
    include/ce/format.hpp
    include/ce/generator.hpp
    include/ce/hex.hpp
    include/ce/random.hpp
    include/ce/stdcoro.hpp
    src/hex.cpp
    src/random.cpp
)

if(NOT WIN32)
    target_sources(ce-utils PRIVATE
        include/ce/file_descriptor.hpp
        include/ce/pipe.hpp
        src/file_descriptor.cpp
    )
endif()

target_compile_features(ce-utils PUBLIC cxx_std_20)

target_link_libraries(ce-utils PUBLIC Boost::headers)

ntc_target(ce-utils ALIAS_NAME ce::utils HEADER_PREFIX "ce/utils/")
