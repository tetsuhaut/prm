module;


#include <algorithm> // std::size(), std::ranges::for_each
#include <array>
#include <format>
#include <iostream>
#include <memory> // uptr, std::is_same_v
#include <optional>
#include <print>
#include <span>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

export module history.WinamaxHandBuilder;

import entities.Action; // Street
import entities.Card;
import entities.Game; // CashGame, Tournament
import entities.GameType;
import entities.Hand;
//import entities.Player;
import entities.Seat;
import history.GameData;
import language.containers;
import language.strings;
import system.PlayerCache;
import system.TextFile;
import system.Time;

export namespace WinamaxHandBuilder {
[[nodiscard]] std::pair<std::unique_ptr<Hand>, std::unique_ptr<GameData>>
    buildCashgameHandAndGameData(TextFile& tfl,
                                 PlayerCache& pc);

[[nodiscard]] std::pair<std::unique_ptr<Hand>, std::unique_ptr<GameData>>
    buildTournamentHandAndGameData(
      TextFile& tfl, PlayerCache& pc);

template<typename GAME_TYPE>
[[nodiscard]] std::pair<std::unique_ptr<Hand>, std::unique_ptr<GameData>> buildHandAndGameData(
      TextFile& tfl,
PlayerCache& pc) {
  static_assert(std::is_same_v<GAME_TYPE, CashGame> or std::is_same_v<GAME_TYPE, Tournament>);

  if constexpr(std::is_same_v<GAME_TYPE, CashGame>) { return buildCashgameHandAndGameData(tfl, pc); }

  if constexpr(std::is_same_v<GAME_TYPE, Tournament>) { return buildTournamentHandAndGameData(tfl, pc); }
}

[[nodiscard]] std::unique_ptr<Hand> buildCashgameHand(TextFile& tfl, PlayerCache& pc);
[[nodiscard]] std::unique_ptr<Hand> buildTournamentHand(TextFile& tfl, PlayerCache& pc);

template<typename GAME_TYPE>
[[nodiscard]] std::unique_ptr<Hand> buildHand(TextFile& tfl, PlayerCache& pc) {
  static_assert(std::is_same_v<GAME_TYPE, CashGame> or std::is_same_v<GAME_TYPE, Tournament>);

  if constexpr(std::is_same_v<GAME_TYPE, CashGame>) { return buildCashgameHand(tfl, pc); }

  if constexpr(std::is_same_v<GAME_TYPE, Tournament>) { return buildTournamentHand(tfl, pc); }
}
} // namespace WinamaxHandBuilder

module : private;

constexpr static std::array FIVE_NONE_CARDS { Card::none, Card::none, Card::none, Card::none, Card::none };

// splits the given str into tokens, separated by delimiters.
// We assume there are always 5 tokens
[[nodiscard]] std::array<std::string_view, 5> split(std::string_view str,
    std::string_view delimiter) {
  std::array<std::string_view, 5> ret { "none", "none", "none", "none", "none" };
  std::size_t offset = 0, delimiterPosition = 0, arrayIndex = 0;

  while (std::string_view::npos != (delimiterPosition = str.find(delimiter, offset))) {
    ret.at(arrayIndex) = str.substr(offset, delimiterPosition - offset);
    offset = delimiterPosition + delimiter.size();
    arrayIndex++;
  }

  if (offset != str.size() - 1) {
    ret.at(arrayIndex) = str.substr(offset);
  }

  return ret;
}

[[nodiscard]] static std::array<Card, 5> parseCards(std::string_view line) {
  const auto pos { line.rfind('[') + 1 };
  const auto strCards { line.substr(pos, line.rfind(']') - pos) };
  const auto& cardsStr { split(strCards, " ") };
  std::array<Card, 5> ret;
  std::transform(std::begin(cardsStr), std::end(cardsStr), std::begin(ret), toCard);
  return ret;
}


static constexpr auto MINUS_LENGTH { language::strings::length(" - ") }; // nb char without '\0
static constexpr auto HAND_ID_LENGTH { language::strings::length(" - HandId: #") }; // nb char without '\0
static constexpr std::string_view WINAMAX_HISTORY_TIME_FORMAT { "%Y/%m/%d %H:%M:%S" }; // ex: 2014/10/31 00:45:01

