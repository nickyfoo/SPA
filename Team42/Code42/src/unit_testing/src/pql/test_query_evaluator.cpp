#include <string>
#include <vector>

#include "catch.hpp"
#include "entities/statement.h"
#include "parse.h"
#include "pkb.h"
#include "query_evaluator.h"
#include "query_preprocessor.h"

std::string sourcePQL =
    "procedure main {"
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

std::string samplePQL =
    "procedure Example {"
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

TEST_CASE("Evaluator_FollowsSynInt_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Follows(s1, 3)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"2"};

  REQUIRE(ret->size() == expected.size());
  REQUIRE(ret->at(0) == expected.at(0));
}

TEST_CASE("Evaluator_FollowsSynSyn_ReturnsCorrect") {
  std::string ss =
      "stmt s1, s2;\n"
      "Select s1 such that Follows(s1, s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "4",  "6",  "7",  "8",  "10", "11",
                                       "12", "13", "14", "15", "16", "17", "19", "21"};

  REQUIRE(ret->size() == expected.size());
  REQUIRE(ret->at(0) == expected.at(0));
}

TEST_CASE("Evaluator_FollowsIntSyn_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Follows(11, s1)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"12"};

  REQUIRE(ret->size() == expected.size());
  REQUIRE(ret->at(0) == expected.at(0));
}

TEST_CASE("Evaluator_Select_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                                       "9",  "10", "11", "12", "13", "14", "15", "16",
                                       "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  REQUIRE(ret->at(0) == expected.at(0));
}

TEST_CASE("Evaluator_BadSynonym_ReturnsEmpty") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 HELLO";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
}

TEST_CASE("Evaluator_UnusedEntity_ReturnsCorrect") {
  std::string ss =
      "stmt s1, s2;\n"
      "Select s1 such that Follows(_, 3)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                                       "9",  "10", "11", "12", "13", "14", "15", "16",
                                       "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_FollowsIntWC_ReturnsCorrect") {
  std::string ss =
      "stmt s1, s2;\n"
      "Select s1 such that Follows(2, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                                       "9",  "10", "11", "12", "13", "14", "15", "16",
                                       "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_FollowsWCInt_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Follows(_, 3)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                                       "9",  "10", "11", "12", "13", "14", "15", "16",
                                       "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_FollowsWCWC_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Follows(_, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                                       "9",  "10", "11", "12", "13", "14", "15", "16",
                                       "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_FollowsSynWC_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Follows(s1, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "4",  "6",  "7",  "8",  "10", "11",
                                       "12", "13", "14", "15", "16", "17", "19", "21"};
  ;

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_FollowsTSynInt_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Follows*(s1, 3)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "2"};
  ;

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Evaluator_FollowsTIntInt_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Follows*(12, 19)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                                       "9",  "10", "11", "12", "13", "14", "15", "16",
                                       "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_FollowsTIntSyn_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Follows*(s1, 14)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_ParentIntInt_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Parent(14, 16)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                                       "9",  "10", "11", "12", "13", "14", "15", "16",
                                       "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_ParentSynWC_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Parent(s1, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_ParentSynInt_ReturnsEmpty") {
  std::string ss =
      "while w;\n"
      "Select w such that Parent(w, 7)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(samplePQL);
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

TEST_CASE("Evaluator_ParentIntSyn_ReturnsCorrect") {
  std::string ss =
      "if ifs;"
      "Select ifs such that Follows(5, ifs)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(samplePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"6"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("Evaluator_UsesSSynSyn_ReturnsCorrect") {
  std::string ss =
      "stmt s1; variable v;\n"
      "Select s1 such that Uses(s1, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"2",  "3",  "6",  "7",  "8",  "9",  "14",
                                       "15", "16", "17", "19", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());

  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_UsesPSynSyn_ReturnsCorrect") {
  std::string ss =
      "procedure p; variable v;\n"
      "Select p such that Uses(p, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"main", "printResults", "computeCentroid"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_ModifiesSSynSyn_ReturnsCorrect") {
  std::string ss =
      "stmt s1; variable v;\n"
      "Select s1 such that Modifies(s1, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1",  "2",  "4",  "5",  "10", "11", "12", "13", "14",
                                       "15", "16", "17", "18", "19", "20", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_ModifiesSSynArg1_ReturnsCorrect") {
  std::string ss =
      "stmt s1;\n"
      "Select s1 such that Modifies(s1, 'cenX')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"2", "11", "14", "16", "19", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_ModifiesSIntSyn_ReturnsEmpty") {
  std::string ss =
      "stmt s1; variable v;\n"
      "Select s1 such that Modifies(6, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("\"Evaluator_ModifiesSIntSyn_ReturnsEmpty") {
  std::string ss =
      "variable v;\n"
      "Select v such that Modifies(19, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"flag", "cenX", "cenY"};

  REQUIRE(ret->size() == expected.size());
}

