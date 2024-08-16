module;

#include <cassert> // assert

export module system.PlayerCache;

import entities.Player;

import std;

export class [[nodiscard]] PlayerCache final {
private:
  std::map<std::string, std::unique_ptr<Player>, std::less<>> m_players {};
  std::mutex m_mutex {};
  std::string m_siteName;

public:
  PlayerCache(std::string_view siteName) noexcept;
  // non copyable
  PlayerCache(const PlayerCache&) = delete;
  PlayerCache(PlayerCache&&) = delete;
  PlayerCache& operator=(const PlayerCache&) = delete;
  PlayerCache& operator=(PlayerCache&&) = delete;
  ~PlayerCache() = default; // needed because PlayerCache owns an std::unique_ptr member
  void setIsHero(std::string_view playerName);
  void erase(std::string_view playerName);
  void addIfMissing(std::string_view playerName);
  [[nodiscard]] std::vector<std::unique_ptr<Player>> extractPlayers();
  [[nodiscard]] bool isEmpty();
}; // class PlayerCache

module : private;

PlayerCache::PlayerCache(std::string_view siteName) noexcept : m_siteName { siteName } {}

void PlayerCache::setIsHero(std::string_view playerName) {
  const std::lock_guard lock { m_mutex };
  auto it { m_players.find(playerName) };
  assert((m_players.end() != it) and "Setting hero on a bad player");
  it->second->setIsHero(true);
}

void PlayerCache::erase(std::string_view playerName) {
  const std::lock_guard lock { m_mutex };
  auto it { m_players.find(playerName) };
  assert((m_players.end() != it) and "Erasing a bad player");
  m_players.erase(it);
}

void PlayerCache::addIfMissing(std::string_view playerName) {
  const std::lock_guard lock { m_mutex };
  m_players.emplace(std::make_pair(playerName, std::make_unique<Player>(Player::Params{ .name = playerName, .site = m_siteName })));
}

bool PlayerCache::isEmpty() {
  const std::lock_guard lock { m_mutex };
  return m_players.empty();
}

std::vector<std::unique_ptr<Player>> PlayerCache::extractPlayers() {
  const std::lock_guard lock { m_mutex };
  std::vector<std::unique_ptr<Player>> ret;
  ret.reserve(m_players.size());
  std::ranges::for_each(m_players, [&](auto & nameToPlayer) { ret.push_back(std::move(nameToPlayer.second)); });
  m_players.clear();
  return ret;
}