//module;
//
//#include "strings/StringLiteral.hpp"
//#include <optional> // std::in_place_t, std::in_place
//
//export module language.Either;
//
//export namespace language {
//template <typename LEFT, typename RIGHT>
//class [[nodiscard]] Either final {
//private:
//  template<typename T> struct [[nodiscard]] Left final {
//    Left(T v) : m_value(v) {}
//    T m_value;
//  };
//
//  template<typename T> struct [[nodiscard]] Right final {
//    Right(T v) : m_value(v) {}
//    T m_value;
//  };
//  std::optional<LEFT> m_left;
//  std::optional<RIGHT> m_right;
//  explicit Either(Left<LEFT> l) : m_left { l.m_value }, m_right {} {}
//  explicit Either(Right<RIGHT> r) : m_left {}, m_right { r.m_value } {}
//public:
//  [[nodiscard]] static Either<LEFT, RIGHT> left(LEFT arg) { return Either(Left(arg)); }
//  [[nodiscard]] static Either<LEFT, RIGHT> right(RIGHT arg) { return Either(Right(arg)); }
//  [[nodiscard]] constexpr bool isLeft() const noexcept { return m_left.has_value(); }
//  [[nodiscard]] constexpr bool isRight() const noexcept { return m_right.has_value(); }
//  [[nodiscard]] constexpr const LEFT& getLeft() const { return m_left.value();  }
//  [[nodiscard]] constexpr LEFT& getLeft() { return m_left.value(); }
//  [[nodiscard]] constexpr const RIGHT& getRight() const { return m_right.value(); }
//  [[nodiscard]] constexpr RIGHT& getRight() { return m_right.value(); }
//}; // class Either
//
///**
// * Contains a result or an error message.
// */
//template <typename RESULT>
//class [[nodiscard]] ErrOrRes final {
//private:
//  std::optional<String> m_err;
//  std::optional<RESULT> m_res;
//  // builds a result
//  explicit ErrOrRes(RESULT r) : m_err {}, m_res { r } {}
//  // builds an error message. we use std::in_place_t to disambiguate in case
//  // RESULT is constructible from StringView
//  ErrOrRes(std::in_place_t, StringView s) : m_err { s }, m_res {} {}
//
//public:
//  template<StringLiteral ERR_MSG>
//  [[nodiscard]] static ErrOrRes<RESULT> err() { return ErrOrRes(std::in_place, ERR_MSG.value); }
//  [[nodiscard]] static ErrOrRes<RESULT> err(StringView s) { return ErrOrRes(std::in_place, s); }
//  [[nodiscard]] static ErrOrRes<RESULT> res(RESULT arg) { return ErrOrRes(arg); }
//  [[nodiscard]] constexpr bool isErr() const noexcept { return m_err.has_value(); }
//  [[nodiscard]] constexpr bool isRes() const noexcept { return m_res.has_value(); }
//  [[nodiscard]] constexpr String getErr() const { return m_err.value(); }
//  [[nodiscard]] constexpr const RESULT& getRes() const { return m_res.value(); }
//  [[nodiscard]] constexpr RESULT& getRes() { return m_res.value(); }
//}; // class ErrOrRes
//} // namespace language