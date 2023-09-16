//#pragma once
//#include "strings/StringView.hpp"
//#include <algorithm> // std::copy_n
//
///**
// * A compile time string type for template parameters.
// * inspired by https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/
// */
//template<typename CHAR, auto N>
//struct [[nodiscard]] StringLiteral final {
//  CHAR value[N + 1] {};
//  static constexpr auto size { N };
//
//  constexpr explicit(false) StringLiteral(const char (&str)[N + 1]) { std::copy_n(str, N, value); }
//
//  constexpr explicit(false) operator StringView() const { return {value, N}; }
//
//  constexpr StringLiteral() {}
//
//  constexpr StringLiteral(std::basic_string_view<CHAR> str) {
//    std::copy_n(str, str.size() + 1, value);
//  }
//};
//
//template<typename CHAR, auto N> StringLiteral(const CHAR(&)[N]) -> StringLiteral < CHAR, N - 1 >;
//
//
///*
// * Concatenates strings at compile time. Can be used as a template argument to concatenate template
// * StringLiteral arguments
// * @returns a StringLiteral containing the concatenation
// */
//template<typename CHAR, std::size_t ...N>
//[[nodiscard]] constexpr auto concatLiteral(const CHAR(&...strings)[N]) {
//  constexpr auto len = (... + N) - sizeof...(N);
//  StringLiteral < CHAR, len + 1 > result {};
//  result.value[len] = '\0';
//  CHAR* dst { result.value };
//
//  for (const CHAR* src : { strings... }) {
//    for (; *src != '\0'; src++, dst++) {
//      *dst = *src;
//    }
//  }
//  return result;
//}