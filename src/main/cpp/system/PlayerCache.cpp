module;

#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <mutex>
#include <string_view>
#include <vector>

export module system.PlayerCache;

import entities.Player;

export class [[nodiscard]] PlayerCache final {
private:
  struct Implementation;
  struct ImplementationDeleter { void operator()(Implementation*) const;  };
  std::unique_ptr<Implementation, ImplementationDeleter>
  m_pImpl; // TODO warnings in visual studio 17.6.4 if ImplementationDeleter is missing

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

struct [[nodiscard]] PlayerCache::Implementation final {
  std::map<std::string, std::unique_ptr<Player>, std::less<>> m_players {};
  std::mutex m_mutex {};
  std::string m_siteName;

  Implementation(std::string_view siteName): m_siteName { siteName } {}
};

void PlayerCache::ImplementationDeleter::operator()(PlayerCache::Implementation* ptr) const { delete ptr; }

PlayerCache::PlayerCache(std::string_view siteName) noexcept : m_pImpl { new Implementation(siteName) } {}

void PlayerCache::setIsHero(std::string_view playerName) {
  const std::lock_guard lock { m_pImpl->m_mutex };
  auto it { m_pImpl->m_players.find(playerName) };
  assert((m_pImpl->m_players.end() != it) && "Setting hero on a bad player");
  it->second->setIsHero(true);
}

void PlayerCache::erase(std::string_view playerName) {
  const std::lock_guard lock { m_pImpl->m_mutex };
  auto it { m_pImpl->m_players.find(playerName) };
  assert((m_pImpl->m_players.end() != it) && "Erasing a bad player");
  m_pImpl->m_players.erase(it);
}

void PlayerCache::addIfMissing(std::string_view playerName) {
  const std::lock_guard lock { m_pImpl->m_mutex };
  m_pImpl->m_players.emplace(std::make_pair(playerName, std::make_unique<Player>(Player::Params{ .name = playerName, .site = m_pImpl->m_siteName })));
}

bool PlayerCache::isEmpty() {
  const std::lock_guard lock { m_pImpl->m_mutex };
  return m_pImpl->m_players.empty();
}

std::vector<std::unique_ptr<Player>> PlayerCache::extractPlayers() {
  const std::lock_guard lock { m_pImpl->m_mutex };
  std::vector<std::unique_ptr<Player>> ret;
  ret.reserve(m_pImpl->m_players.size());
  std::ranges::for_each(m_pImpl->m_players, [&](auto & nameToPlayer) { ret.push_back(std::move(nameToPlayer.second)); });
  m_pImpl->m_players.clear();
  return ret;
}