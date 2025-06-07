#pragma once

#include <tuple>

#include "parse.hpp"
#include "format_string.hpp"
#include "types.hpp"
#include "iostream"

namespace stdx {

template <int I, details::format_string fmt, details::fixed_string source, typename ParsingT, typename... Ts>
consteval std::tuple<ParsingT, Ts...> scan_iter() {
    constexpr ParsingT iter_result = details::parse_input<I, fmt, source, ParsingT>();
    if constexpr (sizeof...(Ts) == 0) {
        return iter_result;
    } else {
        return std::tuple_cat(
            iter_result, 
            scan_iter<I + 1, fmt, source, Ts...>() 
        );
    }
}

// Главная функция
template <details::format_string fmt, details::fixed_string source, typename... Ts>
consteval details::scan_result<Ts...> scan() { // передайте пакет параметров в scan_result
    return details::scan_result<Ts...>{
        scan_iter<0, fmt, source, Ts...>()
    };
}

} // namespace stdx