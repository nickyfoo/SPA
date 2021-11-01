#include "query_evaluator.h"
#include "query_preprocessor.h"
#include "query_optimizer.h"
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

TEST_CASE("PQL_FollowsAndPattern_ReturnsExpected") {

  std::string ss = "assign a; variable v;\n"
                   "Select a such that Follows(a,_) pattern a(v, _)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "21" };

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_PatternAndFollows_ReturnsExpected") {

  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, _) such that Follows(a,_)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "21" };

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_PatternAndFollowsWithExtraWords_ReturnsEmpty") {

  std::string ss = "assign a; variable v;\n"
                   "Select a pattern a(v, _) and such that Follows(a,_)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_FollowsAndPatternUnrelated_ReturnsExpected") {

  std::string ss = "assign a; variable v; while w;\n"
                   "Select w such that Follows(w,_) pattern a(v, _)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"14"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_FollowsStarAndPattern_ReturnsExpected") {

  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Follows*(_,a) pattern a(_, _'cenY'_)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"17", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_ParentAndPattern_ReturnsExpected") {

  std::string ss = "stmt s; assign a; variable v; while w;\n"
                   "Select s such that Parent(s,a) pattern a(_, _'cenX'_)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"14", "19"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_ParentStarAndPattern_ReturnsExpected") {

  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Parent*(_,a) pattern a(v, _'1'_)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"15", "20"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_UsesAndPattern_ReturnsExpected") {

  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Uses(a, v) pattern a(_, _)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"15", "16", "17", "21", "22", "23"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_ModifiesAndPattern_ReturnsExpected") {

  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Modifies(a, v) pattern a(v, _'0'_) ";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"1", "10", "11", "12"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_KeywordAsSynonym_ReturnsExpected") {

  std::string ss = "assign pattern; variable v; while w;\n"
                   "Select pattern such that Follows(pattern , _) pattern pattern (v, _'cenX'_)";

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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"16", "21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_RelationshipAndPatternDependencies_ReturnsExpected") {

  std::string ss = "assign a; variable v; while w;\n"
                   "Select a such that Uses(a, v) pattern a(v, _)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"15", "16", "17", "21", "22"};
  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_RelationshipAndPatternOneCommonSynonym_ReturnsExpected") {

  std::string ss = "assign a; variable v; stmt s;\n"
                   "Select s such that Follows(s, a) pattern a(v, _'count'_)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"21"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}

TEST_CASE("PQL_UsesPAndPatternOneCommonSynonym_ReturnsExpected") {

  std::string ss = "assign a; variable v; procedure p;\n"
                   "Select p such that Uses(p, v) pattern a(v, _'count'_)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  std::vector<std::string> expected = {"main", "computeCentroid", "printResults"};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(std::find(expected.begin(), expected.end(), ret->at(i)) != expected.end());
  }
}

TEST_CASE("PQL_InvalidUsesAndPattern_ReturnsExpected") {
  std::string ss = "read r,r1; print pn; while w; if ifs; assign a; variable v; procedure p; stmt s; constant c;\n"
                   "Select a such that Uses(c,v) pattern a(v, _)";
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
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clause),
                                     clause_groups,
                                     std::get<4>(*clause),
                                     std::get<5>(*clause));

  // Parse source
  BufferedLexer lexer(s);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();
  std::vector<std::string> expected = {};

  REQUIRE(ret->size() == expected.size());
  for (int i = 0; i < expected.size(); i++) {
    REQUIRE(ret->at(i) == expected.at(i));
  }
}
