//module;
//
//#include "clamp-cast.hpp"
//
//export module language.limits;
//
//export namespace limits {
//[[nodiscard]] int toInt(double value);
//[[nodiscard]] int toInt(float value);
//[[nodiscard]] int toInt(std::ptrdiff_t value);
//[[nodiscard]] int toInt(auto) = delete; // forbid other types
//[[nodiscard]] std::size_t toSizeT(int value);
//[[nodiscard]] std::size_t toSizeT(std::ptrdiff_t value);
//[[nodiscard]] std::size_t toSizeT(auto) = delete; // forbid other types
//} // namespace limits
//
//module : private;
//
//[[nodiscard]] int limits::toInt(double value) {
//  return clamp_cast::clamp_cast<int>(value);
//}
//
//[[nodiscard]] int limits::toInt(float value) {
//  return clamp_cast::clamp_cast<int>(value);
//}
//
//[[nodiscard]] int limits::toInt(std::ptrdiff_t value) {
//  if (value > std::numeric_limits<int>::max()) { return std::numeric_limits<int>::max(); }
//
//  if (value < std::numeric_limits<int>::min()) { return std::numeric_limits<int>::min(); }
//
//  return static_cast<int>(value);
//}
//
//[[nodiscard]] std::size_t limits::toSizeT(int value) {
//  // int can be < 0, std::size_t can't
//  return (value < 0) ? 0 : static_cast<std::size_t>(value);
//}
//
//[[nodiscard]] std::size_t limits::toSizeT(std::ptrdiff_t value) {
//  return (value < 0) ? 0 : static_cast<std::size_t>(value);
//}
