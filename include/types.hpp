#pragma once

#include <algorithm>
#include <cstring>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <utility>
namespace stdx::details {

// Шаблонный класс, хранящий C-style строку фиксированной длины
template <size_t ArrSize>
struct fixed_string {
    char data[ArrSize];

    constexpr fixed_string(const char other[ArrSize]) {
        std::strcpy(data, other);
    }

    template <size_t OtherSize>
    constexpr fixed_string(const char other[OtherSize]) requires (OtherSize <= ArrSize) {
        std::strcpy(data, other);;
    }

    constexpr fixed_string(char* src_begin, char* src_end) requires (src_begin <= src_end) {
        auto distance = src_end - src_begin;
        std::copy(src_begin, src_end + 1, &data, &data + distance);
    }

    constexpr size_t size() {
        return ArrSize;
    }
};

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга
struct parse_error : public fixed_string<40> {};

// Шаблонный класс для хранения результатов парсинга
template <typename... Ts>
struct scan_result {
    scan_result() = delete;

    scan_result(std::tuple<Ts...>&& other_tuple)
        : values_(std::forward(other_tuple)) {};

    scan_result(Ts&& ...args)
        : values_(std::make_tuple(std::forward(args)...)) {};
    std::tuple<Ts...> values() {
        return values_;
    }

private:
    std::tuple<Ts...> values_;
};

} // namespace stdx::details
