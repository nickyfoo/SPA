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

TEST_CASE("PKB_FollowsSampleProgram_Correct") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> follows_ans;
  follows_ans[1] = { 2 };
  follows_ans[2] = { 3 };
  follows_ans[4] = { 5 };
  follows_ans[6] = { 7 };
  follows_ans[7] = { 8 };
  follows_ans[8] = { 9 };
  follows_ans[10] = { 11 };
  follows_ans[11] = { 12 };
  follows_ans[12] = { 13 };
  follows_ans[13] = { 14 };
  follows_ans[14] = { 19 };
  follows_ans[15] = { 16 };
  follows_ans[16] = { 17 };
  follows_ans[17] = { 18 };
  follows_ans[19] = { 23 };
  follows_ans[21] = { 22 };

  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers = stmt->get_followers();
    std::vector<int> followers = follows_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers->size() == followers.size());
    for (auto &follower : followers) {
      REQUIRE(stmt_followers->find(follower) != stmt_followers->end());
    }
  }

  std::map<int, std::vector<int>> follows_star_ans;
  follows_star_ans[1] = { 2,3 };
  follows_star_ans[2] = { 3 };
  follows_star_ans[4] = { 5 };
  follows_star_ans[6] = { 7,8,9 };
  follows_star_ans[7] = { 8,9 };
  follows_star_ans[8] = { 9 };
  follows_star_ans[10] = { 11,12,13,14,19,23 };
  follows_star_ans[11] = { 12,13,14,19,23 };
  follows_star_ans[12] = { 13,14,19,23 };
  follows_star_ans[13] = { 14,19,23 };
  follows_star_ans[14] = { 19,23 };
  follows_star_ans[15] = { 16,17,18 };
  follows_star_ans[16] = { 17,18 };
  follows_star_ans[17] = { 18 };
  follows_star_ans[19] = { 23 };
  follows_star_ans[21] = { 22 };

  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers_star = stmt->get_followers_star();
    std::vector<int> followers_star = follows_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers_star->size() == followers_star.size());
    for (auto &follower_star : followers_star) {
      REQUIRE(stmt_followers_star->find(follower_star) != stmt_followers_star->end());
    }
  }

  std::map<int, std::vector<int>> followees_ans;
  followees_ans[2] = { 1 };
  followees_ans[3] = { 2 };
  followees_ans[5] = { 4 };
  followees_ans[7] = { 6 };
  followees_ans[8] = { 7 };
  followees_ans[9] = { 8 };
  followees_ans[11] = { 10 };
  followees_ans[12] = { 11 };
  followees_ans[13] = { 12 };
  followees_ans[14] = { 13 };
  followees_ans[16] = { 15 };
  followees_ans[17] = { 16 };
  followees_ans[18] = { 17 };
  followees_ans[19] = { 14 };
  followees_ans[22] = { 21 };
  followees_ans[23] = { 19 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followees = stmt->get_followees();
    std::vector<int> followees = followees_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees->size() == followees.size());
    for (auto &followee : followees) {
      REQUIRE(stmt_followees->find(followee) != stmt_followees->end());
    }
  }

  std::map<int, std::vector<int>> followees_star_ans;
  followees_star_ans[2] = { 1 };
  followees_star_ans[3] = { 2,1 };
  followees_star_ans[5] = { 4 };
  followees_star_ans[7] = { 6 };
  followees_star_ans[8] = { 7,6 };
  followees_star_ans[9] = { 8,7,6 };
  followees_star_ans[11] = { 10 };
  followees_star_ans[12] = { 11,10 };
  followees_star_ans[13] = { 12,11,10 };
  followees_star_ans[14] = { 13,12,11,10 };
  followees_star_ans[19] = { 14,13,12,11,10 };
  followees_star_ans[16] = { 15 };
  followees_star_ans[17] = { 16,15 };
  followees_star_ans[18] = { 17,16,15 };
  followees_star_ans[23] = { 19,14,13,12,11,10 };
  followees_star_ans[22] = { 21 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followees_star = stmt->get_followees_star();
    std::vector<int> followees_star = followees_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees_star->size() == followees_star.size());
    for (auto &followee_star : followees_star) {
      REQUIRE(stmt_followees_star->find(followee_star) != stmt_followees_star->end());
    }
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
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> follows_ans;
  follows_ans[1] = { 2 };
  follows_ans[2] = { 3 };
  follows_ans[3] = { 4 };
  follows_ans[4] = { 10 };
  follows_ans[5] = { 6 };
  follows_ans[11] = { 12 };
  for (auto stmt: pkb.get_all_statements()) {
    std::set<int> *stmt_followers = stmt->get_followers();
    std::vector<int> followers = follows_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers->size() == followers.size());
    for (auto &follower : followers) {
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
    std::set<int> *stmt_followers_star = stmt->get_followers_star();
    std::vector<int> followers_star = follows_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers_star->size() == followers_star.size());
    for (auto &follower_star : followers_star) {
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
    std::set<int> *stmt_followees = stmt->get_followees();
    std::vector<int> followees = followees_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees->size() == followees.size());
    for (auto &followee : followees) {
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
    std::set<int> *stmt_followees_star = stmt->get_followees_star();
    std::vector<int> followees_star = followees_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees_star->size() == followees_star.size());
    for (auto &followee_star : followees_star) {
      REQUIRE(stmt_followees_star->find(followee_star) != stmt_followees_star->end());
    }
  }
}

