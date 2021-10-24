#include "query_preprocessor.h"
#include "pql_query.h"
#include "catch.hpp"

TEST_CASE("Select_NoExtraClauses_ReturnsCorrect") {
  std::string ss = "stmt s;\n"
                   "Select s";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_UndefinedEntity_ReturnsNullPtr") {
  std::string ss = "stmt s;\n"
                   "Select s1";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Select_BadSelectKeyword_ReturnsNullPtr") {
  std::string ss = "stmt s;\n"
                   "select s";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Select_NoEntitySelected_ReturnsNullPtr") {
  std::string ss = "stmt s;\n"
                   "Select";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Select_ExtraSpaces_ReturnsCorrect") {
  std::string ss = "stmt s;\n"
                   "Select    s ";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("SuchThat_BadSuchThatClauseKeyword_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 st Follows(s1, s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("SuchThat_BadRelRef_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Following(s1, s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Follows_ArgAndStmt_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows('test', s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Follows_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows(s1, s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Follows);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Follows_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows(s1, 2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Follows);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Follows_StmtNumAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows(1, s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Follows);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_stmt_num() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Follows_StmtAndVar_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2; var v;\n"
                   "Select s1 such that Follows(s1, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Follows_StmtAndProcedure_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2; procedure p;\n"
                   "Select s1 such that Follows(s1, p)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("FollowsT_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows*(s1,s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::FollowsT);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("FollowsT_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows*(s1,2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::FollowsT);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("SuchThat_ExtraSpacesWithinBracket_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows*(s1  , s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::FollowsT);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Parent_ArgAndStmt_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent('test', s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Parent_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent(s1, s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Parent);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Parent_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent(s1, 2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Parent);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Parent_StmtNumAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent(1, s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Parent);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_stmt_num() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Parent_StmtNumAndVar_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2; var v;\n"
                   "Select s1 such that Parent(s1, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("ParentT_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent*(s1,s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ParentT);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("ParentT_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent*(s1,2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ParentT);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_AssignAndVar_ReturnsCorrect") {
  std::string ss = "assign a; variable v;\n"
                   "Select a such that Modifies(a, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ModifiesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_ReadndVar_ReturnsCorrect") {
  std::string ss = "read r; variable v;\n"
                   "Select r such that Modifies(r, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "r");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ModifiesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "r");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_IfAndVar_ReturnsCorrect") {
  std::string ss = "if ifs; variable v;\n"
                   "Select ifs such that Modifies(ifs, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ModifiesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_WhileAndVar_ReturnsCorrect") {
  std::string ss = "while whiles; variable v;\n"
                   "Select whiles such that Modifies(whiles, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "whiles");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ModifiesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "whiles");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_ProcedureAndVar_ReturnsCorrect") {
  std::string ss = "procedure proc; variable v;\n"
                   "Select proc such that Modifies(proc, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "proc");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ModifiesP);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_ent_ref().get_synonym() == "proc");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_CallAndVar_ReturnsCorrect") {
  std::string ss = "call calls; variable v;\n"
                   "Select calls such that Modifies(calls, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "calls");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ModifiesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "calls");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_StmtAndVar_ReturnsCorrect") {
  std::string ss = "stmt s; variable v;\n"
                   "Select s such that Modifies(s, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ModifiesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_StmtNumAndVar_ReturnsCorrect") {
  std::string ss = "variable v;\n"
                   "Select v such that Modifies(2, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "v");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::ModifiesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Modifies_AssignAndStmt_ReturnsFalse") {
  std::string ss = "assign assign; stmt s;\n"
                   "Select assign such that Modifies(assign, s)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Modifies_UnknownEntityAndVar_ReturnsFalse") {
  std::string ss = "assign assign; stmt s;\n"
                   "Select assign such that Modifies(pn, 50)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Uses_AssignAndVar_ReturnsCorrect") {
  std::string ss = "assign a; variable v;\n"
                   "Select a such that Uses(a, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::UsesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Uses_PrintAndVar_ReturnsCorrect") {
  std::string ss = "print pn; variable v;\n"
                   "Select pn such that Uses(pn, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "pn");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::UsesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "pn");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Uses_IfAndVar_ReturnsCorrect") {
  std::string ss = "if ifs; variable v;\n"
                   "Select ifs such that Uses(ifs, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::UsesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Uses_WhileAndVar_ReturnsCorrect") {
  std::string ss = "while whiles; variable v;\n"
                   "Select whiles such that Uses(whiles, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "whiles");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::UsesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "whiles");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Uses_ProcedureAndVar_ReturnsCorrect") {
  std::string ss = "procedure proc; variable v;\n"
                   "Select proc such that Uses(proc, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "proc");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::UsesP);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_ent_ref().get_synonym() == "proc");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Uses_CallAndVar_ReturnsCorrect") {
  std::string ss = "call calls; variable v;\n"
                   "Select calls such that Uses(calls, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "calls");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::UsesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "calls");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Uses_StmtAndVar_ReturnsCorrect") {
  std::string ss = "stmt s; variable v;\n"
                   "Select s such that Uses(s, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::UsesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Uses_StmtNumAndVar_ReturnsCorrect") {
  std::string ss = "variable v;\n"
                   "Select v such that Uses(2, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "v");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::UsesS);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Uses_AssignAndStmt_ReturnsCorrect") {
  std::string ss = "assign assign; stmt s;\n"
                   "Select assign such that Uses(assign, s)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Uses_UnknownEntityAndVar_ReturnsCorrect") {
  std::string ss = "assign assign; var v;\n"
                   "Select assign such that Uses(pn, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_AssignWildCardAndPattern_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a pattern a ( _ , 'count + 1')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 1);
  REQUIRE(std::get<2>(*clause)->at(0)->get_synonym()->get_synonym() == "a");
  REQUIRE(std::get<2>(*clause)->at(0)->get_variable()->get_type() == EntRefType::WildCard);
  REQUIRE(std::get<2>(*clause)->at(0)->get_exp_spec()->get_expression() == "count+1");
  REQUIRE(std::get<2>(*clause)->at(0)->get_exp_spec()->IsPartialPattern() == false);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_AssignWildCardAndPartialPattern_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a pattern a ( _ , _'count + 1'_)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 1);
  REQUIRE(std::get<2>(*clause)->at(0)->get_synonym()->get_synonym() == "a");
  REQUIRE(std::get<2>(*clause)->at(0)->get_variable()->get_type() == EntRefType::WildCard);
  REQUIRE(std::get<2>(*clause)->at(0)->get_exp_spec()->get_expression() == "count+1");
  REQUIRE(std::get<2>(*clause)->at(0)->get_exp_spec()->IsPartialPattern() == true);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_AssignWildCardAndWildCard_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a pattern a ( _ , _ )";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 1);
  REQUIRE(std::get<2>(*clause)->at(0)->get_synonym()->get_synonym() == "a");
  REQUIRE(std::get<2>(*clause)->at(0)->get_variable()->get_type() == EntRefType::WildCard);
  REQUIRE(std::get<2>(*clause)->at(0)->get_exp_spec()->IsWildCard() == true);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_AssignVarAndPartialPattern_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a pattern a ( 'normSq' , _'cenX * cenX'_)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_variable()->get_argument() == "normSq");
  REQUIRE(pattern->get_exp_spec()->get_expression() == "cenX*cenX");
  REQUIRE(pattern->get_exp_spec()->IsPartialPattern() == true);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_OneSuchThatAndPattern_ReturnsCorrect") {
  std::string ss = "assign a; while w;"
                   "Select w such that Parent* (w, a) pattern a ('count', _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "w");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::ParentT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "w");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "a");

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_variable()->get_argument() == "count");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);
  REQUIRE(pattern->get_exp_spec()->IsPartialPattern() == false);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_OnePatternAndSuchThat_ReturnsCorrect") {
  std::string ss = "assign a; variable v;"
                   "Select a such that Uses (a, v) pattern a (v, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);
  REQUIRE(pattern->get_exp_spec()->IsPartialPattern() == false);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_UsesRepeatEntity_ReturnsCorrect") {
  std::string ss = "assign a; while w;"
                   "Select a pattern a ('x', _) such that Uses (a, 'x')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_argument() == "x");

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_variable()->get_argument() == "x");
  REQUIRE(pattern->get_exp_spec()->IsPartialPattern() == false);

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_ComplexQueryWithSimilarPatternNaming_ReturnsCorrect") {
  std::string ss = "assign pattern; while w;"
                   "Select pattern pattern pattern ('x', _) "
                   "such that Uses (pattern, 'x')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "pattern");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "pattern");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_argument() == "x");

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "pattern");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_variable()->get_argument() == "x");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);
  REQUIRE(pattern->get_exp_spec()->IsPartialPattern() == false);

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_ComplexQueryWithSimilarNaming_ReturnsCorrect") {
  std::string ss = "assign pattern; variable select;"
                   "Select pattern pattern pattern (select, _) "
                   "such that Uses (pattern, select)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "pattern");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "pattern");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "select");

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "pattern");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "select");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);
  REQUIRE(pattern->get_exp_spec()->IsPartialPattern() == false);

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_ExtraSuchThatKeyword_ReturnsNullPtr") {
  std::string ss = "assign pattern; variable select;"
                   "Select pattern pattern pattern (select, _) "
                   "such that such that Uses (pattern, select)";
  auto query = QueryPreprocessor(ss);

  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Select_StrangeFormat_ReturnsNullPtr") {
  std::string ss = "assign pattern; variable select;"
                   "Select pattern pattern pattern (select, _) "
                   "such that (,)    such that Uses (pattern, select)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_EndingWithSymbol_ReturnsNullPtr") {
  std::string ss = "assign a;"
                   "Select a pattern a ( _ , 'x+y*')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_HavingTwoContinuousSymbols_ReturnsNullPtr") {
  std::string ss = "assign a;"
                   "Select a pattern a ( _ , 'x+*y')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_KeywordPatternInPattern_ReturnsCorrect") {
  std::string ss = "assign a;"
                   "Select a pattern a ( _ , ' pattern ')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::WildCard);
  REQUIRE(pattern->get_exp_spec()->get_expression() == "pattern");
  REQUIRE(pattern->get_exp_spec()->IsPartialPattern() == false);

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_InvalidKeywords_ReturnsNullPtr") {
  std::string ss = "stmt s;"
                   "Select s hello Follows(s, 4)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Select_FollowsPattern_ReturnsCorrect") {
  std::string ss = "assign a; variable v;\n"
                   "Select a such that Follows(a,_) pattern a(v, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_type() == StmtRefType::WildCard);

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("SuchThat_SpacesBetweenSuchThat_ReturnsNullPtr") {
  std::string ss = "stmt s; assign a; Select s such       that Follows (_,_)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Space between 2 entity declarations") {
  std::string ss = "while       while;      call call; Select call such that Follows(call, while)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "call");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "call");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "while");
  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("SuchThat_FollowsAndParent_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2; while w;\n"
                   "Select s1 such that Follows(s1,s2) and Parent(s2, w)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 2);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");

  SuchThatClause *relationship2 = std::get<1>(*clause)->at(1);
  REQUIRE(relationship2->get_type() == RelRef::Parent);
  REQUIRE(relationship2->get_left_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(relationship2->get_right_ref()->get_stmt_ref().get_synonym() == "w");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("SuchThat_FollowsAndParentAndUses_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2; while w;\n"
                   "Select s1 such that Follows(s1,s2) and Parent(s2, w) and Uses(w, 'x')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 3);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");

  SuchThatClause *relationship2 = std::get<1>(*clause)->at(1);
  REQUIRE(relationship2->get_type() == RelRef::Parent);
  REQUIRE(relationship2->get_left_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(relationship2->get_right_ref()->get_stmt_ref().get_synonym() == "w");

  SuchThatClause *relationship3 = std::get<1>(*clause)->at(2);
  REQUIRE(relationship3->get_type() == RelRef::UsesS);
  REQUIRE(relationship3->get_left_ref()->get_stmt_ref().get_synonym() == "w");
  REQUIRE(relationship3->get_right_ref()->get_ent_ref().get_argument() == "x");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("SuchThat_FollowsAndParentAndPattern_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2; while w; assign a; \n"
                   "Select s1 such that Follows(s1,s2) and Parent(s2, w) and pattern a(_, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_PatternAndPattern_ReturnsCorrect") {
  std::string ss = "assign a, a1; variable v;\n"
                   "Select v pattern a(v, _) and a1(v, 'x')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "v");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 2);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);

  PatternClause *pattern2 = std::get<2>(*clause)->at(1);
  REQUIRE(pattern2->get_synonym()->get_synonym() == "a1");
  REQUIRE(pattern2->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern2->get_variable()->get_synonym() == "v");
  REQUIRE(pattern2->get_exp_spec()->IsWildCard() == false);
  REQUIRE(pattern2->get_exp_spec()->IsPartialPattern() == false);
  REQUIRE(pattern2->get_exp_spec()->get_expression() == "x");

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_PatternAndPatternAndPattern_ReturnsCorrect") {
  std::string ss = "assign a, a1, a2; variable v, v2;\n"
                   "Select v pattern a(v, _) and a1(v, 'x') and a2(v2, _'x2'_)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "v");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 3);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);

  PatternClause *pattern2 = std::get<2>(*clause)->at(1);
  REQUIRE(pattern2->get_synonym()->get_synonym() == "a1");
  REQUIRE(pattern2->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern2->get_variable()->get_synonym() == "v");
  REQUIRE(pattern2->get_exp_spec()->IsWildCard() == false);
  REQUIRE(pattern2->get_exp_spec()->IsPartialPattern() == false);
  REQUIRE(pattern2->get_exp_spec()->get_expression() == "x");

  PatternClause *pattern3 = std::get<2>(*clause)->at(2);
  REQUIRE(pattern3->get_synonym()->get_synonym() == "a2");
  REQUIRE(pattern3->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern3->get_variable()->get_synonym() == "v2");
  REQUIRE(pattern3->get_exp_spec()->IsWildCard() == false);
  REQUIRE(pattern3->get_exp_spec()->IsPartialPattern() == true);
  REQUIRE(pattern3->get_exp_spec()->get_expression() == "x2");

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_PatternPatternPattern_ReturnsCorrect") {
  std::string ss = "assign a, a1, a2; variable v, v2;\n"
                   "Select v pattern a(v, _) pattern a1(v, 'x') pattern a2(v2, _'x2'_)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "v");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 3);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);

  PatternClause *pattern2 = std::get<2>(*clause)->at(1);
  REQUIRE(pattern2->get_synonym()->get_synonym() == "a1");
  REQUIRE(pattern2->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern2->get_variable()->get_synonym() == "v");
  REQUIRE(pattern2->get_exp_spec()->IsWildCard() == false);
  REQUIRE(pattern2->get_exp_spec()->IsPartialPattern() == false);
  REQUIRE(pattern2->get_exp_spec()->get_expression() == "x");

  PatternClause *pattern3 = std::get<2>(*clause)->at(2);
  REQUIRE(pattern3->get_synonym()->get_synonym() == "a2");
  REQUIRE(pattern3->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern3->get_variable()->get_synonym() == "v2");
  REQUIRE(pattern3->get_exp_spec()->IsWildCard() == false);
  REQUIRE(pattern3->get_exp_spec()->IsPartialPattern() == true);
  REQUIRE(pattern3->get_exp_spec()->get_expression() == "x2");

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Complex_MultiplePatternAndSuchThat_ReturnsCorrect") {
  std::string ss = "stmt s; assign a, a1; variable v;"
                   "Select <s, a1, a, v> pattern a (v, _) and a1 (v, _) "
                   "such that Modifies(a, v) and Parent*(_, s)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->size() == 4);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s");
  REQUIRE(std::get<0>(*clause)->at(1)->get_synonym() == "a1");
  REQUIRE(std::get<0>(*clause)->at(2)->get_synonym() == "a");
  REQUIRE(std::get<0>(*clause)->at(3)->get_synonym() == "v");

  REQUIRE(std::get<1>(*clause)->size() == 2);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");

  SuchThatClause *relationship2 = std::get<1>(*clause)->at(1);
  REQUIRE(relationship2->get_type() == RelRef::ParentT);
  REQUIRE(relationship2->get_left_ref()->get_stmt_ref().get_type() == StmtRefType::WildCard);
  REQUIRE(relationship2->get_right_ref()->get_stmt_ref().get_synonym() == "s");

  REQUIRE(std::get<2>(*clause)->size() == 2);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");
  REQUIRE(pattern->get_exp_spec()->IsWildCard() == true);

  PatternClause *pattern2 = std::get<2>(*clause)->at(1);
  REQUIRE(pattern2->get_synonym()->get_synonym() == "a1");
  REQUIRE(pattern2->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern2->get_variable()->get_synonym() == "v");
  REQUIRE(pattern2->get_exp_spec()->IsWildCard() == true);

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_WhileVariableWildCard_ReturnsCorrect") {
  std::string ss = "while w; variable v;\n"
                   "Select w pattern w (v , _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "w");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "w");
  REQUIRE(pattern->get_type() == EntityType::While);
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_WhileArgumentWildCard_ReturnsCorrect") {
  std::string ss = "while w;\n"
                   "Select w pattern w ('x', _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "w");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "w");
  REQUIRE(pattern->get_type() == EntityType::While);
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_variable()->get_argument() == "x");

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_WhileWildCardWildCard_ReturnsCorrect") {
  std::string ss = "while w;\n"
                   "Select w pattern w (_, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "w");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "w");
  REQUIRE(pattern->get_type() == EntityType::While);
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::WildCard);

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_WhileVariableAndNonWildCard_ReturnsWrong") {
  std::string ss = "assign w; variable v, v1;\n"
                   "Select w pattern w (v, v1)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_IfVariableWildCard_ReturnsCorrect") {
  std::string ss = "if ifs; variable v;\n"
                   "Select ifs pattern ifs (v , _, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "ifs");
  REQUIRE(pattern->get_type() == EntityType::If);
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_IfArgumentWildCard_ReturnsCorrect") {
  std::string ss = "if ifs;\n"
                   "Select ifs pattern ifs ('x', _, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "ifs");
  REQUIRE(pattern->get_type() == EntityType::If);
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_variable()->get_argument() == "x");

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_IfWildCardWildCard_ReturnsCorrect") {
  std::string ss = "if ifs;\n"
                   "Select ifs pattern ifs (_, _, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->size() == 0);

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "ifs");
  REQUIRE(pattern->get_type() == EntityType::If);
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::WildCard);

  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Pattern_IfVariableAndNonLeftWildCard_ReturnsWrong") {
  std::string ss = "if ifs; variable v, v1;\n"
                   "Select ifs pattern ifs (v, v1, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_IfVariableAndNonRightWildCard_ReturnsWrong") {
  std::string ss = "if ifs; variable v, v1;\n"
                   "Select ifs pattern ifs (v, _, v1)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_IfVariableAndNonLeftAndRightWildCard_ReturnsWrong") {
  std::string ss = "if ifs; variable v, v1; while w;\n"
                   "Select ifs pattern ifs (v, v1, w)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Pattern_IfVariableWithMissingArgument_ReturnsWrong") {
  std::string ss = "if ifs; variable v;\n"
                   "Select ifs pattern ifs (v,_)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_ProcedureAndProcedure_ReturnsCorrect") {
  std::string ss = "procedure p1, p2;\n"
                   "Select p1 with p1.procName = p2.procName";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "p1");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "p1");
  REQUIRE(with->get_left_type() == EntityType::Procedure);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Name);
  REQUIRE(with->get_right_ref() == "p2");
  REQUIRE(with->get_right_type() == EntityType::Procedure);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Name);
}

