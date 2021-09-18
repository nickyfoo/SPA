#include "parse.h"
#include <vector>
#include "pkb.h"
#include "catch.hpp"
#include "entities/statement.h"

std::string source = "procedure main {\n"
                     "\tflag = 0;\n"
                     "\tcall computeCentroid;\n"
                     "\tcall printResults;\n"
                     "}\n"
                     "procedure readPoint {\n"
                     "\tread x;\n"
                     "\tread y;\n"
                     "}\n"
                     "procedure printResults {\n"
                     "\tprint flag;\n"
                     "\tprint cenX;\n"
                     "\tprint cenY;\n"
                     "\tprint normSq;\n"
                     "}\n"
                     "procedure computeCentroid {\n"
                     "\tcount = 0;\n"
                     "\tcenX = 0;\n"
                     "\tcenY = 0;\n"
                     "\tcall readPoint;\n"
                     "\twhile((x != 0) && (y != 0)) {\n"
                     "\t\tcount = count+1;\n"
                     "\t\tcenX = cenX + x;\n"
                     "\t\tcenY = cenY + y;\n"
                     "\t\tcall readPoint;\n"
                     "\t}\n"
                     "\tif (count == 0) then {\n"
                     "\t\tflag = 1;\n"
                     "\t} else {\n"
                     "\t\tcenX = cenX / count;\n"
                     "\t\tcenY = cenY / count;\n"
                     "\t}\n"
                     "\tnormSq = cenX * cenX + cenY * cenY;\n"
                     "}";

TEST_CASE("Test PKB::Initialise()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
}

TEST_CASE("Test pkb::GetFollows()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  for (int i = 1; i <= pkb.get_num_statements() + 1; i++) {
    Statement *stmt = pkb.get_statement(i);
    // checking for NULL response
    if (stmt) stmt->FollowsInfo();
  }
}

TEST_CASE("Test pkb::GetParent()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  for (int i = 1; i <= pkb.get_num_statements() + 1; i++) {
    Statement *stmt = pkb.get_statement(i);
    // checking for NULL response
    if (stmt) stmt->ParentInfo();
  }
}

TEST_CASE("Test pkb::ExtractCalls()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintProcedures();
  std::vector<Procedure *> procedures = pkb.get_all_procedures();
  for (auto&proc:procedures){
    proc->CallsInfo();
  }
}

TEST_CASE("Test pkb::ExtractUsesModifies()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  pkb.PrintProcedures();
  pkb.PrintVariables();
  std::vector<Statement *> statements = pkb.get_all_statements();
  for (auto &stmt : statements) {
    stmt->UsesInfo();
    stmt->ModifiesInfo();
  }
  std::vector<Procedure *> procedures = pkb.get_all_procedures();
  for (auto &proc : procedures) {
    proc->UsesInfo();
    proc->ModifiesInfo();
  }
  std::vector<Variable *> variables = pkb.get_all_variables();
  for (auto &var : variables) {
    var->UsesInfo();
    var->ModifiesInfo();
  }
}

TEST_CASE("Test pkb::ExtractUsesModifies() for containerstmts") {
  std::string source2 =
      "procedure main {"
      "if((x==0) && (y==0) ) then {"
      "a = b + c;"
      "} else {"
      "d = e + f;}"
      "}";

  BufferedLexer lexer(source2.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  pkb.PrintProcedures();
  pkb.PrintVariables();
  std::vector<Statement *> statements = pkb.get_all_statements();
  for (auto &stmt : statements) {
    stmt->UsesInfo();
    stmt->ModifiesInfo();
  }
  std::vector<Procedure *> procedures = pkb.get_all_procedures();
  for (auto &proc : procedures) {
    proc->UsesInfo();
    proc->ModifiesInfo();
  }
  std::vector<Variable *> variables = pkb.get_all_variables();
  for (auto &var : variables) {
    var->UsesInfo();
    var->ModifiesInfo();
  }
}


TEST_CASE("PKB_FollowsNested_Correct") {
  std::string source =
    "procedure nestedproc{"
    "  i = 0;"
    "  j = 0;"
    "  k = 0;"
    "  while(j < n){"
    "    j = j + 1;"
    "    while(k < n){"
    "      if(i <= k) then {"
    "        d = d + 1;"
    "      } else {"
    "        e = e + 1;"
    "      }"
    "    }"
    "  }"
    "  while(j < n){"
    "    j = i + k;"
    "    if (j < n) then {"
    "      if(i <= k) then {"
    "        d = d + 1;"
    "      } else {"
    "        e = e + 1;"
    "      }"
    "    } else {"
    "    }"
    "  }"
    "}";

  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode* p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  pkb.PrintProcedures();
  pkb.PrintVariables();
  for (auto stmt : pkb.get_all_statements()) {
    stmt->FollowsInfo();
  }

  std::map<int, std::vector<int>> follows_ans;
  follows_ans[1] = { 2 };
  follows_ans[2] = { 3 };
  follows_ans[3] = { 4 };
  follows_ans[4] = { 10 };
  follows_ans[5] = { 6 };
  follows_ans[11] = { 12 };
  for (auto stmt: pkb.get_all_statements()) {
    std::set<int>* stmt_followers = stmt->get_followers();
    std::vector<int> followers = follows_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers->size() == followers.size());
    for (auto& follower : followers) {
      REQUIRE(stmt_followers->find(follower) != stmt_followers->end());
    }
  }

  std::map<int, std::vector<int>> follows_star_ans;
  follows_star_ans[1] = { 2,3,4,10 };
  follows_star_ans[2] = { 3,4,10 };
  follows_star_ans[3] = { 4,10 };
  follows_star_ans[4] = { 10 };
  follows_star_ans[5] = { 6 };
  follows_star_ans[11] = { 12 };

  for (auto stmt : pkb.get_all_statements()) {
    std::set<int>* stmt_followers_star = stmt->get_followers_star();
    std::vector<int> followers_star = follows_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers_star->size() == followers_star.size());
    for (auto& follower_star : followers_star) {
      REQUIRE(stmt_followers_star->find(follower_star) != stmt_followers_star->end());
    }
  }

  std::map<int, std::vector<int>> followees_ans;
  followees_ans[2] = { 1 };
  followees_ans[3] = { 2 };
  followees_ans[4] = { 3 };
  followees_ans[10] = { 4 };
  followees_ans[6] = { 5 };
  followees_ans[12] = { 11 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int>* stmt_followees = stmt->get_followees();
    std::vector<int> followees = followees_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees->size() == followees.size());
    for (auto& followee : followees) {
      REQUIRE(stmt_followees->find(followee) != stmt_followees->end());
    }
  }

  std::map<int, std::vector<int>> followees_star_ans;
  followees_star_ans[2] = { 1 };
  followees_star_ans[3] = { 2, 1 };
  followees_star_ans[4] = { 3, 2, 1 };
  followees_star_ans[10] = { 4, 3, 2, 1 };
  followees_star_ans[6] = { 5 };
  followees_star_ans[12] = { 11 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int>* stmt_followees_star = stmt->get_followees_star();
    std::vector<int> followees_star = followees_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees_star->size() == followees_star.size());
    for (auto& followee_star : followees_star) {
      REQUIRE(stmt_followees_star->find(followee_star) != stmt_followees_star->end());
    }
  }


}