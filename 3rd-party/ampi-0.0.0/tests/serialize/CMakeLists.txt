add_executable(test-serialize
    src/main.cpp
)

target_link_libraries(test-serialize PRIVATE ampi::ampi ampi::tests-common)

ntc_target(test-serialize)

add_test(NAME serialize COMMAND test-serialize)
