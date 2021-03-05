add_library(ampi-tests-common
    include/ampi/tests/ut_helpers.hpp
    src/main.cpp
)

find_package(ut REQUIRED)

target_link_libraries(ampi-tests-common INTERFACE boost::ut)

ntc_target(ampi-tests-common
    ALIAS_NAME ampi::tests-common
    HEADER_PREFIX ampi/tests/
)
