#include "catch.hpp"

#include "test_utils.hpp"
#include <string>
#include <vector>


TEST_CASE("AffectsBip*_OneSynonym_ReturnsExpected") {
  std::string ss = "assign a;\n"
                   "Select a such that AffectsBip*(15, a)";
  std::vector<std::string> *ret = EvaluateBipQuery(ss);
  std::vector<std::string> expected = {"9", "10", "11", "14", "15", "17", "22", "24", "25"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("AffectsBip*_TwoSynonyms_ReturnsExpected") {
  std::string ss = "assign a, a1;\n"
                   "Select <a1, a> such that AffectsBip*(a1, a)";
  std::vector<std::string> *ret = EvaluateBipQuery(ss);
  std::vector<std::string> expected = {"14 10", "14 11","14 14","14 15","14 17","14 22","14 24",
  "14 25","14 9","15 10","15 11","15 14","15 15","15 17","15 22","15 24","15 25","15 9",
  "16 10","16 11","16 14","16 15","16 17","16 22","16 24","16 25","16 9","17 10","17 11",
  "18 20","19 20","21 22","24 10","24 11","24 14","24 15","24 17","24 22","24 24","24 25",
  "24 9","25 10","25 11","25 14","25 15","25 17","25 22","25 24","25 25","25 9","3 10","3 11"
  };

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("AffectsBip*_TwoArgs_ReturnsExpected") {
  std::string ss = "assign a, a1;\n"
                   "Select BOOLEAN such that AffectsBip*(15, 24)";
  std::vector<std::string> *ret = EvaluateBipQuery(ss);
  std::vector<std::string> expected = {"TRUE"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Modifies_TwoArgs_ReturnsExpected") {
  std::string ss = "stmt s, s1, s2; assign a, a1; while w, w1; if ifs, ifs1; variable v, v1; procedure p, q, p1; constant c, c1; call cl; print pn; read re; prog_line n, n1;"
                   "Select < cl.stmt#,p.procName, v.varName> such that Modifies (p, v) and Modifies (cl, v)";
  std::vector<std::string> *ret = EvaluateBipQuery(ss);
  std::vector<std::string> expected = {"12 bar area","12 bar pi","12 bar x","12 bar y",
  "12 ellipse area","12 ellipse pi","12 ellipse x","12 ellipse y","12 foo area",
  "12 foo pi","12 foo x","12 foo y","5 bar area","5 bar cos","5 bar length","5 bar pi",
  "5 bar sin","5 bar tan","5 bar x","5 bar y","5 ellipse area","5 ellipse pi",
  "5 ellipse x","5 ellipse y","5 foo area","5 foo cos","5 foo length","5 foo pi",
  "5 foo sin","5 foo tan","5 foo x","5 foo y"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}
