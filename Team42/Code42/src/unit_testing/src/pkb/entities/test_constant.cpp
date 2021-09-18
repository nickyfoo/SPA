#include <vector>
#include "pkb.h"
#include "catch.hpp"

TEST_CASE("GetValue_Value_ReturnsValue", "[constant][entity]") {
  SECTION("Test values up to 10") {
    for (int i = 0; i < 10; i++) {
      Constant c(i);
      REQUIRE(c.get_value() == i);
    }
  }

  SECTION("Test values multiples of 10 up till 100") {
    for (int i = 10; i < 100; i += 10) {
      Constant c(i);
      REQUIRE(c.get_value() == i);
    }
  }
}
