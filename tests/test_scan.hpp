#pragma once
#include <concepts>
#include <string_view>
#include <tuple>
#include "scan.hpp"

consteval void testScan() {
    using namespace stdx::details;
    constexpr auto test_0 = stdx::scan<
        "my test string is {}, right?"_fs, 
        "my test string is test_str, right?", 
        std::string_view>().values();
    static_assert(std::tuple_size<decltype(test_0)>() == 1, "bad scan().values() size");
    static_assert(std::is_same<decltype(std::get<0>(test_0)), const std::string_view&>(), "bad scan().values() type");
    static_assert(std::get<0>(test_0) == "test_str", "bad scan() result");

    constexpr auto test_1 = stdx::scan<
        "my test int is {}, right?"_fs, 
        "my test int is 42, right?", 
        int>().values();
    static_assert(std::tuple_size<decltype(test_1)>() == 1, "bad scan().values() size");
    static_assert(std::is_same<decltype(std::get<0>(test_1)), const int&>(), "bad scan().values() type");
    static_assert(std::get<0>(test_1) == 42, "bad scan() result");
    
   constexpr auto test_2 = stdx::scan<
        "I want to sum {%d} and {%s} numbers."_fs, 
        "I want to sum 42 and 12 numbers.", 
        int, std::string_view>().values();
    static_assert(std::tuple_size<decltype(test_2)>() == 2, "bad scan().values() size");
    static_assert(std::is_same<decltype(std::get<0>(test_2)), const int&>(), "bad scan().values() type");
    static_assert(std::get<0>(test_2) == 42, "bad scan() result");

    // number as string - ок
    static_assert(std::is_same<decltype(std::get<1>(test_2)), const std::string_view&>(), "bad scan().values() type");
    static_assert(std::get<1>(test_2) == "12", "bad scan() result");

    // string as number - shouldn't work
    constexpr auto test_3 = stdx::scan<
        "my test string is {}, right?"_fs, 
        "my test string is test_str, right?", 
        int>().values();

    // string as number with format specifier - shouldn't work
    constexpr auto test_4 = stdx::scan<
        "my test string is {%d}, right?"_fs, 
        "my test string is test_str, right?", 
        int>().values();
}