// Returns the HandId position, the Hand start time and the Hand Id
[[nodiscard]] std::tuple<std::size_t, Time, std::string>
parseStartOfWinamaxPokerLine(std::string_view line) {
  // "^Winamax Poker - .* - HandId: #(.*) .*  - (.*) UTC$"
  if (!line.starts_with("Winamax Poker")) { throw "a Winamax poker line should start with 'Winamax Poker'"; }

  if (!line.ends_with("UTC")) { throw "a Winamax poker line should end with 'UTC'"; }

  const auto datePos { line.rfind(" - ") + MINUS_LENGTH }; // nb char without '\0'

  if (datePos >= line.length()) { throw "bad datePos"; }

  const Time handStartDate({ .strTime = line.substr(datePos, line.rfind(' ') - datePos), .format = WINAMAX_HISTORY_TIME_FORMAT });
  const auto handIdPos { line.find(" - HandId: #") + HAND_ID_LENGTH };
  const auto handId { line.substr(handIdPos, line.find(" - ", handIdPos) - handIdPos) };
  return { handIdPos, handStartDate, std::string(handId) };
}

static constexpr auto BUY_IN_LENGTH { language::strings::length(" buyIn: ") }; // nb char without '\0
static constexpr auto LEVEL_LENGTH { language::strings::length(" level: ") }; // nb char without '\0

[[nodiscard]]  std::tuple<double, int, Time, std::string>
getBuyInLevelDateHandIdFromTournamentWinamaxPokerLine(std::string_view line) {
  const auto& [handIdPos, handStartDate, handId] { parseStartOfWinamaxPokerLine(line) };
  // "^Winamax Poker - .* buyIn: (.*) level: (.*) - HandId: #(.*) - .* - (.*) UTC$"
  const auto buyInPos { line.find(" buyIn: ") + BUY_IN_LENGTH };
  const auto levelPos { line.find(" level: ") + LEVEL_LENGTH };
  const auto buyIn { language::strings::toBuyIn(line.substr(buyInPos, levelPos - LEVEL_LENGTH - buyInPos)) };
  const auto level { language::strings::toInt(line.substr(levelPos, handIdPos - HAND_ID_LENGTH - levelPos)) };
  return { buyIn, level, handStartDate, handId };
}

[[nodiscard]]  std::tuple<int, Time, std::string>
getLevelDateHandIdFromTournamentWinamaxPokerLine(std::string_view line) {
  const auto& [handIdPos, handStartDate, handId] { parseStartOfWinamaxPokerLine(line) };
  // "^Winamax Poker - .* buyIn: (.*) level: (.*) - HandId: #(.*) - .* - (.*) UTC$"
  const auto levelPos { line.find(" level: ") + LEVEL_LENGTH };
  const auto level { language::strings::toInt(line.substr(levelPos, handIdPos - HAND_ID_LENGTH - levelPos)) };
  return { level, handStartDate, handId };
}

[[nodiscard]]  std::tuple<double, double, Time, std::string>
getSmallBlindBigBlindDateHandIdFromCashGameWinamaxPokerLine(std::string_view line) {
  const auto& [_, handStartDate, handId] { parseStartOfWinamaxPokerLine(line) };
  // "^Winamax Poker - .* - HandId: #(.*) - .* \\((.*)/(.*)\\) - (.*) UTC$"
  const auto smallBlindPos { line.find('(') + 1 };
  const auto bigBlindPos { line.find('/') + 1 };
  const auto smallBlind { language::strings::toAmount(line.substr(smallBlindPos, bigBlindPos - 1 - smallBlindPos)) };
  const auto bigBlind { language::strings::toAmount(line.substr(bigBlindPos, line.find(')') - 1)) };
  return { smallBlind, bigBlind, handStartDate, handId };
}

static constexpr auto DEALT_TO_LENGTH { language::strings::length("Dealt to ") };

[[nodiscard]]  std::array<Card, 5> parseHeroCards(TextFile& tf,
    PlayerCache& cache) {
  std::println("Parsing hero cards for file {}.", tf.getFileStem());

  if (tf.startsWith("Dealt to ")) {
    const auto& line { tf.getLine() };
    // "^Dealt to (.*) \\[(.*)\\]$"
    const auto& playerName { line.substr(DEALT_TO_LENGTH, line.find(' ', DEALT_TO_LENGTH) - DEALT_TO_LENGTH) };
    cache.setIsHero(playerName);
    const auto& ret { parseCards(line) };
    tf.next();
    return ret;
  }

  return FIVE_NONE_CARDS;
}

