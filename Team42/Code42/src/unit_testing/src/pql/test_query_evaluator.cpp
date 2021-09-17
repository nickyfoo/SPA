#include "query_evaluator.h"
#include "query_preprocessor.h"
#include "catch.hpp"

#include "parse.h"
#include <string>
#include <vector>
#include "pkb.h"
#include "entities/statement.h"

std::string sourcePQL = "procedure main {"
                        "flag = 0;"
                        "call computeCentroid;"
                        "call printResults;"
                        "}"
                        "procedure readPoint {"
                        "read x;"
                        "read y;"
                        "}"
                        "procedure printResults {"
                        "print flag;"
                        "print cenX;"
                        "print cenY;"
                        "print normSq;"
                        "}"
                        "procedure computeCentroid {"
                        "count = 0;"
                        "cenX = 0;"
                        "cenY = 0;"
                        "call readPoint;"
                        "while((x != 0) && (y != 0)) {"
                        "count = count+1;"
                        "cenX = cenX + x;"
                        "cenY = cenY + y;"
                        "call readPoint;"
                        "}"
                        "if (count == 0) then {"
                        "flag = 1;"
                        "} else {"
                        "cenX = cenX / count;"
                        "cenY = cenY / count;"
                        "}"
                        "normSq = cenX * cenX + cenY * cenY;"
                        "}";

std::string samplePQL = "procedure Example {"
                        "x = 2;"
                        "z = 3;"
                        "i = 5;"
                        "while (i!=0) {"
                        "x = x - 1;"
                        "if (x==1) then {"
                        "z = x + 1; }"
                        "else {"
                        "y = z + x; }"
                        "z = z + x + i;"
                        "call q;"
                        "i = i - 1; }"
                        "call p; }"

                        "procedure p {"
                        "if (x<0) then {"
                        "while (i>0) {"
                        "x = z * 3 + 2 * y;"
                        "call q;"
                        "i = i - 1; }"
                        "x = x + 1;"
                        "z = x + z; }"
                        "else {"
                        "z = 1; }"
                        " z = z + x + i; }"

                        "procedure q {"
                        "if (x==1) then {"
                        "z = x + 1; }"
                        "else {"
                        "x = z + x; } }";

TEST_CASE("Test 1: Follows Synonym + Integer") {
  std::string ss = "stmt s1;\n"
                   "Select s1 such that Follows(s1, 3)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"2"};

  REQUIRE(ret->size() == expected.size());
  REQUIRE(ret->at(0) == expected.at(0));
}

TEST_CASE("Test 2: Follows Synonym + Synonym") {
  std::string ss = "stmt s1, s2;\n"
                   "Select s1 such that Follows(s1, s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "2", "4", "6", "7", "8", "10",
                                       "11", "12", "13", "14", "15", "16", "17", "19", "21"};

  REQUIRE(ret->size() == expected.size());
  REQUIRE(ret->at(0) == expected.at(0));
}

TEST_CASE("Test 3: Follows Integer + Synonym") {
  std::string ss = "stmt s1;\n"
                   "Select s1 such that Follows(11, s1)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"12"};

  REQUIRE(ret->size() == expected.size());
  REQUIRE(ret->at(0) == expected.at(0));
}

TEST_CASE("Test 4: Select") {
  std::string ss = "stmt s1;\n"
                   "Select s1";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                       "9", "10", "11", "12", "13", "14",
                                       "15", "16", "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  REQUIRE(ret->at(0) == expected.at(0));
}

TEST_CASE("Test 5: Incorrect pql input") {
  std::string ss = "stmt s1;\n"
                   "Select s1 HELLO";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
}

TEST_CASE("Test 6: Follows Unused entity declaration") {
  std::string ss = "stmt s1, s2;\n"
                   "Select s1 such that Follows(_, 3)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                       "9", "10", "11", "12", "13", "14",
                                       "15", "16", "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Test 7: Follows Integer, Wildcard") {
    std::string ss = "stmt s1, s2;\n"
                     "Select s1 such that Follows(2, _)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 8: Follows Wildcard, Integer") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(_, 3)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 9: Follows Wildcard, Wildcard") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(_, _)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 9: Follows Synonym, Wildcard") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(s1, _)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "2", "4", "6", "7", "8", "10",
                                         "11", "12", "13", "14", "15", "16",
                                         "17", "19", "21"};;

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 10: Follows* Synonym, Integer") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows*(s1, 3)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "2"};;

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 11: Follows* Integer, Integer") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows*(12, 19)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 12: Follows* Integer, Synonym") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows*(s1, 14)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"10", "11", "12", "13"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 13: Parent Integer, Integer") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Parent(14, 16)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 14: Parent Synonym, Wildcard") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Parent(s1, _)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"14", "19"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 15: Parent Integer, Integer") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Parent(19, 22)";
    auto *query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(sourcePQL.c_str());
    ParseState s{};
    ProgramNode *p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);
    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
                                         "9", "10", "11", "12", "13", "14",
                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < ret->size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 16: Sample source query 1") {
  std::string ss = "while w;\n"
                   "Select w such that Parent(w, 7)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(samplePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Test 17: Sample source query 1") {
  std::string ss = "if ifs;"
                   "Select ifs such that Follows(5, ifs)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(samplePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"6"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Test 18: Uses Synonym, Synonym") {
  std::string ss = "stmt s1; variable v;\n"
                   "Select s1 such that Uses(s1, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"2", "3", "6", "7", "8", "9", "14",
                                       "15", "16", "17", "19", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Test 19: UsesP Synonym, Synonym") {
  std::string ss = "procedure p; variable v;\n"
                   "Select p such that Uses(p, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"main", "printResults", "computeCentroid"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Test 20: Modifies Synonym, Synonym") {
  std::string ss = "stmt s1; variable v;\n"
                   "Select s1 such that Modifies(s1, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "2", "4", "5", "10", "11", "12",
                                       "13", "14", "15", "16", "17", "18",
                                       "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Test 21: Modifies Synonym, Argument") {
  std::string ss = "stmt s1;\n"
                   "Select s1 such that Modifies(s1, 'cenX')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"2", "11", "14", "16", "19", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Test 22: Sample source query 4") {
  std::string ss = "stmt s;"
                   "Select s such that Modifies(s, 'i')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(samplePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"3", "4", "11", "12", "13", "14", "17"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Test 23: ModifiesP Argument, Synonym") {
  std::string ss = "variable v;\n"
                   "Select v such that Modifies('main', v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}
