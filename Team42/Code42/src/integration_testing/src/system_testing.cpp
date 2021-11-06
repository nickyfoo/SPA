#include "catch.hpp"

#include <string>
#include <vector>
#include "test_utils.hpp"

TEST_CASE("PQL_FollowsAndFollows_ReturnsExpected") {

  std::string ss = "assign a; variable v;\n"
                   "Select a such that Follows(a,_) and Follows(_, a)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"11", "12", "16", "17"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_FollowsAndParent_ReturnsExpected") {

  std::string ss = "assign a; variable v;\n"
                   "Select a such that Follows(a,_) and Parent(_, a)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"15", "16", "17", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_BOOLEAN_ReturnsExpected") {

  std::string ss = "assign a; stmt s; while w;\n"
                   "Select BOOLEAN such that Parent(s,a) with s.stmt# = w.stmt#";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"TRUE"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_Tuple_ReturnsExpected") {

  std::string ss = "call c1, c2;\n"
                   "Select <c1, c2>";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"2 2", "2 3", "2 13", "2 18", "3 2", "3 3", "3 13", "3 18",
                                       "13 2", "13 3", "13 13", "13 18", "18 2", "18 3", "18 13",
                                       "18 18"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}