[[nodiscard]]  std::array<Card, 5> parseBoardCards(TextFile& tf) {
  std::println("Parsing board cards for file {}.", tf.getFileStem());
  std::array ret { FIVE_NONE_CARDS };

  while (!tf.lineIsEmpty()) {
    if (tf.startsWith("Board: ")) {
      // "^Board: \\[([\\w\\s]+)\\]$"
      ret = parseCards(tf.getLine());
    }

    tf.next();
  }

  tf.next();
  return ret;
}

[[nodiscard]]  Street parseStreet(TextFile& tf) {
  auto street { Street::none }; // the current line can be *** ANTE/BLINDS ***

  if (tf.startsWith("*** PRE-FLOP ***")) { street = Street::preflop; }
  else if (tf.startsWith("*** FLOP ***")) { street = Street::flop; }
  else if (tf.startsWith("*** TURN ***")) { street = Street::turn; }
  else if (tf.startsWith("*** RIVER ***")) { street = Street::river; }
  else if (tf.startsWith("*** SHOW DOWN ***")) { street = Street::river; }

  tf.next();
  return street;
}

static constexpr auto TABLE_LENGTH { language::strings::length("Table: '") };
static constexpr auto SEAT_NB_LENGTH { language::strings::length(" Seat #") };

// returns nbMaxSeats, tableName, buttonSeat
[[nodiscard]]  std::tuple<Seat, std::string, Seat> getNbMaxSeatsTableNameButtonSeatFromTableLine(
  TextFile& tf) {
  tf.next();
  const auto& line { tf.getLine() };
  std::println("Parsing table line {}.", line);
  // Table: 'Frankfurt 11' 9-max (real money) Seat #2 is the button
  // Table: 'Expresso(111550795)#0' 3-max (real money) Seat #1 is the button
  // ^Table: '(.*)' (.*)-max .* Seat #(.*) is the button$
  const auto pos { line.find("' ", TABLE_LENGTH) };
  const auto& tableName { language::strings::sanitize(line.substr(TABLE_LENGTH, pos - TABLE_LENGTH)) };
  const auto nbMaxSeats { tableSeat::fromString(line.substr(pos + 2, line.find("-max") - pos - 2)) };
  const auto posSharp { line.find(" Seat #") + SEAT_NB_LENGTH };
  const auto& buttonSeatStr { line.substr(posSharp, line.find(" is the button") - posSharp) };
  const auto buttonSeat { tableSeat::fromString(buttonSeatStr) };

  if (line.starts_with("Seat ")) { throw "a Table line should start with 'Seat '"; }

  tf.next();
  return { nbMaxSeats, tableName, buttonSeat };
}

static constexpr auto POSTS_ANTE_LENGTH { language::strings::length(" posts ante ") };

[[nodiscard]]  long parseAnte(TextFile& tf) {
  std::println("Parsing ante for file {}.", tf.getFileStem());
  // "^(.*) posts m_ante (.*).*$"
  long ret = 0;

  if (const auto posAnte { tf.find(" posts ante ") }; std::string::npos != posAnte) {
    ret = language::strings::toInt(tf.getLine().substr(posAnte + POSTS_ANTE_LENGTH));
  }

  while (tf.contains(" posts ")) { tf.next(); }

  return ret;
}

struct ActionParams {
  std::string_view playerName;
  ActionType actionType;
  double betAmount;
};

