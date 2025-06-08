#pragma once

#include <algorithm>
#include <cstring>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <utility>


namespace stdx::details {

// Шаблонный класс, хранящий C-style строку фиксированной длины
template <size_t StrSize>
struct FixedString {
    char data[StrSize]{};

    constexpr FixedString(const char (&other)[StrSize]) {
        std::copy_n(other, StrSize, data);
    }

    template <size_t OtherSize>
    constexpr FixedString(const char (&other)[OtherSize]) requires (OtherSize <= StrSize) {
        std::copy_n(other, OtherSize, data);
    }

    constexpr FixedString(const char* src_begin, const char* src_end) {
        auto distance = src_end - src_begin;
        std::copy_n(src_begin, distance, data);
    }

    static constexpr size_t size() {
        return StrSize - 1;
    }
};

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга
struct ParseError : public FixedString<50> {};

// Шаблонный класс для хранения результатов парсинга
template <typename... Ts>
struct scan_result {
    scan_result() = delete;

    constexpr scan_result(std::tuple<Ts...>&& other_tuple)
        : values_(std::forward<std::tuple<Ts...>>(other_tuple)) {};

    constexpr std::tuple<Ts...> values() const {
        return values_;
    }

private:
    std::tuple<Ts...> values_;
};

} // namespace stdx::details
