#pragma once
#include "format_string.hpp"
#include "types.hpp"


consteval void testFixedString() {
    using namespace stdx::details;

    // c-str constructor
    FixedString fs_0("0123456789");
    static_assert(std::is_same<decltype(fs_0), FixedString<11>>(), "bad FixedString size()");
    static_assert(fs_0.size() == 10, "bad FixedString size()");

    FixedString fs_1("");
    static_assert(std::is_same<decltype(fs_1), FixedString<1>>(), "bad FixedString size()");
    static_assert(fs_1.size() == 0, "bad empty FixedString size()");

    // pair-of-pointers constructor
    constexpr char raw_string_0[11] = "0123456789";
    FixedString<11> fs_2(&raw_string_0[0], &raw_string_0[10]);
    static_assert(fs_2.size() == 10, "bad FixedString size()");

    constexpr char raw_string_1[1] = "";
    FixedString<1> fs_3(&raw_string_1[0], &raw_string_1[1]);
    static_assert(fs_3.size() == 0, "bad FixedString size()");
}

consteval void testFormatString() {
    using namespace stdx::details;
    
    // types
    static_assert(std::same_as<decltype("123"_fs), decltype("123"_fs)>);
    static_assert(!std::same_as<decltype("123"_fs), decltype("456"_fs)>);

    // placeholders
    constexpr auto fs_0 = "012 {} 789 {%u} 17 {%s}"_fs;
    static_assert(fs_0.number_placeholders == 3, "bad number_placeholders");
    static_assert(fs_0.format_specifiers.size() == 3, "bad format_specifiers.size()");
    static_assert(fs_0.format_specifiers[0] == char{} 
        && fs_0.format_specifiers[1] == 'u' 
        && fs_0.format_specifiers[2] == 's', "bad format_specifiers");
    static_assert(fs_0.placeholder_positions.size() == 3, "bad placeholder_positions.size()");
    static_assert(fs_0.placeholder_positions[0].first == 4
        && fs_0.placeholder_positions[0].second == 5
        && fs_0.placeholder_positions[1].first == 11
        && fs_0.placeholder_positions[1].second == 14
        && fs_0.placeholder_positions[2].first == 19
        && fs_0.placeholder_positions[2].second == 22, "bad placeholder_positions");
}