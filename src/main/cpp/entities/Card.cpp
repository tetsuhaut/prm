module;

#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <string_view>

export module entities.Card;

export enum class /*[[nodiscard]]*/ Card : short {
  none = 0,
  twoSpade = 1,
  threeSpade = 2,
  fourSpade = 3,
  fiveSpade = 4,
  sixSpade = 5,
  sevenSpade = 6,
  eightSpade = 7,
  nineSpade = 8,
  tenSpade = 9,
  jackSpade = 10,
  queenSpade = 11,
  kingSpade = 12,
  aceSpade = 13,
  twoHeart = 14,
  threeHeart = 15,
  fourHeart = 16,
  fiveHeart = 17,
  sixHeart = 18,
  sevenHeart = 19,
  eightHeart = 20,
  nineHeart = 21,
  tenHeart = 22,
  jackHeart = 23,
  queenHeart = 24,
  kingHeart = 25,
  aceHeart = 26,
  twoDiamond = 27,
  threeDiamond = 28,
  fourDiamond = 29,
  fiveDiamond = 30,
  sixDiamond = 31,
  sevenDiamond = 32,
  eightDiamond = 33,
  nineDiamond = 34,
  tenDiamond = 35,
  jackDiamond = 36,
  queenDiamond = 37,
  kingDiamond = 38,
  aceDiamond = 39,
  twoClub = 40,
  threeClub = 41,
  fourClub = 42,
  fiveClub = 43,
  sixClub = 44,
  sevenClub = 45,
  eightClub = 46,
  nineClub = 47,
  tenClub = 48,
  jackClub = 49,
  queenClub = 50,
  kingClub = 51,
  aceClub = 52,
  back = 53
};

// Note : must use frozen::string when it is a map key.
// frozen::string can be created from std::string_view.

export [[nodiscard]] std::string_view toString(Card c);
export [[nodiscard]] Card toCard(std::string_view strCard);
export namespace cardImages {
std::string_view getImageFile(Card card);
} // namespace cardImages

module : private;

constexpr auto CARD_TO_IMAGE =
frozen::make_unordered_map<Card, std::string_view>( {
  {Card::aceClub,      IMAGES_DIR "aceClub.gif"},
  {Card::twoClub,      IMAGES_DIR "twoClub.gif"},
  {Card::threeClub,    IMAGES_DIR "threeClub.gif"},
  {Card::fourClub,     IMAGES_DIR "fourClub.gif"},
  {Card::fiveClub,     IMAGES_DIR "fiveClub.gif"},
  {Card::sixClub,      IMAGES_DIR "sixClub.gif"},
  {Card::sevenClub,    IMAGES_DIR "sevenClub.gif"},
  {Card::eightClub,    IMAGES_DIR "eightClub.gif"},
  {Card::nineClub,     IMAGES_DIR "nineClub.gif"},
  {Card::tenClub,      IMAGES_DIR "tenClub.gif"},
  {Card::jackClub,     IMAGES_DIR "jackClub.gif"},
  {Card::queenClub,    IMAGES_DIR "queenClub.gif"},
  {Card::kingClub,     IMAGES_DIR "kingClub.gif"},
  {Card::aceHeart,     IMAGES_DIR "aceDiamond.gif"},
  {Card::twoHeart,     IMAGES_DIR "twoDiamond.gif"},
  {Card::threeHeart,   IMAGES_DIR "threeDiamond.gif"},
  {Card::fourHeart,    IMAGES_DIR "fourDiamond.gif"},
  {Card::fiveHeart,    IMAGES_DIR "fiveDiamond.gif"},
  {Card::sixHeart,     IMAGES_DIR "sixDiamond.gif"},
  {Card::sevenHeart,   IMAGES_DIR "sevenDiamond.gif"},
  {Card::eightHeart,   IMAGES_DIR "eightDiamond.gif"},
  {Card::nineHeart,    IMAGES_DIR "nineDiamond.gif"},
  {Card::tenHeart,     IMAGES_DIR "tenDiamond.gif"},
  {Card::jackHeart,    IMAGES_DIR "jackDiamond.gif"},
  {Card::queenHeart,   IMAGES_DIR "queenDiamond.gif"},
  {Card::kingHeart,    IMAGES_DIR "kingDiamond.gif"},
  {Card::aceDiamond,   IMAGES_DIR "aceHeart.gif"},
  {Card::twoDiamond,   IMAGES_DIR "twoHeart.gif"},
  {Card::threeDiamond, IMAGES_DIR "threeHeart.gif"},
  {Card::fourDiamond,  IMAGES_DIR "fourHeart.gif"},
  {Card::fiveDiamond,  IMAGES_DIR "fiveHeart.gif"},
  {Card::sixDiamond,   IMAGES_DIR "sixHeart.gif"},
  {Card::sevenDiamond, IMAGES_DIR "sevenHeart.gif"},
  {Card::eightDiamond, IMAGES_DIR "eightHeart.gif"},
  {Card::nineDiamond,  IMAGES_DIR "nineHeart.gif"},
  {Card::tenDiamond,   IMAGES_DIR "tenHeart.gif"},
  {Card::jackDiamond,  IMAGES_DIR "jackHeart.gif"},
  {Card::queenDiamond, IMAGES_DIR "queenHeart.gif"},
  {Card::kingDiamond,  IMAGES_DIR "kingHeart.gif"},
  {Card::aceSpade,     IMAGES_DIR "aceSpade.gif"},
  {Card::twoSpade,     IMAGES_DIR "twoSpade.gif"},
  {Card::threeSpade,   IMAGES_DIR "threeSpade.gif"},
  {Card::fourSpade,    IMAGES_DIR "fourSpade.gif"},
  {Card::fiveSpade,    IMAGES_DIR "fiveSpade.gif"},
  {Card::sixSpade,     IMAGES_DIR "sixSpade.gif"},
  {Card::sevenSpade,   IMAGES_DIR "sevenSpade.gif"},
  {Card::eightSpade,   IMAGES_DIR "eightSpade.gif"},
  {Card::nineSpade,    IMAGES_DIR "nineSpade.gif"},
  {Card::tenSpade,     IMAGES_DIR "tenSpade.gif"},
  {Card::jackSpade,    IMAGES_DIR "jackSpade.gif"},
  {Card::queenSpade,   IMAGES_DIR "queenSpade.gif"},
  {Card::kingSpade,    IMAGES_DIR "kingSpade.gif"},
  {Card::back,         IMAGES_DIR "back.gif"},
});

