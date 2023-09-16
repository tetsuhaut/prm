module;

#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <cassert>
#include <string_view>

export module entities.Seat;

export enum class /*[[nodiscard]]*/ Seat : short {
  seatOne, seatTwo, seatThree, seatFour, seatFive, seatSix, seatSeven,
  seatEight, seatNine, seatTen, seatUnknown
};

export namespace tableSeat {
/*
 * Transforms "1" into Seat::SeatOne and so on.
 */
[[nodiscard]] Seat fromString(std::string_view seatStr);

/*
 * Transforms 0 into Seat::SeatOne and so on.
 */
[[nodiscard]] Seat fromArrayIndex(std::size_t i);

/*
 * Transforms 1 into Seat::SeatOne and so on.
 */
[[nodiscard]] Seat fromInt(int i);

/*
 * Transforms Seat::SeatOne into 1 and so on.
 */
[[nodiscard]] int toInt(Seat seat);

/*
 * Transforms Seat::SeatOne into 1 and so on.
 */
[[nodiscard]] std::string_view toString(Seat s);

/*
 * Transforms Seat::SeatOne into 0 and so on.
 */
[[nodiscard]] std::size_t toArrayIndex(Seat seat);
} // namespace tableSeat

module : private;

// Note : must use frozen::string when it is a map key.
// frozen::string can be created from std::string_view.

/*[[nodiscard]]*/ Seat tableSeat::fromString(std::string_view seatStr) {
  assert("1" == seatStr or "2" == seatStr or "3" == seatStr or "4" == seatStr or "5" == seatStr
         or "6" == seatStr or "7" == seatStr or "8" == seatStr or "9" == seatStr or "10" == seatStr);
  static constexpr auto STRING_TO_ENUM {
    frozen::make_unordered_map<frozen::string, Seat>({
      {"1", Seat::seatOne}, {"2", Seat::seatTwo}, {"3", Seat::seatThree}, {"4", Seat::seatFour}, {"5", Seat::seatFive},
      {"6", Seat::seatSix}, {"7", Seat::seatSeven}, {"8", Seat::seatEight}, {"9", Seat::seatNine}, {"10", Seat::seatTen}
    })
  };
  return STRING_TO_ENUM.find(seatStr)->second;
}

/*[[nodiscard]]*/ Seat tableSeat::fromArrayIndex(std::size_t i) {
  assert(10 > i && "Can't find a seat for that value");
  static constexpr auto SIZET_TO_ENUM {
    frozen::make_unordered_map<std::size_t, Seat>({
      {1, Seat::seatOne}, {2, Seat::seatTwo}, {3, Seat::seatThree}, {4, Seat::seatFour}, {5, Seat::seatFive},
      {6, Seat::seatSix}, {7, Seat::seatSeven}, {8, Seat::seatEight}, {9, Seat::seatNine}, {10, Seat::seatTen}
    })
  };
  return SIZET_TO_ENUM.find(i + 1)->second;
}

/*[[nodiscard]]*/ std::size_t tableSeat::toArrayIndex(Seat seat) {
  static constexpr auto ENUM_TO_SIZET {
    frozen::make_unordered_map<Seat, std::size_t>({
      {Seat::seatOne, 1}, {Seat::seatTwo, 2}, {Seat::seatThree, 3}, {Seat::seatFour, 4}, {Seat::seatFive, 5},
      {Seat::seatSix, 6}, {Seat::seatSeven, 7}, {Seat::seatEight, 8}, {Seat::seatNine, 9}, {Seat::seatTen, 10}
    })
  };
  return ENUM_TO_SIZET.find(seat)->second - 1;
}

/*[[nodiscard]]*/ std::string_view tableSeat::toString(Seat seat) {
  static constexpr auto ENUM_TO_STRING {
    frozen::make_unordered_map<Seat, std::string_view>({
      {Seat::seatOne, "1"}, {Seat::seatTwo, "2"}, {Seat::seatThree, "3"}, {Seat::seatFour, "4"}, {Seat::seatFive, "5"},
      {Seat::seatSix, "6"}, {Seat::seatSeven, "7"}, {Seat::seatEight, "8"}, {Seat::seatNine, "9"}, {Seat::seatTen, "10"}
    })
  };
  return ENUM_TO_STRING.find(seat)->second;
}

/*[[nodiscard]]*/ Seat tableSeat::fromInt(int i) {
  assert(0 < i and 11 > i);
  static constexpr auto INT_TO_ENUM {
    frozen::make_unordered_map<int, Seat>({
      {1, Seat::seatOne}, {2, Seat::seatTwo}, {3, Seat::seatThree}, {4, Seat::seatFour}, {5, Seat::seatFive},
      {6, Seat::seatSix}, {7, Seat::seatSeven}, {8, Seat::seatEight}, {9, Seat::seatNine}, {10, Seat::seatTen}
    })
  };
  return INT_TO_ENUM.find(i)->second;
}

/*[[nodiscard]]*/ int tableSeat::toInt(Seat seat) {
  static constexpr auto ENUM_TO_INT {
    frozen::make_unordered_map<Seat, int>({
      {Seat::seatOne, 1}, {Seat::seatTwo, 2}, {Seat::seatThree, 3}, {Seat::seatFour, 4}, {Seat::seatFive, 5},
      {Seat::seatSix, 6}, {Seat::seatSeven, 7}, {Seat::seatEight, 8}, {Seat::seatNine, 9}, {Seat::seatTen, 10}
    })
  };
  return ENUM_TO_INT.find(seat)->second;
}