TEST_CASE("With_ProcedureAndCall_ReturnsCorrect") {
  std::string ss = "procedure p; call c;\n"
                   "Select p with p.procName = c.procName";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "p");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "p");
  REQUIRE(with->get_left_type() == EntityType::Procedure);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Name);
  REQUIRE(with->get_right_ref() == "c");
  REQUIRE(with->get_right_type() == EntityType::Call);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Name);
}

TEST_CASE("With_ProcedureAndVariable_ReturnsCorrect") {
  std::string ss = "procedure p; variable v;\n"
                   "Select v with p.procName = v.varName";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "v");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "p");
  REQUIRE(with->get_left_type() == EntityType::Procedure);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Name);
  REQUIRE(with->get_right_ref() == "v");
  REQUIRE(with->get_right_type() == EntityType::Variable);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Name);
}

TEST_CASE("With_CallAndRead_ReturnsCorrect") {
  std::string ss = "call c; read r;\n"
                   "Select c with c.procName = r.varName";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "c");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "c");
  REQUIRE(with->get_left_type() == EntityType::Call);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Name);
  REQUIRE(with->get_right_ref() == "r");
  REQUIRE(with->get_right_type() == EntityType::Read);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Name);
}

TEST_CASE("With_PrintAndPrint_ReturnsCorrect") {
  std::string ss = "print pn1, pn2;\n"
                   "Select pn1 with pn1.varName = pn2.varName";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "pn1");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "pn1");
  REQUIRE(with->get_left_type() == EntityType::Print);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Name);
  REQUIRE(with->get_right_ref() == "pn2");
  REQUIRE(with->get_right_type() == EntityType::Print);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Name);
}

