#include "query_evaluator.h"
#include "query_preprocessor.h"
#include "query_optimizer.h"
#include "catch.hpp"

#include "parse.h"
#include <string>
#include <vector>
#include "pkb.h"

std::string affects_bip_source = "procedure foo {\n"
                                 "  read x;\n"
                                 "  read y;\n"
                                 "  length = x * x + y * y;\n"
                                 "  t = 30;\n"
                                 "  call bar;\n"
                                 "  print length;\n"
                                 "}\n"
                                 "\n"
                                 "procedure bar {\n"
                                 "  if ((x >= 0) && (y >= 0)) then {\n"
                                 "    while (x != 0) {\n"
                                 "      tan = y / x;\n"
                                 "      sin = y / length;\n"
                                 "      cos = x / length;\n"
                                 "      call ellipse;\n"
                                 "      if (((sin * sin + cos * cos) == 1) || (!(tan != (sin / cos)))) then {\n"
                                 "        x = y / 2;\n"
                                 "        y = x / 2;\n"
                                 "      } else {\n"
                                 "        x = 0;\n"
                                 "      }\n"
                                 "      length = x * x + y * y;\n"
                                 "    }\n"
                                 "  } else {\n"
                                 "    x = 10;\n"
                                 "    y = 10;\n"
                                 "    length = x * x + y * y;\n"
                                 "  }\n"
                                 "}\n"
                                 "\n"
                                 "procedure ellipse {\n"
                                 "  pi = 3;\n"
                                 "  area = pi * x * y;\n"
                                 "  if (area < 10) then {\n"
                                 "    x = x + 1;\n"
                                 "    y = y + y / x + 1;\n"
                                 "  } else {\n"
                                 "    print area;\n"
                                 "  }\n"
                                 "}";

TEST_CASE("AffectsBip*_OneSynonym_ReturnsExpected") {
  std::string ss = "assign a;\n"
                   "Select a such that AffectsBip*(15, a)";
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
  BufferedLexer lexer(affects_bip_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();
  for (std::string output : *ret) {
    std::cout << output << "\n";
  }
}

TEST_CASE("AffectsBip*_TwoSynonyms_ReturnsExpected") {
  std::string ss = "assign a, a1;\n"
                   "Select <a1, a> such that AffectsBip*(a1, a)";
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
  BufferedLexer lexer(affects_bip_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();
  for (std::string output : *ret) {
    std::cout << output << "\n";
  }
}

TEST_CASE("AffectsBip*_TwoArgs_ReturnsExpected") {
  std::string ss = "assign a, a1;\n"
                   "Select BOOLEAN such that AffectsBip*(15, 24)";
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
  BufferedLexer lexer(affects_bip_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();
  for (std::string output : *ret) {
    std::cout << output << "\n";
  }
}

TEST_CASE("Modifies_TwoArgs_ReturnsExpected") {
  std::string ss = "stmt s, s1, s2; assign a, a1; while w, w1; if ifs, ifs1; variable v, v1; procedure p, q, p1; constant c, c1; call cl; print pn; read re; prog_line n, n1;"
                   "Select < cl.stmt#,p.procName, v.varName> such that Modifies (p, v) and Modifies (cl, v)";
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
  BufferedLexer lexer(affects_bip_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();
  for (std::string output : *ret) {
    std::cout << output << "\n";
  }
}