std::string_view cardImages::getImageFile(Card card) {
  assert(card != Card::none);
  return CARD_TO_IMAGE.find(card)->second;
}

std::string_view toString(Card c) {
  static constexpr auto ENUM_TO_STRING {
    frozen::make_unordered_map<Card, std::string_view>({
      { Card::none, "none" }, { Card::twoSpade, "2s" }, { Card::threeSpade, "3s" },
      { Card::fourSpade, "4s" }, { Card::fiveSpade, "5s" },
      { Card::sixSpade, "6s" }, { Card::sevenSpade, "7s" },
      { Card::eightSpade, "8s" }, { Card::nineSpade, "9s" },
      { Card::tenSpade, "Ts" }, { Card::jackSpade, "Js" },
      { Card::queenSpade, "Qs" }, { Card::kingSpade, "Ks" },
      { Card::aceSpade, "As" }, { Card::twoHeart, "2h" },
      { Card::threeHeart, "3h" }, { Card::fourHeart, "4h" },
      { Card::fiveHeart, "5h" }, { Card::sixHeart, "6h" },
      { Card::sevenHeart, "7h" }, { Card::eightHeart, "8h" },
      { Card::nineHeart, "9h" }, { Card::tenHeart, "Th" },
      { Card::jackHeart, "Jh" }, { Card::queenHeart, "Qh" },
      { Card::kingHeart, "Kh" }, { Card::aceHeart, "Ah" },
      { Card::twoDiamond, "2d" }, { Card::threeDiamond, "3d" },
      { Card::fourDiamond, "4d" }, { Card::fiveDiamond, "5d" },
      { Card::sixDiamond, "6d" }, { Card::sevenDiamond, "7d" },
      { Card::eightDiamond, "8d" }, { Card::nineDiamond, "9d" },
      { Card::tenDiamond, "Td" }, { Card::jackDiamond, "Jd" },
      { Card::queenDiamond, "Qd" }, { Card::kingDiamond, "Kd" },
      { Card::aceDiamond, "Ad" }, { Card::twoClub, "2c" },
      { Card::threeClub, "3c" }, { Card::fourClub, "4c" },
      { Card::fiveClub, "5c" }, { Card::sixClub, "6c" },
      { Card::sevenClub, "7c" }, { Card::eightClub, "8c" },
      { Card::nineClub, "9c" }, { Card::tenClub, "Tc" },
      { Card::jackClub, "Jc" }, { Card::queenClub, "Qc" },
      { Card::kingClub, "Kc" }, { Card::aceClub, "Ac" },
    })
  };
  return ENUM_TO_STRING.find(c)->second;
}

Card toCard(std::string_view strCard) {
  static constexpr auto STRING_TO_ENUM {
    frozen::make_unordered_map<frozen::string, Card>({
      { "none", Card::none }, { "2s", Card::twoSpade }, { "3s", Card::threeSpade },
      { "4s", Card::fourSpade }, { "5s", Card::fiveSpade },
      { "6s", Card::sixSpade }, { "7s", Card::sevenSpade },
      { "8s", Card::eightSpade }, { "9s", Card::nineSpade },
      { "Ts", Card::tenSpade }, { "Js", Card::jackSpade },
      { "Qs", Card::queenSpade }, { "Ks", Card::kingSpade },
      { "As", Card::aceSpade }, { "2h", Card::twoHeart },
      { "3h", Card::threeHeart }, { "4h", Card::fourHeart },
      { "5h", Card::fiveHeart }, { "6h", Card::sixHeart },
      { "7h", Card::sevenHeart }, { "8h", Card::eightHeart },
      { "9h", Card::nineHeart }, { "Th", Card::tenHeart },
      { "Jh", Card::jackHeart }, { "Qh", Card::queenHeart },
      { "Kh", Card::kingHeart }, { "Ah", Card::aceHeart },
      { "2d", Card::twoDiamond }, { "3d", Card::threeDiamond },
      { "4d", Card::fourDiamond }, { "5d", Card::fiveDiamond },
      { "6d", Card::sixDiamond }, { "7d", Card::sevenDiamond },
      { "8d", Card::eightDiamond }, { "9d", Card::nineDiamond },
      { "Td", Card::tenDiamond }, { "Jd", Card::jackDiamond },
      { "Qd", Card::queenDiamond }, { "Kd", Card::kingDiamond },
      { "Ad", Card::aceDiamond }, { "2c", Card::twoClub },
      { "3c", Card::threeClub }, { "4c", Card::fourClub },
      { "5c", Card::fiveClub }, { "6c", Card::sixClub },
      { "7c", Card::sevenClub }, { "8c", Card::eightClub },
      { "9c", Card::nineClub }, { "Tc", Card::tenClub },
      { "Jc", Card::jackClub }, { "Qc", Card::queenClub },
      { "Kc", Card::kingClub }, { "Ac", Card::aceClub }
    })
  };
  return STRING_TO_ENUM.find(strCard)->second;
}

