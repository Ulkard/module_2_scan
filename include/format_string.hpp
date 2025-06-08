#pragma once

#include <cstddef>
#include <expected>
#include <array>

#include "types.hpp"

namespace stdx::details {

template <FixedString str>
class FormatString {
    template <size_t Size>
    using PlaceholderPoses = std::array<std::pair<size_t, size_t>, Size>;

    // Функция для получения количества плейсхолдеров и проверки корректности формирующей строки
    static consteval std::expected<size_t, ParseError> get_number_placeholders() {
        constexpr size_t size = str.size();
        if (!size)
            return 0;
        size_t placeholder_count = 0;
        size_t pos = 0;

        while (pos < size) {
            // Пропускаем все символы до '{'
            if (str.data[pos] != '{') {
                ++pos;
                continue;
            }

            // Проверяем незакрытый плейсхолдер
            if (pos + 1 >= size) {
                return std::unexpected(ParseError{"Unclosed last placeholder"});
            }

            // Начало плейсхолдера
            ++placeholder_count;
            ++pos;

            // Проверка спецификатора формата
            if (str.data[pos] == '%') {
                ++pos;
                if (pos >= size) {
                    return std::unexpected(ParseError{"Unclosed last placeholder"});
                }

                // Проверяем допустимые спецификаторы
                const char spec = str.data[pos];
                constexpr char valid_specs[] = {'d', 'u', 's'};
                bool valid = false;

                for (const char s : valid_specs) {
                    if (spec == s) {
                        valid = true;
                        break;
                    }
                }

                if (!valid) {
                    return std::unexpected(ParseError{"Invalid specifier."});
                }
                ++pos;
            }

            // Проверяем закрывающую скобку
            if (pos >= size || str.data[pos] != '}') {
                return std::unexpected(ParseError{"\'}\' hasn't been found in appropriate place"});
            }
            ++pos;
        }

        return placeholder_count;
    }

    // Функция для получения позиций плейсхолдеров
    template <size_t PhSize>
    static consteval PlaceholderPoses<PhSize> get_placeholder_positions() { 
        constexpr size_t size = str.size();
        PlaceholderPoses<PhSize> result;
        if (!size)
            return result;
        size_t current_idx = 0;
        size_t pos = 0;

        while (pos < size) {
            // Пропускаем все символы до '{'
            if (str.data[pos] != '{') {
                ++pos;
                continue;
            }
            // Начало плейсхолдера
            size_t ph_begin = pos;
            ++pos;

            if (str.data[pos] == '%') {
                ++pos;
                const char spec = str.data[pos];
                // TODO: find another way to fill format_specifiers
                //format_specifiers[current_idx] = spec;
            }
        
            while (pos < size) {
                if (str.data[pos] == '}') {
                    result[current_idx] = {ph_begin, pos};
                    ++current_idx;
                    break;
                }
                ++pos;
            }

            ++pos;
        }
        
        return result;
    }

    static consteval size_t get_valid_number_placeholders() {
        constexpr auto raw_result = get_number_placeholders();
        if constexpr (raw_result.has_value()) {
            return raw_result.value();
        } else {
            static_assert(false, "get_number_placeholders() failed");
        }
    }
    

public:
    static constexpr const decltype(str.data)& data() {
        return str.data;
    }
    static constexpr size_t size() {
        return str.size();
    }
    static constexpr size_t number_placeholders = get_valid_number_placeholders();

    static constexpr std::array<char, number_placeholders> format_specifiers{};
    static constexpr PlaceholderPoses<number_placeholders> placeholder_positions = get_placeholder_positions<number_placeholders>();
};

// Пользовательский литерал
template <FixedString str>
constexpr auto operator"" _fs()  
{
    return FormatString<str>{};
}


} // namespace stdx::details
