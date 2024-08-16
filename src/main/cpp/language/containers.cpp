module;

export module language.containers;

import std;

export namespace language::containers {
template<typename SOURCE, typename TARGET>
constexpr void moveInto(SOURCE& s, TARGET& t) noexcept { std::move(std::begin(s), std::end(s), std::back_inserter(t)); }

template<typename SOURCE>
constexpr bool containsIf(const SOURCE& s, auto predicate) { return std::end(s) != std::ranges::find_if(s, predicate); }
}