#include "query_preprocessor.h"
#include "pql_query.h"
#include "catch.hpp"

TEST_CASE("Select_NoExtraClauses_ReturnsCorrect") {
  std::string ss = "stmt s;\n"
                   "Select s";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Select_UndefinedEntity_ReturnsNullPtr") {
  std::string ss = "stmt s;\n"
                   "Select s1";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Select_BadSelectKeyword_ReturnsNullPtr") {
  std::string ss = "stmt s;\n"
                   "select s";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Select_NoEntitySelected_ReturnsNullPtr") {
  std::string ss = "stmt s;\n"
                   "Select";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Select_ExtraSpaces_ReturnsCorrect") {
  std::string ss = "stmt s;\n"
                   "Select    s ";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("SuchThat_BadSuchThatClauseKeyword_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 st Follows(s1, s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("SuchThat_BadRelRef_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Following(s1, s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Follows_ArgAndStmt_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows('test', s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Follows_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows(s1, s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  REQUIRE(clause->get_query_relationships()->at(0)->get_type() == RelRef::Follows);
  REQUIRE(clause->get_query_relationships()->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(clause->get_query_relationships()->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Follows_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows(s1, 2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  REQUIRE(clause->get_query_relationships()->at(0)->get_type() == RelRef::Follows);
  REQUIRE(clause->get_query_relationships()->at(0)->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(clause->get_query_relationships()->at(0)->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Follows_StmtNumAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows(1, s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  REQUIRE(clause->get_query_relationships()->at(0)->get_type() == RelRef::Follows);
  REQUIRE(clause->get_query_relationships()->at(0)->get_left_ref()->get_stmt_ref().get_stmt_num() == 1);
  REQUIRE(clause->get_query_relationships()->at(0)->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Follows_StmtAndVar_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2; var v;\n"
                   "Select s1 such that Follows(s1, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Follows_StmtAndProcedure_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2; procedure p;\n"
                   "Select s1 such that Follows(s1, p)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("FollowsT_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows*(s1,s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::FollowsT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("FollowsT_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows*(s1,2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::FollowsT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("SuchThat_ExtraSpacesWithinBracket_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Follows*(s1  , s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::FollowsT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Parent_ArgAndStmt_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent('test', s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Parent_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent(s1, s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::Parent);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Parent_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent(s1, 2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::Parent);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Parent_StmtNumAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent(1, s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::Parent);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_stmt_num() == 1);
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Parent_StmtNumAndVar_ReturnsNullPtr") {
  std::string ss = "stmt s1; stmt s2; var v;\n"
                   "Select s1 such that Parent(s1, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("ParentT_StmtAndStmt_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent*(s1,s2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ParentT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("ParentT_StmtAndStmtNum_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2;\n"
                   "Select s1 such that Parent*(s1,2)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ParentT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_AssignAndVar_ReturnsCorrect") {
  std::string ss = "assign a; variable v;\n"
                   "Select a such that Modifies(a, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_ReadndVar_ReturnsCorrect") {
  std::string ss = "read r; variable v;\n"
                   "Select r such that Modifies(r, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "r");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "r");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_IfAndVar_ReturnsCorrect") {
  std::string ss = "if ifs; variable v;\n"
                   "Select ifs such that Modifies(ifs, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "ifs");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "ifs");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_WhileAndVar_ReturnsCorrect") {
  std::string ss = "while whiles; variable v;\n"
                   "Select whiles such that Modifies(whiles, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "whiles");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "whiles");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_ProcedureAndVar_ReturnsCorrect") {
  std::string ss = "procedure proc; variable v;\n"
                   "Select proc such that Modifies(proc, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "proc");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesP);
  REQUIRE(relationship->get_left_ref()->get_ent_ref().get_synonym() == "proc");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_CallAndVar_ReturnsCorrect") {
  std::string ss = "call calls; variable v;\n"
                   "Select calls such that Modifies(calls, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "calls");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "calls");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_StmtAndVar_ReturnsCorrect") {
  std::string ss = "stmt s; variable v;\n"
                   "Select s such that Modifies(s, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_StmtNumAndVar_ReturnsCorrect") {
  std::string ss = "variable v;\n"
                   "Select v such that Modifies(2, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "v");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Modifies_AssignAndStmt_ReturnsCorrect") {
  std::string ss = "assign assign; stmt s;\n"
                   "Select assign such that Modifies(assign, s)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Modifies_UnknownEntityAndVar_ReturnsCorrect") {
  std::string ss = "assign assign; stmt s;\n"
                   "Select assign such that Modifies(pn, 50)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Uses_AssignAndVar_ReturnsCorrect") {
  std::string ss = "assign a; variable v;\n"
                   "Select a such that Uses(a, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Uses_PrintAndVar_ReturnsCorrect") {
  std::string ss = "print pn; variable v;\n"
                   "Select pn such that Uses(pn, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "pn");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "pn");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Uses_IfAndVar_ReturnsCorrect") {
  std::string ss = "if ifs; variable v;\n"
                   "Select ifs such that Uses(ifs, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "ifs");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "ifs");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Uses_WhileAndVar_ReturnsCorrect") {
  std::string ss = "while whiles; variable v;\n"
                   "Select whiles such that Uses(whiles, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "whiles");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "whiles");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Uses_ProcedureAndVar_ReturnsCorrect") {
  std::string ss = "procedure proc; variable v;\n"
                   "Select proc such that Uses(proc, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "proc");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesP);
  REQUIRE(relationship->get_left_ref()->get_ent_ref().get_synonym() == "proc");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Uses_CallAndVar_ReturnsCorrect") {
  std::string ss = "call calls; variable v;\n"
                   "Select calls such that Uses(calls, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "calls");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "calls");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Uses_StmtAndVar_ReturnsCorrect") {
  std::string ss = "stmt s; variable v;\n"
                   "Select s such that Uses(s, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Uses_StmtNumAndVar_ReturnsCorrect") {
  std::string ss = "variable v;\n"
                   "Select v such that Uses(2, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "v");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_stmt_num() == 2);
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");
  REQUIRE(clause->get_query_patterns()->size() == 0);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Uses_AssignAndStmt_ReturnsCorrect") {
  std::string ss = "assign assign; stmt s;\n"
                   "Select assign such that Uses(assign, s)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Uses_UnknownEntityAndVar_ReturnsCorrect") {
  std::string ss = "assign assign; var v;\n"
                   "Select assign such that Uses(pn, v)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Pattern_AssignWildCardAndPattern_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a pattern a ( _ , 'count + 1')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::WildCard);
  REQUIRE(pattern->get_right_ref()->get_expression() == "count+1");
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == false);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Pattern_AssignWildCardAndPartialPattern_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a pattern a ( _ , _'count + 1'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::WildCard);
  REQUIRE(pattern->get_right_ref()->get_expression() == "count+1");
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == true);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Pattern_AssignWildCardAndWildCard_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a pattern a ( _ , _ )";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::WildCard);
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Pattern_AssignVarAndPartialPattern_ReturnsCorrect") {
  std::string ss = "assign a;\n"
                   "Select a pattern a ( 'normSq' , _'cenX * cenX'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_left_ref()->get_argument() == "normSq");
  REQUIRE(pattern->get_right_ref()->get_expression() == "cenX*cenX");
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == true);
  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Select_OneSuchThatAndPattern_ReturnsCorrect") {
  std::string ss = "assign a; while w;"
                   "Select w such that Parent* (w, a) pattern a ('count', _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "w");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ParentT);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "w");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "a");

  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_left_ref()->get_argument() == "count");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == false);

  REQUIRE(clause->has_one_repeated_synonym() == true);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Select_OnePatternAndSuchThat_ReturnsCorrect") {
  std::string ss = "assign a; variable v;"
                   "Select a such that Uses (a, v) pattern a (v, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");

  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == false);

  REQUIRE(clause->has_one_repeated_synonym() == true);
  REQUIRE(clause->has_two_repeated_synonyms() == true);
}

TEST_CASE("Select_UsesRepeatEntity_ReturnsCorrect") {
  std::string ss = "assign a; while w;"
                   "Select a pattern a ('x', _) such that Uses (a, 'x')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_argument() == "x");

  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_left_ref()->get_argument() == "x");
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == false);

  REQUIRE(clause->has_one_repeated_synonym() == true);
  REQUIRE(clause->has_two_repeated_synonyms() == true);
}

TEST_CASE("Select_ComplexQueryWithSimilarPatternNaming_ReturnsCorrect") {
  std::string ss = "assign pattern; while w;"
                   "Select pattern pattern pattern ('x', _) "
                   "such that Uses (pattern, 'x')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "pattern");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "pattern");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_argument() == "x");

  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "pattern");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Argument);
  REQUIRE(pattern->get_left_ref()->get_argument() == "x");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == false);

  REQUIRE(clause->has_one_repeated_synonym() == true);
  REQUIRE(clause->has_two_repeated_synonyms() == true);
}

TEST_CASE("Select_ComplexQueryWithSimilarNaming_ReturnsCorrect") {
  std::string ss = "assign pattern; variable select;"
                   "Select pattern pattern pattern (select, _) "
                   "such that Uses (pattern, select)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "pattern");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::UsesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "pattern");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "select");

  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "pattern");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_left_ref()->get_synonym() == "select");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == false);

  REQUIRE(clause->has_one_repeated_synonym() == true);
  REQUIRE(clause->has_two_repeated_synonyms() == true);
}

TEST_CASE("Select_ExtraSuchThatKeyword_ReturnsNullPtr") {
  std::string ss = "assign pattern; variable select;"
                   "Select pattern pattern pattern (select, _) "
                   "such that such that Uses (pattern, select)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Select_StrangeFormat_ReturnsNullPtr") {
  std::string ss = "assign pattern; variable select;"
                   "Select pattern pattern pattern (select, _) "
                   "such that (,)    such that Uses (pattern, select)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Pattern_EndingWithSymbol_ReturnsNullPtr") {
  std::string ss = "assign a;"
                   "Select a pattern a ( _ , 'x+y*')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Pattern_HavingTwoContinuousSymbols_ReturnsNullPtr") {
  std::string ss = "assign a;"
                   "Select a pattern a ( _ , 'x+*y')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}


TEST_CASE("Pattern_KeywordPatternInPattern_ReturnsCorrect") {
  std::string ss = "assign a;"
                   "Select a pattern a ( _ , ' pattern ')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 0);

  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::WildCard);
  REQUIRE(pattern->get_right_ref()->get_expression() == "pattern");
  REQUIRE(pattern->get_right_ref()->IsPartialPattern() == false);

  REQUIRE(clause->has_one_repeated_synonym() == false);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("Select_InvalidKeywords_ReturnsNullPtr") {
  std::string ss = "stmt s;"
                   "Select s hello Follows(s, 4)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Select_FollowsPattern_ReturnsCorrect") {
  std::string ss = "assign a; variable v;\n"
                   "Select a such that Follows(a,_) pattern a(v, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "a");
  REQUIRE(clause->get_query_relationships()->size() == 1);
  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_type() == StmtRefType::WildCard);

  REQUIRE(clause->get_query_patterns()->size() == 1);
  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);

  REQUIRE(clause->has_one_repeated_synonym() == true);
  REQUIRE(clause->has_two_repeated_synonyms() == false);
}

