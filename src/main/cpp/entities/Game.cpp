module;

#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

export module entities.Game;

import entities.Hand;
import entities.Seat;
import system.Time;
/**
 * Game variants that phud understands.
 */
export enum class /*[[nodiscard]]*/ Variant : short {
  none, holdem, omaha, omaha5
};

/**
 * Game limits that phud understands.
 */
export enum class /*[[nodiscard]]*/ Limit : short {
  none, noLimit, potLimit
};

/**
 * A game.
 */
export class [[nodiscard]] Game {
private:
  std::string m_id;
  std::string m_site;
  std::string m_name;
  Variant m_variant;
  Limit m_limitType;
  bool m_isRealMoney;
  Seat m_nbMaxSeats;
  Time m_startDate;
  std::vector<std::unique_ptr<Hand>> m_hands;

public:

  struct [[nodiscard]] Params final {
    std::string_view id;
    std::string_view siteName;
    std::string_view gameName;
    Variant variant;
    Limit limitType;
    bool isRealMoney;
    Seat nbMaxSeats;
    const Time& startDate;
  };

  Game(const Params& args)
    : m_id { args.id },
      m_site { args.siteName },
      m_name { args.gameName },
      m_variant { args.variant },
      m_limitType { args.limitType },
      m_isRealMoney { args.isRealMoney },
      m_nbMaxSeats { args.nbMaxSeats },
      m_startDate { args.startDate },
      m_hands {} {
    assert(!m_id.empty());
    assert(!m_site.empty());
    assert(!m_name.empty());
    assert(m_nbMaxSeats != Seat::seatUnknown && "nbMaxSeats must be defined");
  }
  Game(const Game&) = delete;
  Game(Game&&) = delete;
  Game& operator=(const Game&) = delete;
  Game& operator=(Game&&) = delete;
  ~Game() = default;

  void addHand(std::unique_ptr<Hand> hand) { m_hands.push_back(std::move(hand)); }

  // gcc 10.2 can't do constexpr std::string
  [[nodiscard]] /*constexpr*/ std::string getName() const noexcept { return m_name; }
  [[nodiscard]] constexpr bool isRealMoney() const noexcept { return m_isRealMoney; }
  [[nodiscard]] Time getStartDate() const noexcept { return m_startDate; }
  [[nodiscard]] std::vector<const Hand*> viewHands() const;
  [[nodiscard]] std::vector<const Hand*> viewHands(std::string_view player) const;
  [[nodiscard]] /*constexpr*/ std::string getSiteName() const noexcept { return m_site; }
  [[nodiscard]] /*constexpr*/ std::string getId() const noexcept { return m_id; }
  [[nodiscard]] constexpr Variant getVariant() const noexcept { return m_variant; }
  [[nodiscard]] constexpr Limit getLimitType() const noexcept { return m_limitType; }
  [[nodiscard]] constexpr Seat getMaxNbSeats() const noexcept { return m_nbMaxSeats; }
}; // class Game

export class [[nodiscard]] Tournament final : public Game {
private:
  double m_buyIn;

public:

  struct [[nodiscard]] Params final {
    std::string_view id;
    std::string_view siteName;
    std::string_view tournamentName;
    Variant variant;
    Limit limit;
    bool isRealMoney;
    Seat nbMaxSeats;
    double buyIn;
    const Time& startDate;
  };

  explicit Tournament(const Params& p) :
    Game(Game::Params { .id = p.id, .siteName = p.siteName, .gameName = p.tournamentName,
                        .variant = p.variant, .limitType = p.limit, .isRealMoney = p.isRealMoney,
                        .nbMaxSeats = p.nbMaxSeats, .startDate = p.startDate }),
    m_buyIn { p.buyIn } {
    assert(m_buyIn >= 0 && "negative buyIn");
  }
  Tournament(const Tournament&) = delete;
  Tournament(Tournament&&) = delete;
  Tournament& operator=(const Tournament&) = delete;
  Tournament& operator=(Tournament&&) = delete;
  ~Tournament() = default;
  [[nodiscard]] constexpr double getBuyIn() const noexcept { return m_buyIn; }
}; // class Tournament

export class [[nodiscard]] CashGame final : public Game {
private:
  double m_smallBlind;
  double m_bigBlind;

public:

  struct [[nodiscard]] Params final {
    std::string_view id;
    std::string_view siteName;
    std::string_view cashGameName;
    Variant variant;
    Limit limit;
    bool isRealMoney;
    Seat nbMaxSeats;
    double smallBlind;
    double bigBlind;
    const Time& startDate;
  };

  explicit CashGame(const Params& p) :
    Game(Game::Params { .id = p.id, .siteName = p.siteName, .gameName = p.cashGameName,
                        .variant = p.variant, .limitType = p.limit, .isRealMoney = p.isRealMoney,
                        .nbMaxSeats = p.nbMaxSeats, .startDate = p.startDate }),
    m_smallBlind { p.smallBlind },
    m_bigBlind { p.bigBlind } {
    assert(m_smallBlind >= 0 && "negative small blind");
    assert(m_bigBlind >= 0 && "negative big blind");
  }

  CashGame(const CashGame&) = delete;
  CashGame(CashGame&&) = delete;
  CashGame& operator=(const CashGame&) = delete;
  CashGame& operator=(CashGame&&) = delete;
  ~CashGame() = default;
  [[nodiscard]] constexpr double getSmallBlind() const noexcept { return m_smallBlind; }
  [[nodiscard]] constexpr double getBigBlind() const noexcept { return m_bigBlind; }
}; // class CashGame


// Note : must use frozen::string when it is a map key.
// frozen::string can be created from std::string_view.

export [[nodiscard]] std::string_view toString(Variant variant);
export [[nodiscard]] std::string_view toString(Limit limitType);

module : private;

std::vector<const Hand*> Game::viewHands() const {
  std::vector<const Hand*> ret;
  std::ranges::transform(m_hands, std::back_inserter(ret), [&](const auto & h) { return h.get(); });
  return ret;
}

std::vector<const Hand*> Game::viewHands(std::string_view player) const {
  std::vector<const Hand*> ret;
  std::ranges::for_each(m_hands, [&](const auto & h) { if (h->isPlayerInvolved(player)) { ret.push_back(h.get()); } });
  return ret;
}

std::string_view toString(Variant variant) {
  static constexpr auto VARIANT_TO_STRING {
    frozen::make_unordered_map<Variant, std::string_view>({
      { Variant::holdem, "holdem" }, { Variant::omaha, "omaha" },
      { Variant::omaha5, "omaha5" }, { Variant::none, "none" }
    })
  };
  return VARIANT_TO_STRING.find(variant)->second;
}

std::string_view toString(Limit limitType) {
  static constexpr auto LIMIT_TO_STRING {
    frozen::make_unordered_map<Limit, std::string_view>({
      { Limit::noLimit, "no-limit" }, { Limit::potLimit, "pot-limit" },
      { Limit::none, "none" }
    })
  };
  return LIMIT_TO_STRING.find(limitType)->second;
}