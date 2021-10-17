#include <string>
#include <vector>

#include "catch.hpp"
#include "entities/statement.h"
#include "parse.h"
#include "pkb.h"
#include "query_optimizer.h"
#include "query_evaluator.h"
#include "query_preprocessor.h"

//std::string sourcePQL = "procedure main {"
//                        "flag = 0;"
//                        "call computeCentroid;"
//                        "call printResults;"
//                        "}"
//                        "procedure readPoint {"
//                        "read x;"
//                        "read y;"
//                        "}"
//                        "procedure printResults {"
//                        "print flag;"
//                        "print cenX;"
//                        "print cenY;"
//                        "print normSq;"
//                        "}"
//                        "procedure computeCentroid {"
//                        "count = 0;"
//                        "cenX = 0;"
//                        "cenY = 0;"
//                        "call readPoint;"
//                        "while((x != 0) && (y != 0)) {"
//                        "count = count+1;"
//                        "cenX = cenX + x;"
//                        "cenY = cenY + y;"
//                        "call readPoint;"
//                        "}"
//                        "if (count == 0) then {"
//                        "flag = 1;"
//                        "} else {"
//                        "cenX = cenX / count;"
//                        "cenY = cenY / count;"
//                        "}"
//                        "normSq = cenX * cenX + cenY * cenY;"
//                        "}";
//
//std::string samplePQL =
//    "procedure Example {"
//    "x = 2;"
//    "z = 3;"
//    "i = 5;"
//    "while (i!=0) {"
//    "x = x - 1;"
//    "if (x==1) then {"
//    "z = x + 1; }"
//    "else {"
//    "y = z + x; }"
//    "z = z + x + i;"
//    "call q;"
//    "i = i - 1; }"
//    "call p; }"
//
//    "procedure p {"
//    "if (x<0) then {"
//    "while (i>0) {"
//    "x = z * 3 + 2 * y;"
//    "call q;"
//    "i = i - 1; }"
//    "x = x + 1;"
//    "z = x + z; }"
//    "else {"
//    "z = 1; }"
//    " z = z + x + i; }"
//
//    "procedure q {"
//    "if (x==1) then {"
//    "z = x + 1; }"
//    "else {"
//    "x = z + x; } }";

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
                        "i = i - 1; } }"

                        "procedure q {"
                        "if (x==1) then {"
                        "z = x + 1; }"
                        "else {"
                        "x = z + x; } }";

TEST_CASE("Test 1: Follows Synonym + Integer") {
  std::string ss = "stmt s1;\n"
                   "Select s1 such that Follows(s1, 3)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  QueryOptimizer query_optimizer = QueryOptimizer(std::get<1>(*clause),
                                                  std::get<2>(*clause),
                                                  std::get<3>(*clause),
                                                  std::get<0>(*clause));
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                           clause_groups,
                           std::get<4>(*clause),
                           std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(samplePQL);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator.Evaluate();

  // printing statements
  for (Statement *stmt : pkb.get_all_statements()) {
    stmt->FollowsInfo();
    stmt->ParentInfo();
    stmt->ModifiesInfo();
    stmt->UsesInfo();
  }

  // printing procedures
  for (Procedure *proc : pkb.get_all_procedures()) {
    proc->UsesInfo();
    proc->ModifiesInfo();
    proc->CallsInfo();
  }

  // printing variables
  for (Variable *var : pkb.get_all_variables()) {
    std::cout << "Variable index" << var->get_index() << " \n";
    var->ModifiesInfo();
    var->UsesInfo();
  }

  for (Constant *cons : pkb.get_all_constants()) {
    std::cout << "Constant value " << cons->get_value() << "\n";
  }
}

TEST_CASE("Test 2: Follows Synonym + Synonym") {
  std::string ss = "stmt s1, s2;\n"
                   "Select s1 such that Follows(1, s1)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  QueryOptimizer query_optimizer = QueryOptimizer(std::get<1>(*clause),
                                                  std::get<2>(*clause),
                                                  std::get<3>(*clause),
                                                  std::get<0>(*clause));
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  std::vector<std::string> expected = {"2"};

//  REQUIRE(ret->size() == expected.size());
//  REQUIRE(ret->at(0) == expected.at(0));
}