TEST_CASE("SuchThat_SpacesBetweenSuchThat_ReturnsNullPtr") {
  std::string ss = "stmt s; assign a; Select s such       that Follows (_,_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Space between 2 entity declarations") {
  std::string ss = "while       while;      call call; Select call such that Follows(call, while)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
}

TEST_CASE("SuchThat_FollowsAndParent_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2; while w;\n"
                   "Select s1 such that Follows(s1,s2) and Parent(s2, w)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 2);

  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");

  SuchThatClause *relationship2 = clause->get_query_relationships()->at(1);
  REQUIRE(relationship2->get_type() == RelRef::Parent);
  REQUIRE(relationship2->get_left_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(relationship2->get_right_ref()->get_stmt_ref().get_synonym() == "w");

  REQUIRE(clause->get_query_patterns()->size() == 0);

}

TEST_CASE("SuchThat_FollowsAndParentAndUses_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2; while w;\n"
                   "Select s1 such that Follows(s1,s2) and Parent(s2, w) and Uses(w, 'x')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 3);

  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");

  SuchThatClause *relationship2 = clause->get_query_relationships()->at(1);
  REQUIRE(relationship2->get_type() == RelRef::Parent);
  REQUIRE(relationship2->get_left_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(relationship2->get_right_ref()->get_stmt_ref().get_synonym() == "w");

  SuchThatClause *relationship3 = clause->get_query_relationships()->at(2);
  REQUIRE(relationship3->get_type() == RelRef::UsesS);
  REQUIRE(relationship3->get_left_ref()->get_stmt_ref().get_synonym() == "w");
  REQUIRE(relationship3->get_right_ref()->get_ent_ref().get_argument() == "x");

  REQUIRE(clause->get_query_patterns()->size() == 0);

}

TEST_CASE("SuchThat_FollowsSTParentSTUses_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2; while w;\n"
                   "Select s1 such that Follows(s1,s2) such that Parent(s2, w) such that Uses(w, 'x')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "s1");
  REQUIRE(clause->get_query_relationships()->size() == 3);

  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::Follows);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "s1");
  REQUIRE(relationship->get_right_ref()->get_stmt_ref().get_synonym() == "s2");

  SuchThatClause *relationship2 = clause->get_query_relationships()->at(1);
  REQUIRE(relationship2->get_type() == RelRef::Parent);
  REQUIRE(relationship2->get_left_ref()->get_stmt_ref().get_synonym() == "s2");
  REQUIRE(relationship2->get_right_ref()->get_stmt_ref().get_synonym() == "w");

  SuchThatClause *relationship3 = clause->get_query_relationships()->at(2);
  REQUIRE(relationship3->get_type() == RelRef::UsesS);
  REQUIRE(relationship3->get_left_ref()->get_stmt_ref().get_synonym() == "w");
  REQUIRE(relationship3->get_right_ref()->get_ent_ref().get_argument() == "x");

  REQUIRE(clause->get_query_patterns()->size() == 0);

}

