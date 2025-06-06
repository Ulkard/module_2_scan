#pragma once

#include <cstddef>
#include <expected>

#include "types.hpp"

namespace stdx::details {

template <template <typename> typename fixed_string>
class format_string {
    template <size_t Size>
    using PlaceholderPoses = std::array<std::pair<size_t, size_t>, Size>;

    // Функция для получения количества плейсхолдеров и проверки корректности формирующей строки
    static consteval std::expected<size_t, parse_error> get_number_placeholders() {
        constexpr size_t N = str.size();
        if (!N)
            return 0;
        size_t placeholder_count = 0;
        size_t pos = 0;
        const size_t size = N - 1; // -1 для игнорирования нуль-терминатора

        while (pos < size) {
            // Пропускаем все символы до '{'
            if (str.data[pos] != '{') {
                ++pos;
                continue;
            }

            // Проверяем незакрытый плейсхолдер
            if (pos + 1 >= size) {
                return std::unexpected(parse_error{"Unclosed last placeholder"});
            }

            // Начало плейсхолдера
            ++placeholder_count;
            ++pos;

            // Проверка спецификатора формата
            if (str.data[pos] == '%') {
                ++pos;
                if (pos >= size) {
                    return std::unexpected(parse_error{"Unclosed last placeholder"});
                }

                // Проверяем допустимые спецификаторы
                const char spec = str.data[pos];
                constexpr char valid_specs[] = {'d', 'u', 'f', 's'};
                bool valid = false;

                for (const char s : valid_specs) {
                    if (spec == s) {
                        valid = true;
                        break;
                    }
                }

                if (!valid) {
                    return std::unexpected(parse_error{"Invalid specifier."});
                }
                ++pos;
            }

            // Проверяем закрывающую скобку
            if (pos >= size || str.data[pos] != '}') {
                return std::unexpected(parse_error{"\'}\' hasn't been found in appropriate place"});
            }
            ++pos;
        }

        return placeholder_count;
    }

    // Функция для получения позиций плейсхолдеров

    template <size_t PhSize>
    static consteval PlaceholderPoses<PhSize> get_placeholder_positions() { 
        constexpr size_t N = str.size();
        PlaceholderPoses<PhSize> result;
        if (!N)
            return result;
        size_t current_idx = 0;
        size_t pos = 0;
        const size_t size = N - 1; // -1 для игнорирования нуль-терминатора


        while (pos < size) {
            // Пропускаем все символы до '{'
            if (str.data[pos] != '{') {
                ++pos;
                continue;
            }
        
            // Начало плейсхолдера
            size_t ph_begin = pos;
            ++pos;
            while (pos < size) {
                if (str.data[pos] == '}') {
                    result[current_idx] = {ph_begin, pos};
                    ++current_idx;
                }
                ++pos;
            }

            ++pos;
        }

        static_assert(current_idx == PhSize - 1, "unexpected PhSize");

        return result;
    }

    static fixed_string str;
    static constexpr size_t number_placeholders = [result = get_number_placeholders()](){
        return result.has_value() ? result.value() : 0;
    }();
    static constexpr PlaceholderPoses<number_placeholders> placeholder_positions = get_placeholder_positions();
};

// Пользовательский литерал

template <fixed_string FixedString>
constexpr auto operator"" _fs()  
{
    return FixedString;
}


} // namespace stdx::details