[[nodiscard]]  std::optional<ActionParams>
parseActionParams(std::string_view line) {
  if (line.ends_with(" folds")) {
    return ActionParams { .playerName = line.substr(0, line.rfind(' ')),
                          .actionType = ActionType::fold,
                          .betAmount = 0.0
                        };
  }

  if (line.ends_with(" checks")) {
    return ActionParams {
      .playerName = line.substr(0, line.rfind(' ')),
      .actionType = ActionType::check,
      .betAmount = 0.0
    };
  }

  if (const auto pos = line.find(" calls "); std::string_view::npos != pos) {
    return ActionParams {
      .playerName = line.substr(0, pos),
      .actionType = ActionType::call,
      .betAmount = language::strings::toAmount(line.substr(line.rfind(' ') + 1))
    };
  }

  if (const auto pos = line.find(" bets "); std::string_view::npos != pos) {
    return ActionParams {
      .playerName = line.substr(0, pos),
      .actionType = ActionType::bet,
      .betAmount = language::strings::toAmount(line.substr(line.rfind(' ') + 1))
    };
  }

  if (const auto pos = line.find(" raises "); std::string_view::npos != pos) {
    return ActionParams {
      .playerName = line.substr(0, pos),
      .actionType = ActionType::raise,
      .betAmount = language::strings::toAmount(line.substr(line.rfind(' ') + 1))
    };
  }

  return {};
}

static constexpr std::array<std::string_view, 6> ACTION_TOKENS { " folds", " checks", " bets ", " calls ", " raises ", " shows " };

[[nodiscard]]  std::vector<std::unique_ptr<Action>> parseActions(TextFile& tf, Street street,
std::string_view handId) {
  std::vector<std::unique_ptr<Action>> actions;

  while (tf.containsOneOf(ACTION_TOKENS)) {
    const auto& line { tf.getLine() };

    if (const auto & oActionParams { parseActionParams(line) }; oActionParams.has_value()) {
      const auto [playerName, type, bet] { oActionParams.value() };
      actions.push_back(std::make_unique<Action>(Action::Params {
        .handId = handId,
        .playerName = playerName,
        .street = street,
        .type = type,
        .actionIndex = actions.size(),
        .betAmount = bet }));
    }

    tf.next(); // nothing to do for 'shows' action
  }

  return actions;
}

[[nodiscard]]  std::array<std::string, 10> parseWinners(TextFile& tf) {
  std::array<std::string, 10> winners;
  auto pos { std::string::npos };
  std::size_t i { 0 };

  while (std::string::npos != (pos = tf.find(" collected "))) {
    winners.at(i++) = tf.getLine().substr(0, pos);
    tf.next();
  }

  return winners;
}

[[nodiscard]]  std::vector<std::unique_ptr<Action>> createActionForWinnersWithoutAction(
      std::span<std::string> winners, std::span<std::unique_ptr<Action>> actions, Street street,
std::string_view handId) {
  std::vector<std::unique_ptr<Action>> ret;
  std::ranges::for_each(winners, [&](std::string_view winner) {
    if (!winner.empty() and !language::containers::containsIf(actions, [&](auto & pAction) { return winner == pAction->getPlayerName(); })) {
      ret.push_back(std::make_unique<Action>(Action::Params {
        .handId = handId,
        .playerName = winner,
        .street = street,
        .type = ActionType::none,
        .actionIndex = actions.size() + ret.size(),
        .betAmount = 0.0}));
    }
  });
  return ret;
}

[[nodiscard]]  std::pair<std::vector<std::unique_ptr<Action>>, std::array<std::string, 10>>
parseActionsAndWinners(TextFile& tf, std::string_view handId) {
  std::println("Parsing actions and winners for file {}.", tf.getFileStem());
  std::vector<std::unique_ptr<Action>> actions;
  Street currentStreet = Street::none;

  while (!tf.contains(" collected ")) {
    currentStreet = parseStreet(tf);
    auto currentActions { parseActions(tf, currentStreet, handId) };
    language::containers::moveInto(currentActions, actions);
  }

  auto winners { parseWinners(tf) };
  auto additionalActions { createActionForWinnersWithoutAction(winners, actions, currentStreet, handId) };
  language::containers::moveInto(additionalActions, actions);
  return { std::move(actions), winners };
}

constexpr static auto SEAT_LENGTH { language::strings::length("Seat ") };

[[nodiscard]] std::unordered_map<Seat, std::string> parseSeats(TextFile& tf,
    PlayerCache& /*cache*/) {
  std::unordered_map<Seat, std::string> ret;

  while (tf.startsWith("Seat ")) {
    const auto& line { tf.getLine() };
    const auto pos { line.find(": ", SEAT_LENGTH) };
    const auto seat { tableSeat::fromString(line.substr(SEAT_LENGTH, pos - SEAT_LENGTH)) };
    ret[seat] = line.substr(pos + 2, line.rfind(" (") - pos - 2);
    tf.next();
  }

  while (!tf.contains(" posts ")) { tf.next(); } // disreguard the blinds (lol)

  return ret;
}

