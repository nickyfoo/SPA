#include "query_evaluator.h"
#include "query_preprocessor.h"
#include "catch.hpp"

#include "parse.h"
#include <string>
#include <vector>
#include "pkb.h"

std::string source = "procedure main {"
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

TEST_CASE("PQL_FollowsAndFollows_ReturnsExpected") {

  std::string ss = "assign a; variable v;\n"
                   "Select a such that Follows(a,_) and Follows(_, a)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"11", "12", "16", "17"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_FollowsAndParent_ReturnsExpected") {

  std::string ss = "assign a; variable v;\n"
                   "Select a such that Follows(a,_) and Parent(_, a)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"15", "16", "17", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_BOOLEAN_ReturnsExpected") {

  std::string ss = "assign a; stmt s; while w;\n"
                   "Select BOOLEAN such that Parent(s,a) with s.stmt# = w.stmt#";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"TRUE"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_Tuple_ReturnsExpected") {

  std::string ss = "call c1, c2;\n"
                   "Select <c1, c2>";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"2 2", "2 3", "2 13", "2 18", "3 2", "3 3", "3 13", "3 18",
                                       "13 2", "13 3", "13 13", "13 18", "18 2", "18 3", "18 13", "18 18"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}


