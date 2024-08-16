module;

export module history.WinamaxGameHistory;

import entities.Hand;
import entities.Game; // Limit
import entities.Player;
import entities.Site;
import history.GameData;
import history.WinamaxHandBuilder;
import language.strings; // language::strings::contains()
import system.PlayerCache;
import system.TextFile;

#pragma warning( push )
#pragma warning( disable : 4686)
import std;
#pragma warning( pop ) 

export namespace WinamaxGameHistory {
[[nodiscard]] std::unique_ptr<Site> parseGameHistory(const std::filesystem::path& gameHistoryFile);

std::unique_ptr<Site> parseGameHistory(auto) = delete;
}; // namespace WinamaxGameHistory

module : private;

[[nodiscard]] constexpr Limit fileStemToLimit(std::string_view fileStem) noexcept {
  if (fileStem.size() < 11) { return Limit::none; }

  if (fileStem.ends_with("_pot-limit")) { return Limit::potLimit; }

  if (fileStem.ends_with("_no-limit")) { return Limit::noLimit; }

  return Limit::none;
}

[[nodiscard]] constexpr Variant fileStemToVariant(std::string_view fileStem) noexcept {
  if (fileStem.contains("_holdem_")) { return Variant::holdem; }

  if (fileStem.contains("_omaha_")) { return Variant::omaha; }

  if (fileStem.contains("_omaha5_")) { return Variant::omaha5; }

  return Variant::none;
}

// file: file name, without path and extension
// Fills gameData->m_gameName, gameData->m_isRealMoney, gameData->m_variant and gameData->m_limit
// should parse something like:
// 20141116_Double or Nothing(100679030)_real_holdem_no-limit
// 20150630_Super Freeroll Stade 1 - Déglingos _(123322389)_real_holdem_no-limit
// 20180304_Ferrare 04_real_omaha5_pot-limit
// 20170305_Memphis 06_play_omaha_pot-limit
// "\\d{ 8 }_(.*)_(real|play)?_(.*)_(.*)"
// exported for unit testing
std::optional<std::tuple<bool, std::string, Variant, Limit>> parseFileStem(
std::string_view fileStem) {
  std::tuple<bool, std::string, Variant, Limit> ret {};

  if (12 > fileStem.size()) {
    return ret;
  }

  const auto pos { fileStem.find("_real_", 9) }; // we ignore the date at the start of the file stem
  const auto isRealMoney { std::string_view::npos != pos };

  if (!isRealMoney and (std::string_view::npos == fileStem.find("_play_", 9))) [[unlikely]] {
    return ret;
  }
  const auto gameName { fileStem.substr(9, pos - 9) };
  const auto variant { fileStemToVariant(fileStem) };
  const auto limit { fileStemToLimit(fileStem) };
  ret = { isRealMoney, std::string(gameName), variant, limit };
  return ret;
}

constexpr std::string_view WINAMAX_SITE_NAME = "Winamax";

template <typename GAME_TYPE>
[[nodiscard]] std::unique_ptr<GAME_TYPE> newGame(std::string_view gameId,
    const GameData& gameData) {
  static_assert(std::is_same_v<GAME_TYPE, CashGame> or std::is_same_v<GAME_TYPE, Tournament>);

  if constexpr(std::is_same_v<GAME_TYPE, CashGame>) {
    return std::make_unique<CashGame>(CashGame::Params {.id = gameId, .siteName = WINAMAX_SITE_NAME,
                                      .cashGameName = gameData.m_gameName, .variant = gameData.m_variant, .limit = gameData.m_limit,
                                      .isRealMoney = gameData.m_isRealMoney, .nbMaxSeats = gameData.m_nbMaxSeats,
                                      .smallBlind = gameData.m_smallBlind, .bigBlind = gameData.m_bigBlind, .startDate = gameData.m_startDate});
  }

  if constexpr(std::is_same_v<GAME_TYPE, Tournament>) {
    return std::make_unique<Tournament>(Tournament::Params {.id = gameId, .siteName = WINAMAX_SITE_NAME,
                                        .tournamentName = gameData.m_gameName, .variant = gameData.m_variant, .limit = gameData.m_limit,
                                        .isRealMoney = gameData.m_isRealMoney, .nbMaxSeats = gameData.m_nbMaxSeats, .buyIn = gameData.m_buyIn, .startDate = gameData.m_startDate});
  }
}

void fillFromFileName(const std::tuple<bool, std::string, Variant, Limit>& values,
                      GameData& gameData) {
  const auto [isRealMoney, gameName, variant, limit] { values };
  gameData.m_isRealMoney = isRealMoney;
  gameData.m_gameName = gameName;
  gameData.m_variant = variant;
  gameData.m_limit = limit;
}

template <typename GAME_TYPE> [[nodiscard]]
std::unique_ptr<GAME_TYPE> createGame(const std::filesystem::path& gameHistoryFile,
                                      PlayerCache& cache) {
  const auto& fileStem { language::strings::sanitize(gameHistoryFile.stem().string()) };
  std::unique_ptr<GAME_TYPE> ret;

  if (const auto & oGameDataFromFileName { parseFileStem(fileStem) };
      oGameDataFromFileName.has_value()) {
    TextFile tfl { gameHistoryFile };

    while (tfl.next()) {
      if (nullptr == ret) {
        auto [pHand, pGameData] { WinamaxHandBuilder::buildHandAndGameData<GAME_TYPE>(tfl, cache) };
        fillFromFileName(oGameDataFromFileName.value(), *pGameData);
        ret = newGame<GAME_TYPE>(fileStem, *pGameData);
        ret->addHand(std::move(pHand));
      } else {
        std::println("not the 1st hand : adding the new hand to the existing game history.");
        ret->addHand(WinamaxHandBuilder::buildHand<GAME_TYPE>(tfl, cache));
      }
    }
  }

  return ret;
}

template<typename GAME_TYPE>
[[nodiscard]]  std::unique_ptr<Site> handleGame(const std::filesystem::path& gameHistoryFile) {
  auto pSite { std::make_unique<Site>(WINAMAX_SITE_NAME) };
  PlayerCache cache { WINAMAX_SITE_NAME };

  if (auto g { createGame<GAME_TYPE>(gameHistoryFile, cache) }; nullptr != g) {
    std::println("Game created for file {}.", gameHistoryFile.filename().string());
    pSite->addGame(std::move(g));
  } else {
    std::println(std::cerr, "Game *not* created for file {}.", gameHistoryFile.filename().string());
  }

  auto players { cache.extractPlayers() };
  std::ranges::for_each(players, [&](auto & p) { pSite->addPlayer(std::move(p)); });
  return pSite;
}

// reminder: WinamaxGameHistory is a namespace
std::unique_ptr<Site> WinamaxGameHistory::parseGameHistory(const std::filesystem::path&
    gameHistoryFile) {
  const auto& fileStem { gameHistoryFile.stem().string() };

  if (12 > fileStem.size()
      or gameHistoryFile.extension() != ".txt"
      or fileStem.contains("_summary")
      or fileStem.contains('!') // history files with an '!' in their title are duplicated with another name, so ignore it
      or (std::string::npos == fileStem.find("_real_", 9)
          and std::string::npos == fileStem.find("_play_", 9))) {
    return std::make_unique<Site>(WINAMAX_SITE_NAME);
  }

  return fileStem.contains('(') ? handleGame<Tournament>(gameHistoryFile)
         : handleGame<CashGame>(gameHistoryFile);
}
