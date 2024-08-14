module;

#include <cassert>
#include <string_view>

export module entities.Seat;

import language.Map;

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

/*[[nodiscard]]*/ Seat tableSeat::fromString(std::string_view seatStr) {
  assert("1" == seatStr or "2" == seatStr or "3" == seatStr or "4" == seatStr or "5" == seatStr
         or "6" == seatStr or "7" == seatStr or "8" == seatStr or "9" == seatStr or "10" == seatStr);
  static constexpr auto STRING_TO_ENUM = language::Map<std::string_view, Seat, 10> {{{
    {"1", Seat::seatOne}, {"2", Seat::seatTwo}, {"3", Seat::seatThree}, {"4", Seat::seatFour}, {"5", Seat::seatFive},
    {"6", Seat::seatSix}, {"7", Seat::seatSeven}, {"8", Seat::seatEight}, {"9", Seat::seatNine}, {"10", Seat::seatTen}
  }}};
  return STRING_TO_ENUM.at(seatStr);
}

/*[[nodiscard]]*/ Seat tableSeat::fromArrayIndex(std::size_t i) {
  assert(10 > i and "Can't find a seat for that value");
  static constexpr auto SIZET_TO_ENUM = language::Map<std::size_t, Seat, 10> {{{
    {1, Seat::seatOne}, {2, Seat::seatTwo}, {3, Seat::seatThree}, {4, Seat::seatFour}, {5, Seat::seatFive},
    {6, Seat::seatSix}, {7, Seat::seatSeven}, {8, Seat::seatEight}, {9, Seat::seatNine}, {10, Seat::seatTen}
  }}};
  return SIZET_TO_ENUM.at(i + 1);
}

/*[[nodiscard]]*/ std::size_t tableSeat::toArrayIndex(Seat seat) {
  static constexpr auto ENUM_TO_SIZET = language::Map<Seat, std::size_t, 10> {{{
    {Seat::seatOne, 1}, {Seat::seatTwo, 2}, {Seat::seatThree, 3}, {Seat::seatFour, 4}, {Seat::seatFive, 5},
    {Seat::seatSix, 6}, {Seat::seatSeven, 7}, {Seat::seatEight, 8}, {Seat::seatNine, 9}, {Seat::seatTen, 10}
  }}};
  return ENUM_TO_SIZET.at(seat) - 1;
}

/*[[nodiscard]]*/ std::string_view tableSeat::toString(Seat seat) {
  static constexpr auto ENUM_TO_STRING = language::Map<Seat, std::string_view, 10> {{{
    {Seat::seatOne, "1"}, {Seat::seatTwo, "2"}, {Seat::seatThree, "3"}, {Seat::seatFour, "4"}, {Seat::seatFive, "5"},
    {Seat::seatSix, "6"}, {Seat::seatSeven, "7"}, {Seat::seatEight, "8"}, {Seat::seatNine, "9"}, {Seat::seatTen, "10"}
  }}};
  return ENUM_TO_STRING.at(seat);
}

/*[[nodiscard]]*/ Seat tableSeat::fromInt(int i) {
  assert(0 < i and 11 > i);
  static constexpr auto INT_TO_ENUM = language::Map<int, Seat, 10> { {{
    {1, Seat::seatOne}, {2, Seat::seatTwo}, {3, Seat::seatThree}, {4, Seat::seatFour}, {5, Seat::seatFive},
    {6, Seat::seatSix}, {7, Seat::seatSeven}, {8, Seat::seatEight}, {9, Seat::seatNine}, {10, Seat::seatTen}
  }} };
  return INT_TO_ENUM.at(i);
}

/*[[nodiscard]]*/ int tableSeat::toInt(Seat seat) {
  static constexpr auto ENUM_TO_INT = language::Map<Seat, int, 10> { {{
    {Seat::seatOne, 1}, {Seat::seatTwo, 2}, {Seat::seatThree, 3}, {Seat::seatFour, 4}, {Seat::seatFive, 5},
    {Seat::seatSix, 6}, {Seat::seatSeven, 7}, {Seat::seatEight, 8}, {Seat::seatNine, 9}, {Seat::seatTen, 10}
  }}};
  return ENUM_TO_INT.at(seat);
}
