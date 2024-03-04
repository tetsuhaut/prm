module;

#include <algorithm> // std::find_if
#include <array>
#include <utility> // std::pair
#include <stdexcept> // std::range_error

export module language.Map;

export namespace language {
/**
 * This is a compile-time map. Use it as a constexpr variable.
 * The number of elements must be provided beside the elements themselves at the
 * Map instanciation.
 */
template <typename K, typename V, std::size_t Size>
struct Map {
  std::array<std::pair<K, V>, Size> data;

  [[nodiscard]] constexpr V at(const K& key) const {
    const auto itr { std::find_if(begin(data), end(data), [&key](const auto& v) { return v.first == key; }) };
    return (itr == end(data)) ? throw std::range_error("Not Found") : itr->second;
  }
};

template <typename K, typename V, std::size_t N>
constexpr auto make_Map(const std::pair<K, V> (&items)[N]) {
  return Map<K, V, N>{items};
}

template <typename K, typename V, std::size_t N>
constexpr auto make_Map(const std::array<std::pair<K, V>, N>& items) {
  return Map<K, V, N>{items};
}

} // export namespace language