TEST_CASE("PKB_ParentSampleProgram_Correct") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> children_ans;
  children_ans[14] = { 15,16,17,18 };
  children_ans[19] = { 20,21,22 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children = stmt->get_children();
    std::vector<int> children = children_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children->size() == children.size());
    for (auto &child : children) {
      REQUIRE(stmt_children->find(child) != stmt_children->end());
    }
  }

  std::map<int, std::vector<int>> children_star_ans;
  children_star_ans[14] = { 15,16,17,18 };
  children_star_ans[19] = { 20,21,22 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children_star = stmt->get_children_star();
    std::vector<int> children_star = children_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children_star->size() == children_star.size());
    for (auto &child_star : children_star) {
      REQUIRE(stmt_children_star->find(child_star) != stmt_children_star->end());
    }
  }

  std::map<int, std::vector<int>> parents_ans;
  parents_ans[15] = { 14 };
  parents_ans[16] = { 14 };
  parents_ans[17] = { 14 };
  parents_ans[18] = { 14 };
  parents_ans[20] = { 19 };
  parents_ans[21] = { 19 };
  parents_ans[22] = { 19 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents = stmt->get_parents();
    std::vector<int> parents = parents_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents->size() == parents.size());
    for (auto &parent : parents) {
      REQUIRE(stmt_parents->find(parent) != stmt_parents->end());
    }
  }
  std::map<int, std::vector<int>> parents_star_ans;
  parents_star_ans[15] = { 14 };
  parents_star_ans[16] = { 14 };
  parents_star_ans[17] = { 14 };
  parents_star_ans[18] = { 14 };
  parents_star_ans[20] = { 19 };
  parents_star_ans[21] = { 19 };
  parents_star_ans[22] = { 19 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents_star = stmt->get_parents_star();
    std::vector<int> parents_star = parents_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents_star->size() == parents_star.size());
    for (auto &parent_star : parents_star) {
      REQUIRE(stmt_parents_star->find(parent_star) != stmt_parents_star->end());
    }
  }
}

