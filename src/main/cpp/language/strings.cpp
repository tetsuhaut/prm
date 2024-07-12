module;

#include <cassert>
#include <charconv> // std::from_chars
#include <cstring> // std::size_t
#include <format>
#include <gsl/gsl>
#include <iterator> // std::distance
#include <numeric> // std::accumulate
#include <ranges>
#include <string_view>
#include <vector>

export module language.strings;

export namespace language::strings {
/**
 * returns the number of char in a char*, not counting the last '\0' char.
 */
template <typename CHAR, std::size_t SIZE>
[[nodiscard]] constexpr std::size_t length(const CHAR(&)[SIZE]) noexcept { return SIZE - 1; }

[[nodiscard]] std::string_view myTrim(std::string_view s);

/**
 * @return toBeSanitized with " replaced by '
*/
[[nodiscard]] std::string sanitize(std::string_view toBeSanitized);

[[nodiscard]] constexpr bool isSpace(char c) noexcept {
  return c == ' ' or c == '\f' or c == '\n' or c == '\r' or c == '\t' or c == '\v';
}

[[nodiscard]] constexpr bool contains(std::string_view s,
                                      std::string_view searched, size_t position) noexcept {
  return std::string_view::npos != s.find(searched, position);
}

[[nodiscard]] double toBuyIn(std::string_view buyIn);

[[nodiscard]] double toAmount(std::string_view amount);

[[nodiscard]] int toInt(std::string_view numberStr);

[[nodiscard]] size_t toSizeT(std::string_view numberStr);

[[nodiscard]] std::vector<std::string> split(std::string_view toBeSplitted, const char delimiter);

[[nodiscard]] std::string join(const std::vector<std::string>& toBeJoined, const char delimiter);
} // namespace language::strings

module : private;

[[nodiscard]]  std::string_view language::strings::myTrim(std::string_view s) {
  s.remove_prefix(gsl::narrow_cast<std::string_view::size_type>(std::distance(s.cbegin(),
                  std::find_if_not(s.cbegin(), s.cend(), isSpace))));
  s.remove_suffix(gsl::narrow_cast<std::string_view::size_type>(std::distance(s.crbegin(),
                  std::find_if_not(s.crbegin(), s.crend(), isSpace))));
  return s;
}

[[nodiscard]] std::string language::strings::sanitize(std::string_view toBeSanitized) {
  std::string ret { myTrim(toBeSanitized) };
  std::ranges::replace(ret, '\'', '-');
  return ret;
}

[[nodiscard]] double toDouble(std::string_view amount) {
  const auto str { language::strings::myTrim(amount) };
  double ret { 0 };
#if defined(_MSC_VER) // std::from_chars is not available for double on gcc 11.2.0 TODO est-ce tjrs d'actualite
  std::from_chars(str.data(), str.data() + amount.size(), ret);
#else
#include <stdio.h>

  try { ret = std::stod(String(str)); }
  catch (const std::invalid_argument&) { return 0.0; } // silent error

#endif
  return ret;
}

[[nodiscard]] double language::strings::toBuyIn(std::string_view buyIn) {
  std::string token;
  token.reserve(buyIn.size());
  double ret { 0.0 };
  std::ranges::for_each(buyIn, [&token, &ret](auto c) {
    switch (c) {
      case ',': { token += '.'; } break;

      case '.': [[fallthrough]];

      case '0': [[fallthrough]];

      case '1': [[fallthrough]];

      case '2': [[fallthrough]];

      case '3': [[fallthrough]];

      case '4': [[fallthrough]];

      case '5': [[fallthrough]];

      case '6': [[fallthrough]];

      case '7': [[fallthrough]];

      case '8': [[fallthrough]];

      case '9': { token.push_back(c); } break;

      case '+': { ret += toDouble(token); token.clear(); } break;

      default: break;
    }
  });

  if (!token.empty()) { ret += toDouble(token); }

  return ret;
}

[[nodiscard]] double language::strings::toAmount(std::string_view amount) {
  // because of default US locale, the decimal separator must be '.'
  std::string ret { amount };
  std::ranges::replace(ret, ',', '.');
  return toDouble(ret);
}

template<typename TYPE>
static inline TYPE toType(std::string_view s) {
  const auto str { language::strings::myTrim(s) };
  TYPE ret { 0 };
  const auto [ptr, errorCode] { std::from_chars(str.data(), str.data() + s.size(), ret) };

  if (std::errc() == errorCode) {
    return ret;
  }

  if (std::errc::invalid_argument == errorCode) {
    assert("Not a number");
  }

  if (std::errc::result_out_of_range == errorCode) {
    assert("This number is larger than the returned type");
  }

  assert("errorCode est inconnu !!!");
  return ret;
}

[[nodiscard]] int language::strings::toInt(std::string_view numberStr) {
  return toType<int>(numberStr);
}

[[nodiscard]] size_t language::strings::toSizeT(std::string_view numberStr) {
  return toType<size_t>(numberStr);
}

[[nodiscard]] std::vector<std::string> language::strings::split(std::string_view toBeSplitted, const char delimiter) {
  std::vector<std::string> ret;
  std::string token;
  for (auto c : toBeSplitted) {
    if (delimiter == c) {
      ret.push_back(token);
      token.clear();
    } else {
      token.push_back(c);
    }
  }
  if (!token.empty()) {
    ret.push_back(token);
  }
  return ret;
}


[[nodiscard]] std::string language::strings::join(const std::vector<std::string>& toBeJoined, const char delimiter) {
  const auto joiner { [&delimiter](std::string_view a, std::string_view b) { return std::format("{} {} {}", a, delimiter, b); } };
  return toBeJoined.empty() ? "" : std::accumulate(std::next(toBeJoined.begin()), toBeJoined.end(), toBeJoined[0], joiner);
}