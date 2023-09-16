module;

#include <array>
#include <iterator> // std::begin, std::end, std::back_inserter
#include <ranges>
#include <utility> // std::move

export module language.containers;

export namespace language::containers {
template<typename SOURCE, typename TARGET>
constexpr void moveInto(SOURCE& s, TARGET& t) noexcept { std::move(std::begin(s), std::end(s), std::back_inserter(t)); }

template<typename SOURCE>
constexpr bool containsIf(const SOURCE& s, auto predicate) { return std::end(s) != std::ranges::find_if(s, predicate); }

/**
 * @brief a map-like array
 * @tparam TYPE the type of the values. The keys are the array indices, so size_t.
 * @tparam N the number of key-value pairs in the map
*/
template<typename TYPE, size_t N>
class [[nodiscard]] FMap final {
private:
  std::array<TYPE, N> m_data;
public:
  constexpr TYPE get(size_t index) const { return m_data[index]; }
  void put(size_t index, TYPE value) { m_data[index] = value; }
  constexpr size_t getCapacity() const noexcept { return N; }
  constexpr bool contains(TYPE value) { return m_data.end() != std::ranges::find(m_data, value); }

  // makes FMap compatible with STL algorithms
  auto begin() noexcept { return m_data.begin(); }
  auto begin() const noexcept { return m_data.begin(); }
  auto end() noexcept { return m_data.end(); }
  auto end() const noexcept { return m_data.end(); }
  auto cbegin() noexcept { return m_data.cbegin(); }
  auto cbegin() const noexcept { return m_data.cbegin(); }
  auto cend() noexcept { return m_data.cend(); }
  auto cend() const noexcept { return m_data.cend(); }
  auto rbegin() noexcept { return m_data.rbegin(); }
  auto rbegin() const noexcept { return m_data.rbegin(); }
  auto rend() noexcept { return m_data.rend(); }
  auto rend() const noexcept { return m_data.rend(); }
}; // class FMap
} // namespace language::containers