TEST_CASE("PKB_ParentNested_Correct") {
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
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  for (auto stmt : pkb.get_all_statements()) {
    stmt->ParentInfo();
  }

  std::map<int, std::vector<int>> children_ans;
  children_ans[4] = { 5,6 };
  children_ans[6] = { 7 };
  children_ans[7] = { 8,9 };
  children_ans[10] = { 11, 12 };
  children_ans[12] = { 13 };
  children_ans[13] = { 14,15 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children = stmt->get_children();
    std::vector<int> children = children_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children->size() == children.size());
    for (auto &child : children) {
      REQUIRE(stmt_children->find(child) != stmt_children->end());
    }
  }

  std::map<int, std::vector<int>> children_star_ans;
  children_star_ans[4] = { 5,6,7,8,9};
  children_star_ans[6] = { 7,8,9 };
  children_star_ans[7] = { 8,9 };
  children_star_ans[10] = { 11,12,13,14,15 };
  children_star_ans[12] = { 13,14,15 };
  children_star_ans[13] = { 14,15 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children_star = stmt->get_children_star();
    std::vector<int> children_star = children_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children_star->size() == children_star.size());
    for (auto &child_star : children_star) {
      REQUIRE(stmt_children_star->find(child_star) != stmt_children_star->end());
    }
  }

  std::map<int, std::vector<int>> parents_ans;
  parents_ans[5] = { 4 };
  parents_ans[6] = { 4 };
  parents_ans[7] = { 6 };
  parents_ans[8] = { 7 };
  parents_ans[9] = { 7 };
  parents_ans[11] = { 10 };
  parents_ans[12] = { 10 };
  parents_ans[13] = { 12 };
  parents_ans[14] = { 13 };
  parents_ans[15] = { 13 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents = stmt->get_parents();
    std::vector<int> parents = parents_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents->size() == parents.size());
    for (auto &parent : parents) {
      REQUIRE(stmt_parents->find(parent) != stmt_parents->end());
    }
  }

  std::map<int, std::vector<int>> parents_star_ans;
  parents_star_ans[5] = { 4 };
  parents_star_ans[6] = { 4 };
  parents_star_ans[7] = { 6,4 };
  parents_star_ans[8] = { 7,6,4 };
  parents_star_ans[9] = { 7,6,4 };
  parents_star_ans[11] = { 10 };
  parents_star_ans[12] = { 10 };
  parents_star_ans[13] = { 12,10 };
  parents_star_ans[14] = { 13,12,10 };
  parents_star_ans[15] = { 13,12,10 };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents_star = stmt->get_parents_star();
    std::vector<int> parents_star = parents_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents_star->size() == parents_star.size());
    for (auto &parent_star : parents_star) {
      REQUIRE(stmt_parents_star->find(parent_star) != stmt_parents_star->end());
    }
  }
}

TEST_CASE("PKB_CallsSampleProgram_Correct") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<std::string, std::vector<std::string>> calls_ans;
  calls_ans["main"] = { "computeCentroid", "printResults" };
  calls_ans["computeCentroid"] = { "readPoint" };

  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_calls = proc->get_calls();
    std::vector<std::string> calls = calls_ans[proc->get_name()];
    REQUIRE(proc_calls->size() == calls.size());
    for (auto &call : calls) {
      REQUIRE(proc_calls->find(call) != proc_calls->end());
    }
  }
  
  std::map<std::string, std::vector<std::string>> calls_star_ans;
  calls_star_ans["main"] = { "computeCentroid", "printResults", "readPoint"};
  calls_star_ans["computeCentroid"] = { "readPoint" };

  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *procs_calls_star= proc->get_calls_star();
    std::vector<std::string> calls_star = calls_star_ans[proc->get_name()];
    REQUIRE(procs_calls_star->size() == calls_star.size());
    for (auto &call_star : calls_star) {
      REQUIRE(procs_calls_star->find(call_star) != procs_calls_star->end());
    }
  }

  std::map<std::string, std::vector<std::string>> callers_ans;
  callers_ans["computeCentroid"] = { "main" };
  callers_ans["printResults"] = { "main" };
  callers_ans["readPoint"] = { "computeCentroid" };

  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *callers_procs = proc->get_callers();
    std::vector<std::string> callers = callers_ans[proc->get_name()];
    REQUIRE(callers_procs->size() == callers.size());
    for (auto &caller : callers) {
      REQUIRE(callers_procs->find(caller) != callers_procs->end());
    }
  }

  std::map<std::string, std::vector<std::string>> callers_star_ans;
  callers_star_ans["computeCentroid"] = { "main" };
  callers_star_ans["printResults"] = { "main" };
  callers_star_ans["readPoint"] = { "computeCentroid","main"};

  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *callers_star_procs = proc->get_callers_star();
    std::vector<std::string> callers_star = callers_star_ans[proc->get_name()];
    REQUIRE(callers_star_procs->size() == callers_star.size());
    for (auto &caller_star : callers_star) {
      REQUIRE(callers_star_procs->find(caller_star) != callers_star_procs->end());
    }
  }
}

