#pragma once

#include <charconv>
#include <concepts>
#include <cstdint>
#include <optional>
#include <string>
#include <system_error>

#include "format_string.hpp"
#include "types.hpp"

namespace stdx::details {

// Шаблонная функция, возвращающая пару позиций в строке с исходными данными, соотвествующих I-ому плейсхолдеру
template<int I, format_string fmt, fixed_string source>
consteval auto get_current_source_for_parsing() {
    static_assert(I >= 0 && I < fmt.number_placeholders, "Invalid placeholder index");

    constexpr auto to_sv = [](const auto& fs) {
        return std::string_view(fs.data, fs.size() - 1);
    };

    constexpr auto fmt_sv = to_sv(fmt.fmt);
    constexpr auto src_sv = to_sv(source);
    constexpr auto& positions = fmt.placeholder_positions;

    // Получаем границы текущего плейсхолдера в формате
    constexpr auto pos_i = positions[I];
    constexpr size_t fmt_start = pos_i.first, fmt_end = pos_i.second;

    // Находим начало в исходной строке
    constexpr auto src_start = [&]{
        if constexpr (I == 0) {
            return fmt_start;
        } else {
            // Находим конец предыдущего плейсхолдера в исходной строке
            constexpr auto prev_bounds = get_current_source_for_parsing<I-1, fmt, source>();
            const auto prev_end = prev_bounds.second;

            // Получаем разделитель между текущим и предыдущим плейсхолдерами
            constexpr auto prev_fmt_end = positions[I-1].second;
            constexpr auto sep = fmt_sv.substr(prev_fmt_end + 1, fmt_start - (prev_fmt_end + 1));

            // Ищем разделитель после предыдущего значения
            auto pos = src_sv.find(sep, prev_end);
            return pos != std::string_view::npos ? pos + sep.size() : src_sv.size();
        }
    }();

    // Находим конец в исходной строке
    constexpr auto src_end = [&]{
        // Получаем разделитель после текущего плейсхолдера
        if constexpr(fmt_end == (fmt_sv.size() - 1)) {
            return src_sv.size();
        }
        constexpr auto sep = fmt_sv.substr(fmt_end + 1,
            (I < fmt.number_placeholders - 1)
                ? positions[I+1].first - (fmt_end + 1)
                : fmt_sv.size() - (fmt_end + 1));
        // Ищем разделитель после текущего значения
        constexpr auto pos = src_sv.find(sep, src_start);
        return pos != std::string_view::npos ? pos : src_sv.size();
    }();
    return std::pair{src_start, src_end};
}


template <typename T, typename... Types>
concept IsTypeOnOf = (std::same_as<T, Types> || ...);

// Реализуйте семейство функций parse_value
template<fixed_string str, char format_spec, typename ParsingT>
requires IsTypeOnOf<ParsingT, int8_t, int16_t, int32_t, int64_t, const int8_t, const int16_t, const int32_t, const int64_t>
    && (format_spec == 'd' || format_spec == char{})
consteval ParsingT parse_value() {
    return std::stoi(str);
}

template<fixed_string str, char format_spec, typename ParsingT>
requires IsTypeOnOf<ParsingT, uint8_t, uint16_t, uint32_t, uint64_t, const uint8_t, const uint16_t, const uint32_t, const uint64_t>
    && (format_spec == 'u' || format_spec == char{})
consteval ParsingT parse_value() {
    return std::stoul(str);
}

template<fixed_string str, char format_spec, typename ParsingT>
requires IsTypeOnOf<ParsingT, std::string_view, const std::string_view>
    && (format_spec == 's' || format_spec == char{})
consteval ParsingT parse_value() {
    return str;
}

template<fixed_string str, char format_spec, typename ParsingT>
consteval ParsingT parse_value() {
    // emit error somehow
    // parse_error("parsing type mismatch format specifier")
    return {};
}

// Шаблонная функция, выполняющая преобразования исходных данных в конкретный тип на основе I-го плейсхолдера
template<int I, format_string fmt, fixed_string source, typename ParsingT>
consteval ParsingT parse_input() { 
    constexpr auto target_idxs = get_current_source_for_parsing<I, fmt, source>();
    constexpr fixed_string str_to_parse = {target_idxs};
    return parse_value<str_to_parse, fmt.format_specifiers[I], ParsingT>();
}

} // namespace stdx::details