constexpr static auto WINAMAX_SITE_NAME { "Winamax" };

template<GameType gameType>
[[nodiscard]]  std::unique_ptr<Hand> getHand(TextFile& tf, PlayerCache& cache,
    int level, const Time& date, std::string_view handId) {
  std::println("Building hand and maxSeats from history file {}.", tf.getFileStem());
  const auto& [nbMaxSeats, tableName, buttonSeat] { getNbMaxSeatsTableNameButtonSeatFromTableLine(tf) };
  const auto& seatPlayers { parseSeats(tf, cache) };
  std::ranges::for_each(seatPlayers, [&cache](const auto & entry) {
    const auto& [seat, playerName] { entry };

    if (!playerName.empty()) { cache.addIfMissing(playerName); }
  });
  const auto ante { parseAnte(tf) };
  const auto& heroCards { parseHeroCards(tf, cache) };
  auto [actions, winners] { parseActionsAndWinners(tf, handId) };
  const auto& boardCards { parseBoardCards(tf) };
  std::println("nb actions={}", actions.size());
  Hand::Params params { .id = handId, .gameType = gameType, .siteName = WINAMAX_SITE_NAME,
                        .tableName = tableName, .buttonSeat = buttonSeat, .maxSeats = nbMaxSeats, .level = level,
                        .ante = ante, .startDate = date, .seatPlayers = seatPlayers, .heroCards = heroCards,
                        .boardCards = boardCards, .actions = std::move(actions), .winners = winners };
  return std::make_unique<Hand>(params);
}

std::unique_ptr<Hand> WinamaxHandBuilder::buildCashgameHand(TextFile& tf, PlayerCache& pc) {
  std::println("Building Cashgame from history file {}.", tf.getFileStem());
  const auto& [_, date, handId] { parseStartOfWinamaxPokerLine(tf.getLine()) };
  return getHand<GameType::cashGame>(tf, pc, 0, date, handId); // for cashGame, level is zero
}

std::unique_ptr<Hand> WinamaxHandBuilder::buildTournamentHand(TextFile& tf, PlayerCache& pc) {
  std::println("Building Tournament from history file {}.", tf.getFileStem());
  const auto& [level, date, handId] { getLevelDateHandIdFromTournamentWinamaxPokerLine(tf.getLine()) };
  return getHand<GameType::tournament>(tf, pc, level, date, handId);
}

std::pair<std::unique_ptr<Hand>, std::unique_ptr<GameData>> WinamaxHandBuilder::buildCashgameHandAndGameData(
      TextFile& tf,
PlayerCache& pc) {
  std::println("Building Cashgame and game data from history file {}.",
                           tf.getFileStem());
  const auto& [smallBlind, bigBlind, date, handId] { getSmallBlindBigBlindDateHandIdFromCashGameWinamaxPokerLine(tf.getLine()) };
  auto pHand { getHand<GameType::cashGame>(tf, pc, 0, date, handId) };
  return { std::move(pHand), std::make_unique<GameData>(GameData::Args{.nbMaxSeats = pHand->getMaxSeats(), .smallBlind = smallBlind, .bigBlind = bigBlind, .buyIn = 0, .startDate = pHand->getStartDate() }) };
}

std::pair<std::unique_ptr<Hand>, std::unique_ptr<GameData>> WinamaxHandBuilder::buildTournamentHandAndGameData(
      TextFile& tf,
PlayerCache& pc) {
  std::println("Building Tournament and game data from history file {}.", tf.getFileStem());
  const auto& [buyIn, level, date, handId] { getBuyInLevelDateHandIdFromTournamentWinamaxPokerLine(tf.getLine()) };
  auto pHand { getHand<GameType::tournament>(tf, pc, level, date, handId) };
  return { std::move(pHand), std::make_unique<GameData>(GameData::Args{.nbMaxSeats = pHand->getMaxSeats(), .smallBlind = 0, .bigBlind = 0, .buyIn = buyIn, .startDate = pHand->getStartDate()}) };
}
