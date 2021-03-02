//
// Created by r.anchugov on 08.02.2021.
//

#ifndef MELON_CORE_JNI_TESTS_HPP
#define MELON_CORE_JNI_TESTS_HPP

#include <string>
#include <cstdint>

int int_test_sum(int a, int b);

double double_test_sum(double a, double b);

std::uint64_t std_test_sum(std::uint64_t a, std::uint64_t b);

std::string string_test();

const char* buff_test();


#endif //MELON_CORE_JNI_TESTS_HPP
