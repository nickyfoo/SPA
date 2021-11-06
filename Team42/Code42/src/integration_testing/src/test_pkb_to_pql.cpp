#include "catch.hpp"

#include "test_utils.hpp"
#include <string>
#include <utility>
#include <vector>

TEST_CASE("PQL_FollowsAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a such that Follows(a,_) pattern a(v, _)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_PatternAndFollows_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, _) such that Follows(a,_)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_PatternAndFollowsWithExtraWords_ReturnsEmpty") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, _) and such that Follows(a,_)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_FollowsAndPatternUnrelated_ReturnsExpected") {
  std::string ss = "assign a; variable v; while w;\n"
                   "Select w such that Follows(w,_) pattern a(v, _)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"14"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_FollowsStarAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Follows*(_,a) pattern a(_, _'cenY'_)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"17", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_ParentAndPattern_ReturnsExpected") {
  std::string ss = "stmt s; assign a; variable v; while w;\n"
                   "Select s such that Parent(s,a) pattern a(_, _'cenX'_)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"14", "19"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_ParentStarAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Parent*(_,a) pattern a(v, _'1'_)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"15", "20"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_UsesAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Uses(a, v) pattern a(_, _)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"15", "16", "17", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_ModifiesAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Modifies(a, v) pattern a(v, _'0'_) ";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"1", "10", "11", "12"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_KeywordAsSynonym_ReturnsExpected") {
  std::string ss = "assign pattern; variable v; while w;\n"
                   "Select pattern such that Follows(pattern , _) pattern pattern (v, _'cenX'_)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"16", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_RelationshipAndPatternDependencies_ReturnsExpected") {
  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Uses(a, v) pattern a(v, _)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"15", "16", "17", "21", "22"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_RelationshipAndPatternOneCommonSynonym_ReturnsExpected") {
  std::string ss = "assign a; variable v; stmt s;\n"
                   "Select s such that Follows(s, a) pattern a(v, _'count'_)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_UsesPAndPatternOneCommonSynonym_ReturnsExpected") {
  std::string ss = "assign a; variable v; procedure p;\n"
                   "Select p such that Uses(p, v) pattern a(v, _'count'_)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"main", "computeCentroid", "printResults"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("PQL_InvalidUsesAndPattern_ReturnsExpected") {
  std::string ss = "read r,r1; print pn; while w; if ifs; assign a; variable v; procedure p; stmt s; constant c;\n"
                   "Select a such that Uses(c,v) pattern a(v, _)";

  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_NoClause_ReturnsExpected") {
  std::string ss = "stmt s1;\n"
                   "Select s1";
  std::vector<std::string> *ret = EvaluateQuery(ss);
  std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                       "9", "10", "11", "12", "13", "14",
                                       "15", "16", "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("SingleClause_FollowsClause_ReturnsExpected") {
  SECTION("Follows with one left syn") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(s1, 3)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"2"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Follows with 1 right syn") {
    std::string ss = "stmt s1, s2;\n"
                     "Select s1 such that Follows(1, s1)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"2"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Follows with left wildcard and right arg") {
    std::string ss = "stmt s1, s2;\n"
                     "Select s1 such that Follows(_, 3)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Follows with left arg and right wildcard") {
    std::string ss = "stmt s1, s2;\n"
                     "Select s1 such that Follows(2, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Follows with both wildcards") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(_, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Follows with left syn and right wildcard") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(s1, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1", "2", "4", "6", "7", "8", "10",
                                         "11", "12", "13", "14", "15", "16",
                                         "17", "19", "21"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }
}

TEST_CASE("SingleClause_Follows*Clause_ReturnsExpected") {
  SECTION("Follows* with 2 args") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows*(12, 19)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Follows* with left syn and right arg") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows*(s1, 3)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1", "2"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }
}