TEST_CASE("With_ConstantAndConstant_ReturnsCorrect") {
  std::string ss = "constant c1, c2;\n"
                   "Select c1 with c1.value = c2.value";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "c1");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "c1");
  REQUIRE(with->get_left_type() == EntityType::Constant);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "c2");
  REQUIRE(with->get_right_type() == EntityType::Constant);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("With_ConstantAndStmt_ReturnsCorrect") {
  std::string ss = "constant c1; stmt s;\n"
                   "Select c1 with c1.value = s.stmt#";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "c1");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "c1");
  REQUIRE(with->get_left_type() == EntityType::Constant);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "s");
  REQUIRE(with->get_right_type() == EntityType::Stmt);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("With_StmtAndRead_ReturnsCorrect") {
  std::string ss = "stmt s; read r;\n"
                   "Select r with s.stmt# = r.stmt#";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "r");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "s");
  REQUIRE(with->get_left_type() == EntityType::Stmt);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "r");
  REQUIRE(with->get_right_type() == EntityType::Read);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("With_PrintAndCall_ReturnsCorrect") {
  std::string ss = "print pn; call c;\n"
                   "Select pn with pn.stmt# = c.stmt#";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "pn");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "pn");
  REQUIRE(with->get_left_type() == EntityType::Print);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "c");
  REQUIRE(with->get_right_type() == EntityType::Call);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("With_WhileAndAssign_ReturnsCorrect") {
  std::string ss = "while w; assign a;\n"
                   "Select a with w.stmt# = a.stmt#";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "w");
  REQUIRE(with->get_left_type() == EntityType::While);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "a");
  REQUIRE(with->get_right_type() == EntityType::Assign);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("With_ProcedureAndStmt_ReturnsNullPtr") {
  std::string ss = "procedure p; stmt s;\n"
                   "Select p with p.procName = s.stmt#";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_ProcedureAndProcedureWrongKeyword_ReturnsNullPtr") {
  std::string ss = "procedure p1, p2;\n"
                   "Select p1 with p1.procName = p2.varName";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_PrintAndPrintWithDiffType_ReturnsNullPtr") {
  std::string ss = "print pn1, pn2;\n"
                   "Select pn1 with pn1.varName = pn2.stmt#";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_DoubleEqualSign_ReturnsNullPtr") {
  std::string ss = "print pn1, pn2;\n"
                   "Select pn1 with pn1.varName == pn2.varName";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_StmtAndStmtWrongKeyword_ReturnsNullPtr") {
  std::string ss = "stmt s1, s2;\n"
                   "Select s1 with s1.stmt# = s2.stmtNum";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_IntegerAndProcedure_ReturnsNullPtr") {
  std::string ss = "procedure p;\n"
                   "Select p with 2 = p.procName";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_PrintAndInteger_ReturnsNullPtr") {
  std::string ss = "print pn;\n"
                   "Select pn with pn.varName = 4";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_IntegerAndConstant_ReturnsCorrect") {
  std::string ss = "constant c;\n"
                   "Select c with 2 = c.value";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "c");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "2");
  REQUIRE(with->get_left_type() == EntityType::None);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "c");
  REQUIRE(with->get_right_type() == EntityType::Constant);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("With_IfAndInteger_ReturnsCorrect") {
  std::string ss = "if ifs;\n"
                   "Select ifs with ifs.stmt# = 25461";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "ifs");
  REQUIRE(with->get_left_type() == EntityType::If);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "25461");
  REQUIRE(with->get_right_type() == EntityType::None);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("With_VariableAndIdent_ReturnsCorrect") {
  std::string ss = "variable v;\n"
                   "Select v with v.varName = 'x'";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "v");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "v");
  REQUIRE(with->get_left_type() == EntityType::Variable);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Name);
  REQUIRE(with->get_right_ref() == "x");
  REQUIRE(with->get_right_type() == EntityType::None);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Name);
}