TEST_CASE("SuchThat_FollowsAndParentAndPattern_ReturnsCorrect") {
  std::string ss = "stmt s1; stmt s2; while w; assign a; \n"
                   "Select s1 such that Follows(s1,s2) and Parent(s2, w) and pattern a(_, _)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause == nullptr);
}

TEST_CASE("Pattern_PatternAndPattern_ReturnsCorrect") {
  std::string ss = "assign a, a1; variable v;\n"
                   "Select v pattern a(v, _) and a1(v, 'x')";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "v");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 2);

  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);

  PatternClause *pattern2 = clause->get_query_patterns()->at(1);
  REQUIRE(pattern2->get_synonym()->get_synonym() == "a1");
  REQUIRE(pattern2->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern2->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern2->get_right_ref()->IsWildCard() == false);
  REQUIRE(pattern2->get_right_ref()->IsPartialPattern() == false);
  REQUIRE(pattern2->get_right_ref()->get_expression() == "x");
}

TEST_CASE("Pattern_PatternAndPatternAndPattern_ReturnsCorrect") {
  std::string ss = "assign a, a1, a2; variable v, v2;\n"
                   "Select v pattern a(v, _) and a1(v, 'x') and a2(v2, _'x2'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "v");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 3);

  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);

  PatternClause *pattern2 = clause->get_query_patterns()->at(1);
  REQUIRE(pattern2->get_synonym()->get_synonym() == "a1");
  REQUIRE(pattern2->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern2->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern2->get_right_ref()->IsWildCard() == false);
  REQUIRE(pattern2->get_right_ref()->IsPartialPattern() == false);
  REQUIRE(pattern2->get_right_ref()->get_expression() == "x");

  PatternClause *pattern3 = clause->get_query_patterns()->at(2);
  REQUIRE(pattern3->get_synonym()->get_synonym() == "a2");
  REQUIRE(pattern3->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern3->get_left_ref()->get_synonym() == "v2");
  REQUIRE(pattern3->get_right_ref()->IsWildCard() == false);
  REQUIRE(pattern3->get_right_ref()->IsPartialPattern() == true);
  REQUIRE(pattern3->get_right_ref()->get_expression() == "x2");
}

