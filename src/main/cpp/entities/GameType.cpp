module;

#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <string_view>

export module entities.GameType;

export enum class [[nodiscard]] GameType : short { none, cashGame, tournament };
export [[nodiscard]] std::string_view toString(GameType gt);

module : private;

std::string_view toString(GameType gt) {
  static constexpr auto ENUM_TO_STRING {
    frozen::make_unordered_map<GameType, std::string_view>({
      { GameType::cashGame, "cashGame" }, { GameType::tournament, "tournament" }
    })
  };
  return ENUM_TO_STRING.find(gt)->second;
}
