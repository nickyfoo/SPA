#include "query_evaluator.h"
#include "query_preprocessor.h"
#include "query_optimizer.h"
#include "catch.hpp"

#include "parse.h"
#include <string>
#include <vector>
#include "pkb.h"
#include "entities/statement.h"

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
class StatementTableStub : public StmtTable {
 protected:
  std::map<int, Statement> table_;

 public:
  void AddStmt(Statement statement) {
    table_[statement.get_stmt_no()] = statement;
  }

  std::vector<Statement *> get_all_statements() override {
    std::vector<Statement *> ans;
    for (auto &[stmt_no, stmt] : table_) {
      ans.push_back(&stmt);
    }
    return ans;
  }
};

class ProcedureTableStub : public ProcTable {
 protected:
  std::vector<Procedure> table_;
  std::map<std::string, int> name_to_index_;

 public:
  void AddProc(Procedure proc) {
    int pos = table_.size();
    table_.push_back(proc);
    name_to_index_[proc.get_name()] = pos;
  }
};

class VarTableStub : public VarTable {
 protected:
  std::vector<Variable> table_;
  std::map<std::string, int> name_to_index_;

 public:
  void AddVar(Variable variable) {
    int pos = table_.size();
    table_.push_back(variable);
    name_to_index_[variable.get_name()] = pos;
  }
};

class PKBStub : public PKB {
 protected:
  ProcedureTableStub proc_table_;
  StatementTableStub stmt_table_;
  VarTableStub var_table_;
  ConstTable const_table_;
 public:
  void set_stmt_table(StatementTableStub stmt_table) {
    stmt_table_ = stmt_table;
  }
  void set_proc_table(ProcedureTableStub proc_table) {
    proc_table_ = proc_table;
  }
  void set_var_table(VarTableStub var_table) {
    var_table_ = var_table;
  }
  void set_const_table(ConstTable const_table) {
    const_table_ = const_table;
  }
};