TEST_CASE("PKB_UsesModifiesSampleProgram_Correct") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<std::string>> stmt_uses_ans;
  stmt_uses_ans[2] = { "x","y","count","cenX","cenY" };
  stmt_uses_ans[3] = { "flag","cenX","cenY","normSq"};
  stmt_uses_ans[6] = { "flag" };
  stmt_uses_ans[7] = { "cenX" };
  stmt_uses_ans[8] = { "cenY" };
  stmt_uses_ans[9] = { "normSq" };
  stmt_uses_ans[14] = { "x","y","count","cenX","cenY" };
  stmt_uses_ans[15] = { "count" };
  stmt_uses_ans[16] = { "cenX", "x" };
  stmt_uses_ans[17] = { "cenY", "y" };
  stmt_uses_ans[19] = { "count","cenX", "cenY" };
  stmt_uses_ans[21] = { "cenX","count" };
  stmt_uses_ans[22] = { "cenY","count" };
  stmt_uses_ans[23] = { "cenX","cenY" };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_uses = stmt->get_uses();
    std::vector<std::string> uses = stmt_uses_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(stmt_uses->find(use) != stmt_uses->end());
    }
  }

  std::map<int, std::vector<std::string>> stmt_modifies_ans;
  stmt_modifies_ans[1] = { "flag" };
  stmt_modifies_ans[2] = { "count","cenX","cenY","x","y","flag","normSq" };
  stmt_modifies_ans[4] = { "x" };
  stmt_modifies_ans[5] = { "y" };
  stmt_modifies_ans[10] = { "count" };
  stmt_modifies_ans[11] = { "cenX" };
  stmt_modifies_ans[12] = { "cenY" };
  stmt_modifies_ans[13] = { "x","y" };
  stmt_modifies_ans[14] = { "count","cenX","cenY","x","y" };
  stmt_modifies_ans[15] = { "count" };
  stmt_modifies_ans[16] = { "cenX" };
  stmt_modifies_ans[17] = { "cenY" };
  stmt_modifies_ans[18] = { "x","y" };
  stmt_modifies_ans[19] = { "flag","cenX", "cenY"};
  stmt_modifies_ans[20] = { "flag" };
  stmt_modifies_ans[21] = { "cenX" };
  stmt_modifies_ans[22] = { "cenY" };
  stmt_modifies_ans[23] = { "normSq" };

  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_modifies = stmt->get_modifies();
    std::vector<std::string> modifies = stmt_modifies_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(stmt_modifies->find(modify) != stmt_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_uses_ans;
  proc_uses_ans["main"] = { "x","y","count","cenX", "cenY","flag","normSq" };
  proc_uses_ans["printResults"] = { "flag","cenX","cenY","normSq" };
  proc_uses_ans["computeCentroid"] = { "x","y","count","cenX", "cenY" };
  for (auto proc: pkb.get_all_procedures()) {
    std::set<std::string> *proc_uses= proc->get_uses();
    std::vector<std::string> uses = proc_uses_ans[proc->get_name()];
    REQUIRE(proc_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(proc_uses->find(use) != proc_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_modifies_ans;
  proc_modifies_ans["main"] = { "count","cenX","cenY","x","y","flag","normSq" };
  proc_modifies_ans["readPoint"] = { "x","y" };
  proc_modifies_ans["computeCentroid"] = { "count","cenX","cenY","x","y","flag","normSq" };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_modifies = proc->get_modifies();
    std::vector<std::string> modifies = proc_modifies_ans[proc->get_name()];
    REQUIRE(proc_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(proc_modifies->find(modify) != proc_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_used_by_procs;
  var_used_by_procs["flag"] = { "printResults","main" };
  var_used_by_procs["cenX"] = { "printResults","computeCentroid","main" };
  var_used_by_procs["cenY"] = { "printResults","computeCentroid","main" };
  var_used_by_procs["normSq"] = { "printResults","main" };
  var_used_by_procs["x"] = { "computeCentroid","main" };
  var_used_by_procs["y"] = { "computeCentroid","main" };
  var_used_by_procs["count"] = { "computeCentroid","main" };

  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_using = var->get_procs_using();
    std::vector<std::string> procs = var_used_by_procs[var->get_name()];
    REQUIRE(procs_using->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_using->find(proc) != procs_using->end());
    }
  }

  std::map<std::string, std::vector<int>> var_used_by_stmts;
  var_used_by_stmts["flag"] = { 3,6 };
  var_used_by_stmts["cenX"] = { 2,3,7,14,16,19,21,23 };
  var_used_by_stmts["cenY"] = { 2,3,8,14,17,19,22,23 };
  var_used_by_stmts["normSq"] = { 3,9 };
  var_used_by_stmts["x"] = { 2,14,16 };
  var_used_by_stmts["y"] = { 2,14,17 };
  var_used_by_stmts["count"] = { 2,14,15,19,21,22 };

  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_using = var->get_stmts_using();
    std::vector<int> stmts = var_used_by_stmts[var->get_name()];
    REQUIRE(stmts_using->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_using->find(stmt) != stmts_using->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_modified_by_procs;
  var_modified_by_procs["flag"] = { "main","computeCentroid"};
  var_modified_by_procs["cenX"] = { "computeCentroid","main" };
  var_modified_by_procs["cenY"] = { "computeCentroid","main" };
  var_modified_by_procs["normSq"] = { "main","computeCentroid" };
  var_modified_by_procs["x"] = { "readPoint","computeCentroid","main" };
  var_modified_by_procs["y"] = { "readPoint","computeCentroid","main" };
  var_modified_by_procs["count"] = { "computeCentroid","main" };

  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_modifying = var->get_procs_modifying();
    std::vector<std::string> procs = var_modified_by_procs[var->get_name()];
    REQUIRE(procs_modifying->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_modifying->find(proc) != procs_modifying->end());
    }
  }

  std::map<std::string, std::vector<int>> var_modified_by_stmts;
  var_modified_by_stmts["flag"] = { 1,2,19,20 };
  var_modified_by_stmts["cenX"] = { 2,11,14,16,19,21};
  var_modified_by_stmts["cenY"] = { 2,12,14,17,19,22 };
  var_modified_by_stmts["normSq"] = { 2,23 };
  var_modified_by_stmts["x"] = { 2,4,13,14,18 };
  var_modified_by_stmts["y"] = { 2,5,13,14,18 };
  var_modified_by_stmts["count"] = { 2,10,14,15};

  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_modifying = var->get_stmts_modifying();
    std::vector<int> stmts = var_modified_by_stmts[var->get_name()];
    REQUIRE(stmts_modifying->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_modifying->find(stmt) != stmts_modifying->end());
    }
  }
}