//TEST_CASE("Test 3: Follows Integer + Synonym") {
//  std::string ss = "stmt s1;\n"
//                   "Select s1 such that Follows(11, s1)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"12"};
//
//  REQUIRE(ret->size() == expected.size());
//  REQUIRE(ret->at(0) == expected.at(0));
//}
//
//TEST_CASE("Test 4: Select") {
//  std::string ss = "stmt s1;\n"
//                   "Select s1";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
//                                       "9", "10", "11", "12", "13", "14",
//                                       "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//  REQUIRE(ret->size() == expected.size());
//  REQUIRE(ret->at(0) == expected.at(0));
//}
//
//TEST_CASE("Test 5: Incorrect pql input") {
//  std::string ss = "stmt s1;\n"
//                   "Select s1 HELLO";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {};
//
//  REQUIRE(ret->size() == expected.size());
//}
//
//TEST_CASE("Test 6: Follows Unused entity declaration") {
//  std::string ss = "stmt s1, s2;\n"
//                   "Select s1 such that Follows(_, 3)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
//                                       "9", "10", "11", "12", "13", "14",
//                                       "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Test 7: Follows Integer, Wildcard") {
//    std::string ss = "stmt s1, s2;\n"
//                     "Select s1 such that Follows(2, _)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
//                                         "9", "10", "11", "12", "13", "14",
//                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 8: Follows Wildcard, Integer") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Follows(_, 3)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
//                                         "9", "10", "11", "12", "13", "14",
//                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 9: Follows Wildcard, Wildcard") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Follows(_, _)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
//                                         "9", "10", "11", "12", "13", "14",
//                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 9: Follows Synonym, Wildcard") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Follows(s1, _)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"1", "2", "4", "6", "7", "8", "10",
//                                         "11", "12", "13", "14", "15", "16",
//                                         "17", "19", "21"};;
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 10: Follows* Synonym, Integer") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Follows*(s1, 3)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"1", "2"};;
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 11: Follows* Integer, Integer") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Follows*(12, 19)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
//                                         "9", "10", "11", "12", "13", "14",
//                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 12: Follows* Integer, Synonym") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Follows*(s1, 14)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"10", "11", "12", "13"};
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 13: Parent Integer, Integer") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Parent(14, 16)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
//                                         "9", "10", "11", "12", "13", "14",
//                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 14: Parent Synonym, Wildcard") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Parent(s1, _)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"14", "19"};
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 15: Parent Integer, Integer") {
//    std::string ss = "stmt s1;\n"
//                     "Select s1 such that Parent(19, 22)";
//    auto *query = new QueryPreprocessor(ss);
//    PQLQuery *clause = query->get_pql_query();
//
//    // Parse source
//    BufferedLexer lexer(sourcePQL);
//    ParseState s{};
//    ProgramNode *p = ParseProgram(&lexer, &s);
//    PKB pkb = PKB(p);
//    auto evaluator = new QueryEvaluator(clause, &pkb);
//    std::vector<std::string> *ret = evaluator->Evaluate();
//
//    std::vector<std::string> expected = {"1", "2", "3", "4", "5", "6", "7", "8",
//                                         "9", "10", "11", "12", "13", "14",
//                                         "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//    REQUIRE(ret->size() == expected.size());
//    for (int i = 0; i < ret->size(); i++) {
//        REQUIRE(ret->at(i) == expected.at(i));
//    }
//}
//
//TEST_CASE("Test 16: Sample source query 1") {
//  std::string ss = "while w;\n"
//                   "Select w such that Parent(w, 7)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(samplePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Test 17: Sample source query 1") {
//  std::string ss = "if ifs;"
//                   "Select ifs such that Follows(5, ifs)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(samplePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"6"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Test 18: Uses Synonym, Synonym") {
//  std::string ss = "stmt s1; variable v;\n"
//                   "Select s1 such that Uses(s1, v)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"2", "3", "6", "7", "8", "9", "14",
//                                       "15", "16", "17", "19", "21", "22", "23"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Test 19: UsesP Synonym, Synonym") {
//  std::string ss = "procedure p; variable v;\n"
//                   "Select p such that Uses(p, v)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"main", "printResults", "computeCentroid"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Test 20: Modifies Synonym, Synonym") {
//  std::string ss = "stmt s1; variable v;\n"
//                   "Select s1 such that Modifies(s1, v)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"1", "2", "4", "5", "10", "11", "12",
//                                       "13", "14", "15", "16", "17", "18",
//                                       "19", "20", "21", "22", "23"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Test 21: Modifies Synonym, Argument") {
//  std::string ss = "stmt s1;\n"
//                   "Select s1 such that Modifies(s1, 'cenX')";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"2", "11", "14", "16", "19", "21"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Invalid Modifies print stmt, Argument") {
//  std::string ss = "stmt s1; variable v;\n"
//                   "Select s1 such that Modifies(6, v)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Invalid Modifies read stmt, Argument") {
//  std::string ss = "variable v;\n"
//                   "Select v such that Modifies(19, v)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"flag", "cenX", "cenY"};
//
//  REQUIRE(ret->size() == expected.size());
//}
//
//TEST_CASE("Test 22: Sample source query 4") {
//  std::string ss = "stmt s;"
//                   "Select s such that Modifies(s, 'i')";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(samplePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"3", "4", "11", "12", "13", "14", "17"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(ret->at(i) == expected.at(i));
//  }
//}
//
//TEST_CASE("Test 23: ModifiesP Argument, Synonym") {
//  std::string ss = "variable v;\n"
//                   "Select v such that Modifies('main', v)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}