TEST_CASE("Evaluator_ModifiesSSynArg2_ReturnsCorrect") {
  std::string ss =
      "stmt s;"
      "Select s such that Modifies(s, 'i')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(samplePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"3", "4", "11", "12", "13", "14", "17"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_ModifiesPArgSyn_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Modifies('main', v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_NextIntInt1_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Next(2, 3)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_NextIntInt2_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Next(18, 14)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_NextIntInt3_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Next(19, 11)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  REQUIRE(ret->size() == 0);
}

TEST_CASE("Evaluator_NextSynInt1_ReturnsCorrect") {
  std::string ss =
      "prog_line p;\n"
      "Select p such that Next(p, 14)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"13", "18"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_NextSynInt2_ReturnsCorrect") {
  std::string ss =
      "prog_line p;\n"
      "Select p such that Next(p, 14)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"13", "18"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_NextIntSyn_ReturnsCorrect") {
  std::string ss =
      "prog_line p;\n"
      "Select p such that Next(19, p)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"20", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_NextSynSyn_ReturnsCorrect") {
  std::string ss =
      "prog_line p1, p2;\n"
      "Select <p1, p2> such that Next(p1, p2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1 2",   "2 3",   "4 5",   "6 7",   "7 8",   "8 9",
                                       "10 11", "11 12", "12 13", "13 14", "14 15", "15 16",
                                       "16 17", "17 18", "18 14", "14 19", "19 20", "19 21",
                                       "21 22", "20 23", "22 23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_NextWCWC_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Next(_, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_NextIntWC_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Next(4, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_CallsSynSyn_ReturnsCorrect") {
  std::string ss =
      "procedure p1, p2;\n"
      "Select <p1, p2> such that Calls(p1, p2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"main computeCentroid", "main printResults",
                                       "computeCentroid readPoint"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_CallsSynArg_ReturnsCorrect") {
  std::string ss =
      "procedure p1;\n"
      "Select p1 such that Calls(p1, \"readPoint\")";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"computeCentroid"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_CallsSynWC_ReturnsCorrect") {
  std::string ss =
      "procedure p1;\n"
      "Select p1 such that Calls(p1, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"main", "computeCentroid"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_CallsArgSyn_ReturnsCorrect") {
  std::string ss =
      "procedure p1;\n"
      "Select p1 such that Calls(\"main\", p1)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"computeCentroid", "printResults"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_CallsArgArg1_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Calls(\"main\", \"computeCentroid\")";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_CallsArgArg2_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Calls(\"main\", \"readPoint\")";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  REQUIRE(ret->size() == 0);
}

TEST_CASE("Evaluator_CallsArgWC1_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Calls(\"main\", _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_CallsArgWC2_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Calls(\"readPoint\", _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  REQUIRE(ret->size() == 0);
}

TEST_CASE("Evaluator_CallsWCSyn_ReturnsCorrect") {
  std::string ss =
      "procedure p1;\n"
      "Select p1 such that Calls(_, p1)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"computeCentroid", "printResults", "readPoint"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_CallsWCArg1_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Calls(_, \"computeCentroid\")";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_CallsWCArg2_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Calls(_, \"main\")";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  REQUIRE(ret->size() == 0);
}

TEST_CASE("Evaluator_CallsWCWC_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Calls(_, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_AffectsIntInt_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Affects(10, 15)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_AffectsIntInt2_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Affects(15, 21)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_AffectsIntInt3_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Affects(16, 21)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_AffectsSynInt1_ReturnsCorrect") {
  std::string ss =
      "assign a;\n"
      "Select a such that Affects(a, 23)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"11","12","16","17","21", "22"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_AffectsSynInt2_ReturnsCorrect") {
  std::string ss =
      "stmt s;\n"
      "Select s such that Affects(s, 15)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"10", "15"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_AffectsIntSyn_ReturnsCorrect") {
  std::string ss =
      "assign a;\n"
      "Select a such that Affects(10, a)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"15", "21","22"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_AffectsSynSyn_ReturnsCorrect") {
  std::string ss =
      "prog_line n1, n2;\n"
      "Select <n1, n2> such that Affects(n1, n2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"10 15","10 21","10 22", "11 16","11 21", "11 23",
                                       "12 17","12 22", "12 23", "15 15", "15 21", "15 22", "16 16", "16 21", "16 23",
                                       "17 17", "17 22", "17 23", "21 23", "22 23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_AffectsWCWC_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Affects(_, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_AffectsIntWC_ReturnsCorrect") {
  std::string ss =
      "variable v;\n"
      "Select v such that Affects(10, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
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

TEST_CASE("Evaluator_AffectsWithWhile_ReturnsWrong") {
  std::string ss =
      "while w; variable v;\n"
      "Select v such that Affects(w, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("Evaluator_AffectsWithRead_ReturnsWrong") {
  std::string ss =
      "read r; variable v;\n"
      "Select v such that Affects(_, r)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();

  // Parse source
  BufferedLexer lexer(sourcePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  auto evaluator = new QueryEvaluator(clause, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < ret->size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}
