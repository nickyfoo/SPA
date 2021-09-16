#include "query_evaluator.h"
#include "query_preprocessor.h"
#include "catch.hpp"

#include "parse.h"
#include <string>
#include <vector>
#include "pkb.h"

std::string s = "procedure main {"
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

TEST_CASE("Pattern_AssignVariableAndWildCard_ReturnsExpected") {

  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, '0')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "10", "11", "12"};
  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableAndVariable_ReturnsEmpty") {
  std::string ss = "assign a; variable v1, v2; \n"
                   "Select a pattern a(v1, v2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignVariableAndPartialPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, _'cenX'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"16", "21", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignWildCardAndWildCard_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(_, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignWildCardAndPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(_, '0')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "10", "11", "12"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignWildCardAndPartialPattern_ReturnsExpected") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(_, _'cenX'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"16", "21", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_PrintWildCardAndPattern_ReturnsEmpty") {
  std::string ss = "print p; variable v;\n"
                   "Select p pattern p(_, _'cenX'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignWildCardAndWrongPartialPattern_ReturnsEmpty") {
  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(_, 'cenX'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Pattern_AssignStmtAndPartialPattern_ReturnsEmpty") {
  std::string ss = "assign a; stmt s;\n"
                   "Select a pattern a(s, _'cenX'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(s.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}