TEST_CASE("Pattern_PatternPatternPattern_ReturnsCorrect") {
  std::string ss = "assign a, a1, a2; variable v, v2;\n"
                   "Select v pattern a(v, _) pattern a1(v, 'x') pattern a2(v2, _'x2'_)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->at(0) == "v");
  REQUIRE(clause->get_query_relationships()->size() == 0);
  REQUIRE(clause->get_query_patterns()->size() == 3);

  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);

  PatternClause *pattern2 = clause->get_query_patterns()->at(1);
  REQUIRE(pattern2->get_synonym()->get_synonym() == "a1");
  REQUIRE(pattern2->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern2->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern2->get_right_ref()->IsWildCard() == false);
  REQUIRE(pattern2->get_right_ref()->IsPartialPattern() == false);
  REQUIRE(pattern2->get_right_ref()->get_expression() == "x");

  PatternClause *pattern3 = clause->get_query_patterns()->at(2);
  REQUIRE(pattern3->get_synonym()->get_synonym() == "a2");
  REQUIRE(pattern3->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern3->get_left_ref()->get_synonym() == "v2");
  REQUIRE(pattern3->get_right_ref()->IsWildCard() == false);
  REQUIRE(pattern3->get_right_ref()->IsPartialPattern() == true);
  REQUIRE(pattern3->get_right_ref()->get_expression() == "x2");
}