TEST_CASE("With_IfAndIntegerExtraSpace_ReturnsCorrect") {
  std::string ss = "if ifs;\n"
                   "Select ifs with ifs.stmt#     =  25461";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "ifs");
  REQUIRE(with->get_left_type() == EntityType::If);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "25461");
  REQUIRE(with->get_right_type() == EntityType::None);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("With_IfAndIntegerSpaceBeforeAttrValueType_ReturnsNullPtr") {
  std::string ss = "if ifs;\n"
                   "Select ifs with ifs. stmt# = 25461";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_IfAndIntegerSpaceAfterAttr_ReturnsNullPtr") {
  std::string ss = "if ifs;\n"
                   "Select ifs with ifs .stmt# = 25461";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("With_TwoWiths_ReturnsCorrect") {
  std::string ss = "if ifs; procedure p;\n"
                   "Select ifs with ifs.stmt# = 25461 and p.procName = 'computeCentroid'";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "ifs");
  REQUIRE(std::get<1>(*clause)->size() == 0);
  REQUIRE(std::get<2>(*clause)->size() == 0);

  REQUIRE(std::get<3>(*clause)->size() == 2);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_ref() == "ifs");
  REQUIRE(with->get_left_type() == EntityType::If);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "25461");
  REQUIRE(with->get_right_type() == EntityType::None);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);

  WithClause *with2 = std::get<3>(*clause)->at(1);
  REQUIRE(with2->get_left_ref() == "p");
  REQUIRE(with2->get_left_type() == EntityType::Procedure);
  REQUIRE(with2->get_left_attr_value_type() == AttrValueType::Name);
  REQUIRE(with2->get_right_ref() == "computeCentroid");
  REQUIRE(with2->get_right_type() == EntityType::None);
  REQUIRE(with2->get_right_attr_value_type() == AttrValueType::Name);
}

