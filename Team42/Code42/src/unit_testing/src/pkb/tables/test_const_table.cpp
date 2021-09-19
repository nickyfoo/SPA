#include <vector>
#include "const_table.h"
#include "catch.hpp"

TEST_CASE("AddGetAllConstants_DifferentSizeTables_ReturnsVectorConstants", "[constant][table]") {
  SECTION("Table size: 1") {
    int const_value = rand();
    ConstTable const_table;
    const_table.AddConstant(const_value);
    std::vector<Constant *> const_vec = const_table.get_all_constants();

    REQUIRE(const_vec.size() == 1);
    REQUIRE(std::any_of(const_vec.begin(),
                        const_vec.end(),
                        [&](Constant *elem) { return elem->get_value() == const_value; }));
  }

  SECTION("Table size: 5") {
    std::set<int> const_values{rand(), rand(), rand(), rand(), rand()};
    ConstTable const_table;
    for (int const_value: const_values) const_table.AddConstant(const_value);
    std::vector<Constant *> const_vec = const_table.get_all_constants();

    REQUIRE(const_vec.size() == const_values.size());
    for (int const_value: const_values) {
      REQUIRE(std::any_of(const_vec.begin(),
                          const_vec.end(),
                          [&](Constant *elem) { return elem->get_value() == const_value; }));
    }
  }
}
