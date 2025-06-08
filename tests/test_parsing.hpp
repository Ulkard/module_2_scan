#pragma once
#include "parse.hpp"
#include "types.hpp"
#include <cstdint>
#include <string_view>

consteval void testParsingSource() {
    using namespace stdx::details;
    constexpr auto fmt =        "012 {} 678 {%u} 14 {%s}."_fs;
    constexpr FixedString str = "012 4 678 11 14 test_2.";
    constexpr auto result_0 = get_current_source_for_parsing<0, fmt, str>();
    static_assert (result_0.first == 4, "bad parsing_source.first");
    static_assert (result_0.second == 5, "bad parsing_source.second");

    constexpr auto result_1 = get_current_source_for_parsing<1, fmt, str>();
    static_assert (result_1.first == 10, "bad parsing_source.first");
    static_assert (result_1.second == 12, "bad parsing_source.second");

    constexpr auto result_2 = get_current_source_for_parsing<2, fmt, str>();
    static_assert (result_2.first == 16, "bad parsing_source.first");
    static_assert (result_2.second == 22, "bad parsing_source.second");
}

consteval void testParseValue() {
    using namespace stdx::details;
    constexpr FixedString int_str("-123");
    constexpr int32_t int_result = parse_value<int_str, char{}, int32_t>();
    static_assert(int_result == -123, "parse_value<int, ..> failed");

    constexpr FixedString uint_str("456");
    constexpr uint32_t uint_result = parse_value<uint_str, char{}, uint32_t>();
    static_assert(uint_result == 456, "parse_value<uint, ..> failed");

    constexpr FixedString str_str("test_str");
    constexpr std::string_view str_result = parse_value<str_str, char{}, std::string_view>();
    static_assert(str_result == "test_str", "parse_value<int, ..> failed");
}

consteval void testParseInput() {
    using namespace stdx::details;
    constexpr auto fmt =        "012 {} 678 {%u} 14 {%s}."_fs;
    constexpr FixedString str = "012 4 678 11 14 test_2.";

    constexpr int32_t result_0 = parse_input<0, fmt, str, int32_t>();
    static_assert(result_0 == 4, "parse_value<int, ..> failed");

    constexpr uint32_t result_1 = parse_input<1, fmt, str, uint32_t>();
    static_assert(result_1 == 11, "parse_value<int, ..> failed");

    constexpr std::string_view result_2 = parse_input<2, fmt, str, std::string_view>();
    static_assert(result_2 == "test_2", "parse_value<int, ..> failed");
}