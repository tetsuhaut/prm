module;

#include <boost/test/unit_test.hpp>

export module test.language.strings;

import language.strings;

BOOST_AUTO_TEST_SUITE(stringsTest)

BOOST_AUTO_TEST_CASE(stringsTest_splitEmptyShouldSucceed) {
  const auto tokens { language::strings::split("", ',') };
  BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(stringsTest_splitNonEmptyShouldSucceed) {
  const auto tokens { language::strings::split("a,b", ',') };
  BOOST_REQUIRE(tokens.size() == 2);
  BOOST_REQUIRE(tokens[0] == "a");
  BOOST_REQUIRE(tokens[1] == "b");
}

BOOST_AUTO_TEST_CASE(stringsTest_splitNonEmptyWithEndingDelimiterShouldSucceed) {
  const auto tokens { language::strings::split("a,b,", ',') };
  BOOST_REQUIRE(tokens.size() == 2);
  BOOST_REQUIRE(tokens[0] == "a");
  BOOST_REQUIRE(tokens[1] == "b");
}

BOOST_AUTO_TEST_SUITE_END()