PKBStub *BuildPKB() {
  PKBStub *pkb_stub = new PKBStub();
  StatementTableStub stmt_table = StatementTableStub();
  ProcedureTableStub proc_table = ProcedureTableStub();
  VarTableStub var_table = VarTableStub();
  ConstTable const_table = ConstTable();

  // Creating Statements and adding to stmt table
  Statement stmt1 = Statement(1, NodeType::Assign);
  stmt1.AddFollower(2);
  stmt1.AddFollowerStar(2);
  stmt1.AddFollowerStar(3);
  stmt1.AddFollowerStar(4);
  stmt1.AddModifies("x");
  stmt_table.AddStmt(stmt1);

  Statement stmt2 = Statement(2, NodeType::Assign);
  stmt2.AddFollower(3);
  stmt2.AddFollowerStar(3);
  stmt2.AddFollowerStar(4);
  stmt2.AddFollowee(1);
  stmt2.AddFollowerStar(1);
  stmt2.AddModifies("z");
  stmt_table.AddStmt(stmt2);

  Statement stmt3 = Statement(3, NodeType::Assign);
  stmt3.AddFollower(4);
  stmt3.AddFollowerStar(4);
  stmt3.AddFollowee(2);
  stmt3.AddFolloweeStar(1);
  stmt3.AddFolloweeStar(2);
  stmt3.AddModifies("i");
  stmt_table.AddStmt(stmt3);

  Statement stmt4 = Statement(4, NodeType::While);
  stmt4.AddFollowee(3);
  stmt4.AddFolloweeStar(1);
  stmt4.AddFolloweeStar(2);
  stmt4.AddFolloweeStar(3);
  stmt4.AddChild(5);
  stmt4.AddChild(6);
  stmt4.AddChild(9);
  stmt4.AddChild(10);
  stmt4.AddChild(11);
  stmt4.AddChildStar(5);
  stmt4.AddChildStar(6);
  stmt4.AddChildStar(7);
  stmt4.AddChildStar(8);
  stmt4.AddChildStar(9);
  stmt4.AddChildStar(10);
  stmt4.AddChildStar(11);
  stmt4.AddModifies("i");
  stmt4.AddModifies("x");
  stmt4.AddModifies("y");
  stmt4.AddModifies("z");
  stmt4.AddUses("i");
  stmt4.AddUses("x");
  stmt4.AddUses("z");
  stmt_table.AddStmt(stmt4);

  Statement stmt5 = Statement(5, NodeType::Assign);
  stmt5.AddFollower(6);
  stmt5.AddFollowerStar(6);
  stmt5.AddFollowerStar(9);
  stmt5.AddFollowerStar(10);
  stmt5.AddFollowerStar(11);
  stmt5.AddParent(4);
  stmt5.AddParentStar(4);
  stmt5.AddModifies("x");
  stmt5.AddUses("x");
  stmt_table.AddStmt(stmt5);

  Statement stmt6 = Statement(6, NodeType::If);
  stmt6.AddFollower(9);
  stmt6.AddFollowerStar(9);
  stmt6.AddFollowerStar(10);
  stmt6.AddFollowerStar(11);
  stmt6.AddFollowee(5);
  stmt6.AddFolloweeStar(5);
  stmt6.AddParent(4);
  stmt6.AddParentStar(4);
  stmt6.AddChild(7);
  stmt6.AddChild(8);
  stmt6.AddChildStar(7);
  stmt6.AddChildStar(8);
  stmt6.AddModifies("y");
  stmt6.AddModifies("z");
  stmt6.AddUses("x");
  stmt6.AddUses("z");
  stmt_table.AddStmt(stmt6);

  Statement stmt7 = Statement(7, NodeType::Assign);
  stmt7.AddParent(6);
  stmt7.AddParentStar(4);
  stmt7.AddParentStar(6);
  stmt7.AddModifies("z");
  stmt7.AddUses("x");
  stmt_table.AddStmt(stmt7);

  Statement stmt8 = Statement(8, NodeType::Assign);
  stmt8.AddParent(6);
  stmt8.AddParentStar(4);
  stmt8.AddParentStar(6);
  stmt8.AddModifies("y");
  stmt8.AddUses("x");
  stmt8.AddUses("z");
  stmt_table.AddStmt(stmt8);

  Statement stmt9 = Statement(9, NodeType::Assign);
  stmt9.AddFollower(10);
  stmt9.AddFollowerStar(10);
  stmt9.AddFollowerStar(11);
  stmt9.AddFollowee(6);
  stmt9.AddFolloweeStar(5);
  stmt9.AddFolloweeStar(6);
  stmt9.AddParent(4);
  stmt9.AddParentStar(4);
  stmt9.AddModifies("z");
  stmt9.AddUses("i");
  stmt9.AddUses("x");
  stmt9.AddUses("z");
  stmt_table.AddStmt(stmt9);

  Statement stmt10 = Statement(10, NodeType::Call);
  stmt10.AddFollower(11);
  stmt10.AddFollowerStar(11);
  stmt10.AddFollowee(9);
  stmt10.AddFolloweeStar(5);
  stmt10.AddFolloweeStar(6);
  stmt10.AddFolloweeStar(9);
  stmt10.AddParent(4);
  stmt10.AddParentStar(4);
  stmt10.AddModifies("x");
  stmt10.AddModifies("z");
  stmt10.AddUses("x");
  stmt10.AddUses("z");
  stmt_table.AddStmt(stmt10);

  Statement stmt11 = Statement(11, NodeType::Assign);
  stmt11.AddFollowee(10);
  stmt11.AddFolloweeStar(5);
  stmt11.AddFolloweeStar(6);
  stmt11.AddFolloweeStar(9);
  stmt11.AddFolloweeStar(10);
  stmt11.AddParent(4);
  stmt11.AddParentStar(4);
  stmt11.AddModifies("i");
  stmt11.AddUses("i");
  stmt_table.AddStmt(stmt11);

  Statement stmt12 = Statement(12, NodeType::If);
  stmt12.AddChild(13);
  stmt12.AddChild(14);
  stmt12.AddChildStar(13);
  stmt12.AddChildStar(14);
  stmt12.AddModifies("x");
  stmt12.AddModifies("z");
  stmt12.AddUses("x");
  stmt12.AddUses("z");
  stmt_table.AddStmt(stmt12);

  Statement stmt13 = Statement(13, NodeType::Assign);
  stmt13.AddParent(12);
  stmt13.AddParentStar(12);
  stmt13.AddModifies("z");
  stmt13.AddUses("x");
  stmt_table.AddStmt(stmt13);

  Statement stmt14 = Statement(14, NodeType::Assign);
  stmt14.AddParent(12);
  stmt14.AddParentStar(12);
  stmt14.AddModifies("x");
  stmt14.AddUses("x");
  stmt14.AddUses("z");
  stmt_table.AddStmt(stmt14);

  stmt_table.CategorizeStatements();

  // Creating procedures and adding to proc table.
  Procedure example = Procedure("Example");
  example.AddUses("i");
  example.AddUses("x");
  example.AddUses("z");
  example.AddModifies("i");
  example.AddModifies("x");
  example.AddModifies("y");
  example.AddModifies("z");
  example.AddCalls("q");
  example.AddCallsStar("q");
  proc_table.AddProc(example);

  Procedure q = Procedure("q");
  q.AddUses("x");
  q.AddUses("z");
  q.AddModifies("x");
  q.AddModifies("z");
  q.AddCallers("Example");
  q.AddCallersStar("Example");
  proc_table.AddProc(q);

  // Adding variables to var table
  Variable x = Variable(0, "x");
  x.AddStmtModifying(1);
  x.AddStmtModifying(4);
  x.AddStmtModifying(5);
  x.AddStmtModifying(10);
  x.AddStmtModifying(12);
  x.AddStmtModifying(14);
  x.AddProcModifying("Example");
  x.AddProcModifying("q");
  x.AddStmtUsing(4);
  x.AddStmtUsing(5);
  x.AddStmtUsing(6);
  x.AddStmtUsing(7);
  x.AddStmtUsing(8);
  x.AddStmtUsing(9);
  x.AddStmtUsing(10);
  x.AddStmtUsing(12);
  x.AddStmtUsing(13);
  x.AddStmtUsing(14);
  x.AddProcUsing("Example");
  x.AddProcUsing("q");
  var_table.AddVar(x);

  Variable z = Variable(1, "z");
  z.AddStmtModifying(2);
  z.AddStmtModifying(4);
  z.AddStmtModifying(6);
  z.AddStmtModifying(7);
  z.AddStmtModifying(9);
  z.AddStmtModifying(10);
  z.AddStmtModifying(12);
  z.AddStmtModifying(13);
  z.AddProcModifying("Example");
  z.AddProcModifying("q");
  z.AddStmtUsing(4);
  z.AddStmtUsing(6);
  z.AddStmtUsing(8);
  z.AddStmtUsing(9);
  z.AddStmtUsing(10);
  z.AddStmtUsing(12);
  z.AddStmtUsing(14);
  z.AddProcUsing("Example");
  z.AddProcUsing("q");
  var_table.AddVar(z);

  Variable i = Variable(2, "i");
  i.AddStmtModifying(3);
  i.AddStmtModifying(4);
  i.AddStmtModifying(11);
  i.AddProcModifying("Example");
  i.AddStmtUsing(4);
  i.AddStmtUsing(9);
  i.AddStmtUsing(11);
  i.AddProcUsing("Example");
  var_table.AddVar(i);

  Variable y = Variable(3, "y");
  y.AddStmtModifying(4);
  y.AddStmtModifying(6);
  y.AddStmtModifying(8);
  y.AddProcModifying("Example");
  var_table.AddVar(y);

  // Adding constants to const table.
  const_table.AddConstant("0");
  const_table.AddConstant("1");
  const_table.AddConstant("2");
  const_table.AddConstant("3");
  const_table.AddConstant("5");

  // Setting tables in pkb
  pkb_stub->set_const_table(const_table);
  pkb_stub->set_proc_table(proc_table);
  pkb_stub->set_stmt_table(stmt_table);
  pkb_stub->set_var_table(var_table);

  return pkb_stub;
}
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

  // Parse source
  PKBStub *pkb_stub = BuildPKB();
  auto evaluator = new QueryEvaluator(pql_query, pkb_stub);
  std::vector<std::string> *ret = evaluator->Evaluate();

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
