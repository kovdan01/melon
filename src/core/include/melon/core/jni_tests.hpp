//
// Created by r.anchugov on 08.02.2021.
//

#ifndef MELON_CORE_JNI_TESTS_HPP_
#define MELON_CORE_JNI_TESTS_HPP_

#include <cstdint>
#include <string>

namespace melon::core
{

int int_test_sum(int a, int b);

double double_test_sum(double a, double b);

std::uint64_t std_test_sum(std::uint64_t a, std::uint64_t b);

std::string string_test();

}  // namespace melon::core

#endif  // MELON_CORE_JNI_TESTS_HPP_