TEST_CASE("Next_ArgAndStmt_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Next('test', s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Next_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Next(s1, s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Next);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_line_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_line_ref().get_synonym() == "s2");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Next_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Next(s1, 2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Next);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_line_ref().get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_line_ref().get_line_num() == 2);

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Next_StmtNumAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Next(1, s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_type() == RelRef::Next);
  REQUIRE(std::get<1>(*clause)->at(0)->get_left_ref()->get_line_ref().get_line_num() == 1);
  REQUIRE(std::get<1>(*clause)->at(0)->get_right_ref()->get_line_ref().get_synonym() == "s2");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Next_StmtAndVar_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2; var v;\n"
                   "Select s1 such that Next(s1, v)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Next_StmtAndProcedure_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2; procedure p;\n"
                   "Select s1 such that Next(s1, p)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("NextT_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Next*(s1,s2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::NextT);
  REQUIRE(relationship->get_left_ref()->get_line_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_line_ref().get_synonym() == "s2");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("NextT_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Next*(s1,2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::NextT);
  REQUIRE(relationship->get_left_ref()->get_line_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_line_ref().get_line_num() == 2);

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Affects_ArgAndAssign_ReturnsNullPtr") {
  std::string ss = "assign a;\n"
                   "Select a such that Affects('test', a)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Affects_AssignAndAssign_ReturnsCorrect") {
  std::string ss = "assign a1; assign a2;\n"
                   "Select a1 such that Affects(a1, a2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Affects);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "a2");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Affects_AssignAndStmtNum_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a such that Affects(a, 2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Affects);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Affects_StmtNumAndAssign_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a such that Affects(1, a)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Affects);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_stmt_num() == 1);
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "a");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Affects_AssignAndStmt_ReturnsCorrect") {
  std::string ss = "assign a; stmt s;\n"
                   "Select s such that Affects(a, s)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Affects);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Affects_AssignAndProcedure_ReturnsNullPtr") {
  std::string ss = "assign a; procedure p;\n"
                   "Select a such that Affects(a, p)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("AffectsT_AssigAndAssign_ReturnsCorrect") {
  std::string ss = "assign a1, a2;\n"
                   "Select a1 such that Affects*(a1,a2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::AffectsT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "a2");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("AffectsT_AssignAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt a1;\n"
                   "Select a1 such that Affects*(a1,2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "a1");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::AffectsT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Calls_ProcedureAndProcedure_ReturnsCorrect") {
  std::string ss = "procedure proc, proc2;\n"
                   "Select proc such that Calls(proc, proc2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "proc");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Calls);
  REQUIRE(relationship->get_left_ref()->get_ent_ref().get_synonym() == "proc");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "proc2");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Calls_ProcedureAndAssign_ReturnsNullPtr") {
  std::string ss = "procedure proc; assign a;\n"
                   "Select proc such that Calls(proc, a)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Calls_ProcedureAndArgument_ReturnsCorrect") {
  std::string ss = "procedure proc;\n"
                   "Select proc such that Calls(proc, 'computeCentroid')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "proc");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Calls);
  REQUIRE(relationship->get_left_ref()->get_ent_ref().get_synonym() == "proc");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_argument() == "computeCentroid");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("CallsT_ProcedureAndProcedure_ReturnsCorrect") {
  std::string ss = "procedure proc, proc2;\n"
                   "Select proc such that Calls*(proc, proc2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "proc");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::CallsT);
  REQUIRE(relationship->get_left_ref()->get_ent_ref().get_synonym() == "proc");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "proc2");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("CallsT_ProcedureAndAssign_ReturnsNullPtr") {
  std::string ss = "procedure proc; assign a;\n"
                   "Select proc such that Calls*(proc, a)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("CallsT_ProcedureAndArgument_ReturnsCorrect") {
  std::string ss = "procedure proc;\n"
                   "Select proc such that Calls*(proc, 'computeCentroid')";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "proc");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::CallsT);
  REQUIRE(relationship->get_left_ref()->get_ent_ref().get_synonym() == "proc");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_argument() == "computeCentroid");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("CallsT_ProcedureAndWhile_ReturnsNullPtr") {
  std::string ss = "procedure proc; while w;\n"
                   "Select proc such that Calls(proc, w)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("CallsT_IntegerAndProcedure_ReturnsNullPtr") {
  std::string ss = "procedure proc; while w;\n"
                   "Select proc such that Calls(5, proc)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Select_ProgLine_ReturnsCorrect") {
  std::string ss = "prog_line n;"
                   "Select n such that Next* (5, n) and Next* (n, 12)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "n");
  REQUIRE(std::get<1>(*clause)->size() == 2);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::NextT);
  REQUIRE(relationship->get_left_ref()->get_line_ref().get_line_num() == 5);
  REQUIRE(relationship->get_right_ref()->get_line_ref().get_synonym() == "n");

  SuchThatClause *relationship2 = std::get<1>(*clause)->at(1);
  REQUIRE(relationship2->get_type() == RelRef::NextT);
  REQUIRE(relationship2->get_left_ref()->get_line_ref().get_synonym() == "n");
  REQUIRE(relationship2->get_right_ref()->get_line_ref().get_line_num() == 12);

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}