TEST_CASE("SingleClause_ParentClause_ReturnsExpected") {
  SECTION("Parent with 2 args") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Parent(14, 16)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Parent with left syn and right arg") {
    std::string ss = "while w;\n"
                     "Select w such that Parent(w, 7)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Parent with left syn and right wildcard") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Parent(s1, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"14", "19"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }
}

TEST_CASE("SingleClause_UsesClause_ReturnsExpected") {
  SECTION("UsesS with 2 syns") {
    std::string ss = "stmt s1; variable v;\n"
                     "Select s1 such that Uses(s1, v)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"2", "3", "6", "7", "8", "9", "14",
                                         "15", "16", "17", "19", "21", "22", "23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("UsesP with 2 syns") {
    std::string ss = "procedure p; variable v;\n"
                     "Select p such that Uses(p, v)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"main", "printResults", "computeCentroid"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }
}

TEST_CASE("SingleClause_ModifiesClause_ReturnsExpected") {
  SECTION("ModifiesS with left arg and right syn") {
    std::string ss = "stmt s1; variable v;\n"
                     "Select s1 such that Modifies(6, v)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("ModifiesS with left syn and right arg") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Modifies(s1, \"cenX\")";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"2", "11", "14", "16", "19", "21"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("ModifiesS with 2 syns") {
    std::string ss = "stmt s1; variable v;\n"
                     "Select s1 such that Modifies(s1, v)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1", "2", "4", "5", "10", "11", "12",
                                         "13", "14", "15", "16", "17", "18",
                                         "19", "20", "21", "22", "23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("ModifiesP with left arg and right syn") {
    std::string ss = "variable v;\n"
                     "Select v such that Modifies(\"main\", v)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }
}

TEST_CASE("SingleClause_NextClause_ReturnsExpected") {
  SECTION("Next with 2 args") {
    std::string ss =
        "variable v;\n"
        "Select v such that Next(2, 3)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with 2 args") {
    std::string ss =
        "variable v;\n"
        "Select v such that Next(18, 14)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with 2 args") {
    std::string ss =
        "variable v;\n"
        "Select v such that Next(19, 11)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with left syn and right arg") {
    std::string ss =
        "prog_line p;\n"
        "Select p such that Next(p, 14)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"13", "18"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with left arg and right syn") {
    std::string ss =
        "prog_line p;\n"
        "Select p such that Next(19, p)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"20", "21"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with 2 syns") {
    std::string ss =
        "prog_line p1, p2;\n"
        "Select <p1, p2> such that Next(p1, p2)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1 2",   "2 3",   "4 5",   "6 7",   "7 8",   "8 9",
                                         "10 11", "11 12", "12 13", "13 14", "14 15", "15 16",
                                         "16 17", "17 18", "18 14", "14 19", "19 20", "19 21",
                                         "21 22", "20 23", "22 23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with 2 wildcards") {
    std::string ss =
        "variable v;\n"
        "Select v such that Next(_, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with left arg and right wildcard") {
    std::string ss =
        "variable v;\n"
        "Select v such that Next(4, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with left arg and right wildcard, returns empty") {
    std::string ss =
        "variable v;\n"
        "Select v such that Next(9, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with left wildcard and right arg") {
    std::string ss =
        "variable v;\n"
        "Select v such that Next(_, 14)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with left wildcard and right arg, returns empty") {
    std::string ss =
        "variable v;\n"
        "Select v such that Next(_, 10)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with left syn and right wildcard") {
    std::string ss =
        "prog_line p;\n"
        "Select p such that Next(p, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"1",  "2",  "4",  "6",  "7",  "8",  "10", "11", "12", "13",
                                         "14", "15", "16", "17", "18", "19", "20", "21", "22"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Next with left wildcard and right syn") {
    std::string ss =
        "prog_line p;\n"
        "Select p such that Next(_, p)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"2",  "3",  "5",  "7",  "8",  "9",  "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }
}

TEST_CASE("SingleClause_CallsClause_ReturnsExpected") {
  SECTION("Calls with 2 args") {
    std::string ss =
        "variable v;\n"
        "Select v such that Calls(\"main\", \"computeCentroid\")";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with 2 args, returns empty") {
    std::string ss =
        "variable v;\n"
        "Select v such that Calls(\"main\", \"readPoint\")";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with 2 syns") {
    std::string ss =
        "procedure p1, p2;\n"
        "Select <p1, p2> such that Calls(p1, p2)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"main computeCentroid", "main printResults",
                                         "computeCentroid readPoint"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with left syn and right arg") {
    std::string ss =
        "procedure p1;\n"
        "Select p1 such that Calls(p1, \"readPoint\")";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"computeCentroid"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with left syn and right wildcard") {
    std::string ss =
        "procedure p1;\n"
        "Select p1 such that Calls(p1, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"main", "computeCentroid"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with left arg and right syn") {
    std::string ss =
        "procedure p1;\n"
        "Select p1 such that Calls(\"main\", p1)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"computeCentroid", "printResults"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with left arg and right wildcard") {
    std::string ss =
        "variable v;\n"
        "Select v such that Calls(\"main\", _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with left wildcard and right arg") {
    std::string ss =
        "variable v;\n"
        "Select v such that Calls(_, \"computeCentroid\")";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with left wildcard and right arg, returns empty") {
    std::string ss =
        "variable v;\n"
        "Select v such that Calls(_, \"main\")";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with left arg and right wildcard, returns empty") {
    std::string ss =
        "variable v;\n"
        "Select v such that Calls(\"readPoint\", _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with left wildcard and right syn") {
    std::string ss =
        "procedure p1;\n"
        "Select p1 such that Calls(_, p1)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"computeCentroid", "printResults", "readPoint"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }

  SECTION("Calls with 2 wildcards") {
    std::string ss =
        "variable v;\n"
        "Select v such that Calls(_, _)";
    std::vector<std::string> *ret = EvaluateQuery(ss);

    std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
    }
  }
}

TEST_CASE("Pattern_AssignVariableAndWildCard_ReturnsExpected") {

  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, _)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, '0')";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"1", "10", "11", "12"};
  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableAndVariable_ReturnsEmpty") {
  std::string ss = "assign a; variable v1, v2; \n"
                   "Select a pattern a(v1, v2)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableAndPartialPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, _'cenX'_)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"16", "21", "23"};
  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignWildCardAndWildCard_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(_, _)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignWildCardAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(_, '0')";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"1", "10", "11", "12"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignWildCardAndPartialPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(_, _'cenX'_)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"16", "21", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_PrintWildCardAndPattern_ReturnsEmpty") {
  std::string ss = "print p; variable v;\n"
                   "Select p pattern p(_, _'cenX'_)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignWildCardAndWrongPartialPattern_ReturnsEmpty") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(_, 'cenX'_)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignStmtAndPartialPattern_ReturnsEmpty") {
  std::string ss = "assign a; stmt s;\n"
                   "Select a pattern a(s, _'cenX'_)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableNameAndWildCard_ReturnsExpected") {
  std::string ss = "assign a;\n"
                   "Select a pattern a(\"cenX\", _)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"11", "16", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableNameAndExactMatch_ReturnsExpected") {
  std::string ss = "assign a;\n"
                   "Select a pattern a(\"cenX\", \"0\")";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"11"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableNameAndPartialMatch_ReturnsExpected") {
  std::string ss = "assign a;\n"
                   "Select a pattern a(\"cenX\", _\"cenX\"_)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {"16", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableNameAndPartialMatchBad_ReturnsExpected") {
  std::string ss = "assign a;\n"
                   "Select a pattern a(_, _\"cenX cenY\"_)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableNameAndExactMatchBad_ReturnsExpected") {
  std::string ss = "assign a;\n"
                   "Select a pattern a(_, _'count 1 +'_)";
  std::vector<std::string> *ret = EvaluateQuery(ss);

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}
