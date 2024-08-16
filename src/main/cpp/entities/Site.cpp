module;

#include <cassert> // assert

export module entities.Site;

import entities.Game; // CashGame, Tournament
import entities.Player;
import language.containers;

import std;

/**
 * A Poker site, i.e. a bunch of hands played on different games, that enables us to build
 * statistics on encountered players behavior.
 */
export class [[nodiscard]] Site final {
private:
  std::string m_name;
  std::string m_heroName;
  std::unordered_map<std::string, std::unique_ptr<Player>> m_players {};
  std::vector<std::unique_ptr<CashGame>> m_cashGames {};
  std::vector<std::unique_ptr<Tournament>> m_tournaments {};

public:
  explicit Site(std::string_view name);
  Site(const Site&) = delete;
  Site(Site&&) = delete;
  Site& operator=(const Site&) = delete;
  Site& operator=(Site&&) = delete;
  ~Site() = default; // needed because Site owns std::unique_ptr members
  void addPlayer(std::unique_ptr<Player> p);
  void addGame(std::unique_ptr<CashGame> game);
  [[nodiscard]] std::vector<const CashGame*> viewCashGames() const;
  void addGame(std::unique_ptr<Tournament> game);
  [[nodiscard]] std::vector<const Tournament*> viewTournaments() const;
  [[nodiscard]] /*constexpr*/ std::string getName() const noexcept;
  [[nodiscard]] std::vector<const Player*> viewPlayers() const;
  [[nodiscard]] const Player* viewPlayer(std::string_view name) const;
  void merge(Site& other);
  [[nodiscard]] std::string whoIsHero() const noexcept;
}; // class Site

module : private;

Site::Site(std::string_view name) : m_name { name } { assert(!m_name.empty() and "name is empty"); }

void Site::addPlayer(std::unique_ptr<Player> p) {
  assert(p->getSiteName() == m_name and "player is on another site");

  if (!m_players.contains(p->getName())) {
    if (p->isHero()) { m_heroName = p->getName(); }

    m_players[p->getName()] = std::move(p);
  }
}

void Site::addGame(std::unique_ptr<CashGame> game) {
  assert(game->getSiteName() == m_name and "game is on another site");
  m_cashGames.push_back(std::move(game));
}

template<template<typename, typename> typename SmartPointer, typename T, typename U>
[[nodiscard]] inline std::vector<const T*> mkView(const std::vector<SmartPointer<T, U>>& v) {
  std::vector<const T*> ret;
  ret.reserve(v.size());

  // here we use a 'for' loop to avoid depending on <algorithm>
  for (const auto& e : v) { ret.push_back(e.get()); }

  return ret;
}


[[nodiscard]] std::vector<const CashGame*> Site::viewCashGames() const { return mkView(m_cashGames); }

void Site::addGame(std::unique_ptr<Tournament> game) {
  assert(game->getSiteName() == m_name and "game is on another site");
  m_tournaments.push_back(std::move(game));
}

[[nodiscard]] std::vector<const Tournament*> Site::viewTournaments() const { return mkView(m_tournaments); }

[[nodiscard]] /*constexpr*/ std::string Site::getName() const noexcept { return m_name; }

[[nodiscard]] std::vector<const Player*> Site::viewPlayers() const {
  std::vector<const Player*> ret;
  ret.reserve(m_players.size());
  std::ranges::transform(m_players, std::back_inserter(ret),
  [](const auto & entry) noexcept { return entry.second.get(); });
  return ret;
}

[[nodiscard]] const Player* Site::viewPlayer(std::string_view name) const {
  const auto& p { m_players.find(std::string(name)) };
  return m_players.end() == p ? nullptr : p->second.get();
}

void Site::merge(Site& other) {
  assert(other.getName() == m_name and "Can't merge data from different poker sites");
  std::ranges::for_each(other.m_players, [this](auto & pair) { addPlayer(std::move(pair.second)); });
  language::containers::moveInto(other.m_cashGames, m_cashGames);
  language::containers::moveInto(other.m_tournaments, m_tournaments);
}

[[nodiscard]] std::string Site::whoIsHero() const noexcept { return m_heroName; }