TEST_CASE("Complex_MultiplePatternAndSuchThat_ReturnsCorrect") {
  std::string ss = "stmt s; assign a, a1; variable v;"
                   "Select <s, a1, a, v> pattern a (v, _) and a1 (v, _) "
                   "such that Modifies(a, v) and Parent*(_, s)";
  auto *query = new QueryPreprocessor(ss);
  PQLQuery *clause = query->get_pql_query();
  REQUIRE(clause != nullptr);
  REQUIRE(clause->get_query_entities()->size() == 4);
  REQUIRE(clause->get_query_entities()->at(0) == "s");
  REQUIRE(clause->get_query_entities()->at(1) == "a1");
  REQUIRE(clause->get_query_entities()->at(2) == "a");
  REQUIRE(clause->get_query_entities()->at(3) == "v");

  REQUIRE(clause->get_query_relationships()->size() == 2);

  SuchThatClause *relationship = clause->get_query_relationships()->at(0);
  REQUIRE(relationship->get_type() == RelRef::ModifiesS);
  REQUIRE(relationship->get_left_ref()->get_stmt_ref().get_synonym() == "a");
  REQUIRE(relationship->get_right_ref()->get_ent_ref().get_synonym() == "v");

  SuchThatClause *relationship2 = clause->get_query_relationships()->at(1);
  REQUIRE(relationship2->get_type() == RelRef::ParentT);
  REQUIRE(relationship2->get_left_ref()->get_stmt_ref().get_type() == StmtRefType::WildCard);
  REQUIRE(relationship2->get_right_ref()->get_stmt_ref().get_synonym() == "s");

  REQUIRE(clause->get_query_patterns()->size() == 2);

  PatternClause *pattern = clause->get_query_patterns()->at(0);
  REQUIRE(pattern->get_synonym()->get_synonym() == "a");
  REQUIRE(pattern->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern->get_right_ref()->IsWildCard() == true);

  PatternClause *pattern2 = clause->get_query_patterns()->at(1);
  REQUIRE(pattern2->get_synonym()->get_synonym() == "a1");
  REQUIRE(pattern2->get_left_ref()->get_type() == EntRefType::Synonym);
  REQUIRE(pattern2->get_left_ref()->get_synonym() == "v");
  REQUIRE(pattern2->get_right_ref()->IsWildCard() == true);
}
