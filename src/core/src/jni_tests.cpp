//
// Created by r.anchugov on 08.02.2021.
//

#include <melon/core/jni_tests.hpp>

int int_test_sum(int a, int b)
{
    return a + b;
}

double double_test_sum(double a, double b)
{
    return a + b;
}

std::uint64_t std_test_sum(std::uint64_t a, std::uint64_t b)
{
    return a + b;
}

std::string string_test()
{
    std::string s1 = "Poook";
    return s1;
}