//TEST_CASE("Test 24: Next Integer, Integer") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Next(2, 3)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 25: Next Integer, Integer") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Next(18, 14)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 26: Next Integer, Integer") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Next(19, 11)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  REQUIRE(ret->size() == 0);
//}
//
//TEST_CASE("Test 27: Next Synonym, Integer") {
//  std::string ss =
//      "prog_line p;\n"
//      "Select p such that Next(p, 14)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"13", "18"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 28: Next Synonym, Integer") {
//  std::string ss =
//      "prog_line p;\n"
//      "Select p such that Next(p, 14)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"13", "18"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 29: Next Integer, Synonym") {
//  std::string ss =
//      "prog_line p;\n"
//      "Select p such that Next(19, p)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"20", "21"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 29: Next Synonym, Synonym") {
//  std::string ss =
//      "prog_line p1, p2;\n"
//      "Select <p1, p2> such that Next(p1, p2)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"1 2",   "2 3",   "4 5",   "6 7",   "7 8",   "8 9",
//                                       "10 11", "11 12", "12 13", "13 14", "14 15", "15 16",
//                                       "16 17", "17 18", "18 14", "14 19", "19 20", "19 21",
//                                       "21 22", "20 23", "22 23"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 30: Next Wildcard, Wildcard") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Next(_, _)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 31: Next Integer, Wildcard") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Next(4, _)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 32: Next Integer, Wildcard") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Next(9, _)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  REQUIRE(ret->size() == 0);
//}
//
//TEST_CASE("Test 32: Next Wildcard, Integer") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Next(_, 14)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"x", "y", "count", "cenX", "cenY", "flag", "normSq"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 33: Next Wilcard, Integer") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Next(_, 10)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  REQUIRE(ret->size() == 0);
//}
//
//TEST_CASE("Test 34: Next Synonym, Wildcard") {
//  std::string ss =
//      "prog_line p;\n"
//      "Select p such that Next(p, _)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"1",  "2",  "4",  "6",  "7",  "8",  "10", "11", "12", "13",
//                                       "14", "15", "16", "17", "18", "19", "20", "21", "22"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 35: Next Wildcard, Synonym") {
//  std::string ss =
//      "prog_line p;\n"
//      "Select p such that Next(_, p)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"2",  "3",  "5",  "7",  "8",  "9",  "11", "12", "13", "14",
//                                       "15", "16", "17", "18", "19", "20", "21", "22", "23"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 36: Calls Synonym, Synonym") {
//  std::string ss =
//      "procedure p1, p2;\n"
//      "Select <p1, p2> such that Calls(p1, p2)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"main computeCentroid", "main printResults",
//                                       "computeCentroid readPoint"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 37: Calls Synonym, Argument") {
//  std::string ss =
//      "procedure p1;\n"
//      "Select p1 such that Calls(p1, \"readPoint\")";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"computeCentroid"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 38: Calls Synonym, Wildcard") {
//  std::string ss =
//      "procedure p1;\n"
//      "Select p1 such that Calls(p1, _)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"main", "computeCentroid"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 39: Calls Argument, Synonym") {
//  std::string ss =
//      "procedure p1;\n"
//      "Select p1 such that Calls(\"main\", p1)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"computeCentroid", "printResults"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 40: Calls Argument, Argument") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Calls(\"main\", \"computeCentroid\")";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 41: Calls Argument, Argument") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Calls(\"main\", \"readPoint\")";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  REQUIRE(ret->size() == 0);
//}
//
//TEST_CASE("Test 42: Calls Argument, Wildcard") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Calls(\"main\", _)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 43: Calls Argument, Wildcard") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Calls(\"readPoint\", _)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  REQUIRE(ret->size() == 0);
//}
//
//TEST_CASE("Test 44: Calls Wildcard, Synonym") {
//  std::string ss =
//      "procedure p1;\n"
//      "Select p1 such that Calls(_, p1)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"computeCentroid", "printResults", "readPoint"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 45: Calls Wildcard, Argument") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Calls(_, \"computeCentroid\")";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
//
//TEST_CASE("Test 46: Calls Wildcard, Argument") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Calls(_, \"main\")";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  REQUIRE(ret->size() == 0);
//}
//
//TEST_CASE("Test 47: Calls Wildcard, Wildcard") {
//  std::string ss =
//      "variable v;\n"
//      "Select v such that Calls(_, _)";
//  auto *query = new QueryPreprocessor(ss);
//  PQLQuery *clause = query->get_pql_query();
//
//  // Parse source
//  BufferedLexer lexer(sourcePQL);
//  ParseState s{};
//  ProgramNode *p = ParseProgram(&lexer, &s);
//  PKB pkb = PKB(p);
//  auto evaluator = new QueryEvaluator(clause, &pkb);
//  std::vector<std::string> *ret = evaluator->Evaluate();
//
//  std::vector<std::string> expected = {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
//
//  REQUIRE(ret->size() == expected.size());
//  for (int i = 0; i < ret->size(); i++) {
//    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
//  }
//}
