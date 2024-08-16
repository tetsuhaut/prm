module;

export module language.Map;

import std;

export namespace language {
/**
 * This is a compile-time map. Use it as a constexpr variable.
 * The number of elements must be provided beside the elements themselves at the
 * Map instanciation.
 */
template <typename K, typename V, std::size_t Size>
struct Map {
  std::array<std::pair<K, V>, Size> m_data;

  [[nodiscard]] constexpr V at(const K& key) const {
    const auto itr { std::find_if(begin(m_data), end(m_data), [&key](const auto& v) { return v.first == key; }) };
    return (itr == end(m_data)) ? throw std::range_error("Key Not Found in Map") : itr->second;
  }
}; // struct Map

template<typename K, typename V, typename ... Args>
[[nodiscard]] constexpr Map< K, V, sizeof...(Args)> makeMap(Args&& ...args) {
  return { { std::forward<Args>(args)... } };
}

} // export namespace language