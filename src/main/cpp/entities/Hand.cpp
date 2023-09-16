module;

#include <algorithm> // std::range
#include <array> // std::array
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

export module entities.Hand;

import entities.Action;
import entities.Card;
import entities.GameType;
import entities.Seat;
import system.Time;

export class [[nodiscard]] Hand final {
private:
  std::string m_id;
  GameType m_gameType;
  std::string m_siteName;
  std::string m_tableName;
  Seat m_buttonSeat;
  Seat m_maxSeats;
  int m_level;
  long m_ante;
  Time m_date;
  std::array<Card, 5> m_heroCards;
  std::array<Card, 5> m_boardCards;
  std::unordered_map<Seat, std::string> m_seats;
  std::vector<std::unique_ptr<Action>> m_actions;
  std::array<std::string, 10> m_winners;

public:
  struct [[nodiscard]] Params final {
    std::string_view id;
    GameType gameType;
    std::string_view siteName;
    std::string_view tableName;
    Seat buttonSeat;
    Seat maxSeats;
    int level;
    long ante;
    const Time& startDate;
    const std::unordered_map<Seat, std::string>& seatPlayers;
    const std::array<Card, 5>& heroCards;
    const std::array<Card, 5>& boardCards;
    std::vector<std::unique_ptr<Action>> actions;
    const std::array<std::string, 10>& winners;
  }; // struct Params

  explicit Hand(Params& p)
    : m_id { p.id },
      m_gameType { p.gameType },
      m_siteName { p.siteName },
      m_tableName { p.tableName },
      m_buttonSeat { p.buttonSeat },
      m_maxSeats { p.maxSeats },
      m_level { p.level },
      m_ante { p.ante },
      m_date { p.startDate },
      m_heroCards { p.heroCards },
      m_boardCards { p.boardCards },
      m_seats { p.seatPlayers },
      m_actions { std::move(p.actions) },
      m_winners { p.winners } {
    assert(!m_id.empty() && "id is empty");
    assert(!m_siteName.empty() && "site is empty");
    assert(!m_tableName.empty() && "table is empty");
    assert(m_ante >= 0 && "ante is negative");
    assert(m_seats.size() > 1 && m_seats.size() < 11);
  }

  Hand(const Hand&) = delete;
  Hand(Hand&&) = delete;
  Hand& operator=(const Hand&) = delete;
  Hand& operator=(Hand&&) = delete;
  ~Hand() = default;
  [[nodiscard]] std::vector<const Action*> viewActions() const;
  [[nodiscard]] std::string getId() const noexcept { return m_id; }
  [[nodiscard]] GameType getGameType() const noexcept { return m_gameType; }
  [[nodiscard]] std::string getSiteName() const noexcept { return m_siteName; }
  [[nodiscard]] std::string getTableName() const noexcept { return m_tableName; }
  [[nodiscard]] std::unordered_map<Seat, std::string> getSeats() const noexcept { return m_seats; }
  [[nodiscard]] Seat getButtonSeat() const noexcept { return m_buttonSeat; }
  [[nodiscard]] Seat getMaxSeats() const noexcept { return m_maxSeats; }
  [[nodiscard]] int getLevel()const noexcept { return m_level; }
  [[nodiscard]] long getAnte() const noexcept { return m_ante; }
  [[nodiscard]] Time getStartDate() const noexcept { return m_date; }
  [[nodiscard]] bool isPlayerInvolved(std::string_view name) const;
  [[nodiscard]] Card getHeroCard1() const { return m_heroCards.at(0); }
  [[nodiscard]] Card getHeroCard2() const { return m_heroCards.at(1); }
  [[nodiscard]] Card getHeroCard3() const { return m_heroCards.at(2); }
  [[nodiscard]] Card getHeroCard4() const { return m_heroCards.at(3); }
  [[nodiscard]] Card getHeroCard5() const { return m_heroCards.at(4); }
  [[nodiscard]] Card getBoardCard1() const { return m_boardCards.at(0); }
  [[nodiscard]] Card getBoardCard2() const { return m_boardCards.at(1); }
  [[nodiscard]] Card getBoardCard3() const { return m_boardCards.at(2); }
  [[nodiscard]] Card getBoardCard4() const { return m_boardCards.at(3); }
  [[nodiscard]] Card getBoardCard5() const { return m_boardCards.at(4); }
  [[nodiscard]] bool isWinner(std::string_view playerName) const noexcept {
    return m_winners.end() != std::ranges::find(m_winners, playerName.data());
  }
}; // class Hand

module : private;

template<template<typename, typename> typename SmartPointer, typename T, typename U>
[[nodiscard]] std::vector<const T*> mkView(const std::vector<SmartPointer<T, U>>& v) {
  std::vector<const T*> ret;
  ret.reserve(v.size());

  // here we use a 'for' loop to avoid depending on <algorithm>
  for (const auto& e : v) { ret.push_back(e.get()); }

  return ret;
}

std::vector<const Action*> Hand::viewActions() const { return mkView(m_actions); }

bool Hand::isPlayerInvolved(std::string_view name) const {
  return m_actions.end() != std::ranges::find_if(m_actions,
  [&name](const auto & a) { return name == a->getPlayerName(); });
}