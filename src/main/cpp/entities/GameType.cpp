module;

export module entities.GameType;

import std;

export enum class [[nodiscard]] GameType : short { none, cashGame, tournament };
export [[nodiscard]] std::string_view toString(GameType gt);

import language.Map;

module : private;

std::string_view toString(GameType gt) {
  static constexpr auto ENUM_TO_STRING = language::Map<GameType, std::string_view, 2> { {{
    { GameType::cashGame, "cashGame" }, { GameType::tournament, "tournament" }
  }}};
  return ENUM_TO_STRING.at(gt);
}