TEST_CASE("Select_ComplexQueries_ReturnsCorrect") {
  std::string ss = "prog_line n; stmt s, s1;"
                   "Select s such that Follows* (s, s1) with s1.stmt#=n and n=10";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::FollowsT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s1");

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 2);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_type() == EntityType::Stmt);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "n");
  REQUIRE(with->get_right_type() == EntityType::ProgLine);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("Select_ValidAttr_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1.stmt# such that Follows(s1, 2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s1");
  REQUIRE(std::get<0>(*clause)->at(0)->get_return_type() == ReturnType::Integer);
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_InvalidAttr_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1.varName such that Follows(s1, 2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}

TEST_CASE("Select_BOOLEANTrue_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select BOOLEAN such that Follows(s1, 2)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_return_type() == ReturnType::Boolean);
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);

  REQUIRE(std::get<2>(*clause)->size() == 0);
  REQUIRE(std::get<3>(*clause)->size() == 0);
}

TEST_CASE("Select_QueriesWithSuchThatPatternWith_ReturnsCorrect") {
  std::string ss = "prog_line n; stmt s, s1; if ifs; variable v;"
                   "Select s such that Follows* (s, s1) with s1.stmt#=n pattern ifs(v, _, _)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == true);
  REQUIRE(std::get<0>(*clause)->at(0)->get_synonym() == "s");
  REQUIRE(std::get<1>(*clause)->size() == 1);
  SuchThatClause *relationship = std::get<1>(*clause)->at(0);
  REQUIRE(relationship->get_type() == RelRef::FollowsT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s1");

  REQUIRE(std::get<2>(*clause)->size() == 1);
  PatternClause *pattern = std::get<2>(*clause)->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "ifs");
  REQUIRE(pattern->get_type() == EntityType::If);
  REQUIRE(pattern->get_variable()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_variable()->get_synonym() == "v");

  REQUIRE(std::get<3>(*clause)->size() == 1);
  WithClause *with = std::get<3>(*clause)->at(0);
  REQUIRE(with->get_left_type() == EntityType::Stmt);
  REQUIRE(with->get_left_attr_value_type() == AttrValueType::Integer);
  REQUIRE(with->get_right_ref() == "n");
  REQUIRE(with->get_right_type() == EntityType::ProgLine);
  REQUIRE(with->get_right_attr_value_type() == AttrValueType::Integer);
}

TEST_CASE("Calls_ProcedureAndRead_ReturnsNullPtr") {
  std::string ss = "procedure p; read r;"
                   "Select p such that Calls(r, p)";
  auto query = QueryPreprocessor(ss);
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool> *clause = query.get_clauses();
  REQUIRE(std::get<5>(*clause) == false);
}