TEST_CASE("PKB_UsesModifiesContainerStmt_Correct") {
  std::string source =
    "procedure main {"
    "if((x==0) && (y==0) ) then {"
    "a = b + c;"
    "} else {"
    "d = e + f;}"
    "}";

  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  pkb.PrintProcedures();
  pkb.PrintVariables();
  std::vector<Statement*> statements = pkb.get_all_statements();
  for (auto &stmt : statements) {
    stmt->UsesInfo();
    stmt->ModifiesInfo();
  }
  std::vector<Procedure*> procedures = pkb.get_all_procedures();
  for (auto &proc : procedures) {
    proc->UsesInfo();
    proc->ModifiesInfo();
  }
  std::vector<Variable*> variables = pkb.get_all_variables();
  for (auto &var : variables) {
    var->UsesInfo();
    var->ModifiesInfo();
  }

  std::map<int, std::vector<std::string>> stmt_uses_ans;
  stmt_uses_ans[1] = { "x","y","b","c","e","f" };
  stmt_uses_ans[2] = { "b", "c" };
  stmt_uses_ans[3] = { "e","f" };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_uses = stmt->get_uses();
    std::vector<std::string> uses = stmt_uses_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(stmt_uses->find(use) != stmt_uses->end());
    }
  }

  std::map<int, std::vector<std::string>> stmt_modifies_ans;
  stmt_modifies_ans[1] = { "a","d" };
  stmt_modifies_ans[2] = { "a" };
  stmt_modifies_ans[3] = { "d" };

  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_modifies = stmt->get_modifies();
    std::vector<std::string> modifies = stmt_modifies_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(stmt_modifies->find(modify) != stmt_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_uses_ans;
  proc_uses_ans["main"] = { "x","y","b","c","e","f" };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_uses = proc->get_uses();
    std::vector<std::string> uses = proc_uses_ans[proc->get_name()];
    REQUIRE(proc_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(proc_uses->find(use) != proc_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_modifies_ans;
  proc_modifies_ans["main"] = { "a","d" };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_modifies = proc->get_modifies();
    std::vector<std::string> modifies = proc_modifies_ans[proc->get_name()];
    REQUIRE(proc_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(proc_modifies->find(modify) != proc_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_used_by_procs;
  var_used_by_procs["b"] = { "main" };
  var_used_by_procs["c"] = { "main" };
  var_used_by_procs["e"] = { "main" };
  var_used_by_procs["f"] = { "main" };
  var_used_by_procs["x"] = { "main" };
  var_used_by_procs["y"] = { "main" };

  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_using = var->get_procs_using();
    std::vector<std::string> procs = var_used_by_procs[var->get_name()];
    REQUIRE(procs_using->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_using->find(proc) != procs_using->end());
    }
  }

  std::map<std::string, std::vector<int>> var_used_by_stmts;
  var_used_by_stmts["x"] = { 1 };
  var_used_by_stmts["y"] = { 1 };
  var_used_by_stmts["b"] = { 1,2 };
  var_used_by_stmts["c"] = { 1,2 };
  var_used_by_stmts["e"] = { 1,3 };
  var_used_by_stmts["f"] = { 1,3 };

  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_using = var->get_stmts_using();
    std::vector<int> stmts = var_used_by_stmts[var->get_name()];
    REQUIRE(stmts_using->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_using->find(stmt) != stmts_using->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_modified_by_procs;
  var_modified_by_procs["a"] = { "main" };
  var_modified_by_procs["d"] = { "main" };

  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_modifying = var->get_procs_modifying();
    std::vector<std::string> procs = var_modified_by_procs[var->get_name()];
    REQUIRE(procs_modifying->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_modifying->find(proc) != procs_modifying->end());
    }
  }

  std::map<std::string, std::vector<int>> var_modified_by_stmts;
  var_modified_by_stmts["a"] = { 1,2 };
  var_modified_by_stmts["d"] = { 1,3 };

  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_modifying = var->get_stmts_modifying();
    std::vector<int> stmts = var_modified_by_stmts[var->get_name()];
    REQUIRE(stmts_modifying->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_modifying->find(stmt) != stmts_modifying->end());
    }
  }
}
