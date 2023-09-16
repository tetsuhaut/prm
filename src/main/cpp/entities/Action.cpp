module;

#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <cassert>
#include <string>
#include <string_view>

export module entities.Action;

/**
 * The elementary move a player can do during the game.
 */
export enum /*[[nodiscard]]*/ class ActionType : short { none, fold, check, call, bet, raise };

/**
 * The name of the current game board configuration.
 */
export enum class /*[[nodiscard]]*/ Street : short { none, preflop, flop, turn, river };

/**
 * The elementary move of a player.
 */
export class [[nodiscard]] Action final {
private:
  std::string m_handId;
  std::string m_playerName;
  Street m_street;
  ActionType m_type;
  std::size_t m_index;
  double m_betAmount;

public:

  struct [[nodiscard]] Params final {
    std::string_view handId;
    std::string_view playerName;
    Street street;
    ActionType type;
    std::size_t actionIndex;
    double betAmount;
  };

  explicit Action(const Params& p)
    : m_handId { p.handId },
      m_playerName { p.playerName },
      m_street { p.street },
      m_type { p.type },
      m_index { p.actionIndex },
      m_betAmount { p.betAmount } {
    assert(Street::none != m_street);
    assert(!m_handId.empty());
    assert(!m_playerName.empty());
    assert(m_betAmount >= 0);
  }

  Action(const Action&) = delete;
  Action(Action&&) = delete;
  Action& operator=(const Action&) = delete;
  Action& operator=(Action&&) = delete;
  ~Action() = default;
  [[nodiscard]] Street getStreet() const noexcept { return m_street; }
  [[nodiscard]] std::string getHandId() const noexcept { return m_handId; }
  [[nodiscard]] std::string getPlayerName() const noexcept { return m_playerName; }
  [[nodiscard]] ActionType getType() const noexcept { return m_type; }
  [[nodiscard]] std::size_t getIndex() const noexcept { return m_index; }
  [[nodiscard]] double getBetAmount() const noexcept { return m_betAmount; }
}; // class Action

export [[nodiscard]] std::string_view toString(ActionType at);
export [[nodiscard]] std::string_view toString(Street st);

module : private;

// Note : must use frozen::string for map keys.
// frozen::string can be created from std::string_view.

// exported methods
[[nodiscard]] std::string_view toString(ActionType at) {
  static constexpr auto ACTION_TYPE_TO_STRING {
    frozen::make_unordered_map<ActionType, std::string_view>({
      { ActionType::bet, "bet" }, { ActionType::call, "call" },
      { ActionType::check, "check" }, { ActionType::fold, "fold" },
      { ActionType::raise, "raise" }, { ActionType::none, "none" }
    })
  };
  return ACTION_TYPE_TO_STRING.find(at)->second;
}

[[nodiscard]] std::string_view toString(Street st) {
  static constexpr auto STREET_TO_STRING {
    frozen::make_unordered_map<Street, std::string_view>({
      { Street::preflop, "preflop" }, { Street::flop, "flop" },
      { Street::turn, "turn" }, { Street::river, "river" }
    })
  };
  return STREET_TO_STRING.find(st)->second;
}
