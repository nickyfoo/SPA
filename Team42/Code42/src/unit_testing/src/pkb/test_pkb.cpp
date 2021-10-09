#include <vector>

#include "catch.hpp"
#include "entities/statement.h"
#include "parse.h"
#include "pkb.h"
#include <chrono>

TEST_CASE("PKBExtractEntities_SampleProgram_Correct") {
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  SECTION("Test procedures") {
    std::set<std::string> correct_procedures =
        {"main", "readPoint", "printResults", "computeCentroid"};
    std::vector<Procedure *> proc_vec = pkb.get_all_procedures();

    REQUIRE(pkb.get_num_procedures() == correct_procedures.size());
    for (auto proc: proc_vec) {
      REQUIRE(correct_procedures.count(proc->get_name()) == 1);
    }
  }

  SECTION("Test statements") {
    int correct_num_statements = 23;
    REQUIRE(pkb.get_num_statements() == correct_num_statements);
  }

  SECTION("Test procedures") {
    std::set<std::string> correct_variables =
        {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
    std::vector<Variable *> var_vec = pkb.get_all_variables();

    REQUIRE(var_vec.size() == correct_variables.size());
    for (auto var: var_vec) {
      REQUIRE(correct_variables.count(var->get_name()) == 1);
    }
  }

  SECTION("Test constants") {
    std::set<std::string> correct_constants = {"0", "1"};
    std::vector<Constant *> const_vec = pkb.get_all_constants();

    REQUIRE(const_vec.size() == correct_constants.size());
    for (auto constant: const_vec) {
      REQUIRE(correct_constants.count(constant->get_value()) == 1);
    }
  }
}

TEST_CASE("PKB_FollowsSampleProgram_Correct") {
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> follows_ans = {
      {1, {2}}, {2, {3}}, {3, {}}, {4, {5}}, {5, {}}, {6, {7}}, {7, {8}}, {8, {9}}, {9, {}},
      {10, {11}}, {11, {12}}, {12, {13}}, {13, {14}}, {14, {19}}, {15, {16}}, {16, {17}}, {17, {18}},
      {18, {}}, {19, {23}}, {20, {}}, {21, {22}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers = stmt->get_followers();
    std::vector<int> followers = follows_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers->size() == followers.size());
    for (auto &follower : followers) {
      REQUIRE(stmt_followers->find(follower) != stmt_followers->end());
    }
  }

  std::map<int, std::vector<int>> followees_ans = {
      {1, {}}, {2, {1}}, {3, {2}}, {4, {}}, {5, {4}}, {6, {}}, {7, {6}}, {8, {7}}, {9, {8}},
      {10, {}}, {11, {10}}, {12, {11}}, {13, {12}}, {14, {13}}, {15, {}}, {16, {15}}, {17, {16}},
      {18, {17}}, {19, {14}}, {20, {}}, {21, {}}, {22, {21}}, {23, {19}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followees = stmt->get_followees();
    std::vector<int> followees = followees_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees->size() == followees.size());
    for (auto &followee : followees) {
      REQUIRE(stmt_followees->find(followee) != stmt_followees->end());
    }
  }
}

TEST_CASE("PKB_FollowsStarSampleProgram_Correct") {
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> follows_star_ans = {
      {1, {2, 3}}, {2, {3}}, {3, {}}, {4, {5}}, {5, {}}, {6, {7, 8, 9}}, {7, {8, 9}}, {8, {9}},
      {9, {}}, {10, {11, 12, 13, 14, 19, 23}}, {11, {12, 13, 14, 19, 23}}, {12, {13, 14, 19, 23}},
      {13, {14, 19, 23}}, {14, {19, 23}}, {15, {16, 17, 18}}, {16, {17, 18}}, {17, {18}},
      {18, {}}, {19, {23}}, {20, {}}, {21, {22}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers_star = stmt->get_followers_star();
    std::vector<int> followers_star = follows_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers_star->size() == followers_star.size());
    for (auto &follower_star : followers_star) {
      REQUIRE(stmt_followers_star->find(follower_star) != stmt_followers_star->end());
    }
  }

  std::map<int, std::vector<int>> followees_star_ans = {
      {1, {}}, {2, {1}}, {3, {1, 2}}, {4, {}}, {5, {4}}, {6, {}}, {7, {6}}, {8, {6, 7}},
      {9, {6, 7, 8}}, {10, {}}, {11, {10}}, {12, {10, 11}}, {13, {10, 11, 12}},
      {14, {10, 11, 12, 13}}, {15, {}}, {16, {15}}, {17, {15, 16}}, {18, {15, 16, 17}},
      {19, {10, 11, 12, 13, 14}}, {20, {}}, {21, {}}, {22, {21}},
      {23, {10, 11, 12, 13, 14, 19}}
  };
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
  std::string nested_source =
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
      "      f = f + 1;"
      "    }"
      "  }"
      "}";

  BufferedLexer lexer(nested_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> follows_ans = {
      {1, {2}}, {2, {3}}, {3, {4}}, {4, {10}}, {5, {6}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {12}}, {12, {}}, {13, {}}, {14, {}}, {15, {}}, {16, {}}, {17, {}},
      {18, {}}, {19, {}}, {20, {}}, {21, {}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers = stmt->get_followers();
    std::vector<int> followers = follows_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers->size() == followers.size());
    for (auto &follower : followers) {
      REQUIRE(stmt_followers->find(follower) != stmt_followers->end());
    }
  }

  std::map<int, std::vector<int>> followees_ans = {
      {1, {}}, {2, {1}}, {3, {2}}, {4, {3}}, {5, {}}, {6, {5}}, {7, {}}, {8, {}}, {9, {}},
      {10, {4}}, {11, {}}, {12, {11}}, {13, {}}, {14, {}}, {15, {}}, {16, {}}, {17, {}},
      {18, {}}, {19, {}}, {20, {}}, {21, {}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followees = stmt->get_followees();
    std::vector<int> followees = followees_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees->size() == followees.size());
    for (auto &followee : followees) {
      REQUIRE(stmt_followees->find(followee) != stmt_followees->end());
    }
  }
}

TEST_CASE("PKB_FollowsStarNested_Correct") {
  std::string nested_source =
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
      "      f = f + 1;"
      "    }"
      "  }"
      "}";

  BufferedLexer lexer(nested_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> follows_star_ans = {
      {1, {2, 3, 4, 10}}, {2, {3, 4, 10}}, {3, {4, 10}}, {4, {10}}, {5, {6}}, {6, {}}, {7, {}},
      {8, {}}, {9, {}}, {10, {}}, {11, {12}}, {12, {}}, {13, {}}, {14, {}}, {15, {}}, {16, {}}
  };

  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers_star = stmt->get_followers_star();
    std::vector<int> followers_star = follows_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers_star->size() == followers_star.size());
    for (auto &follower_star : followers_star) {
      REQUIRE(stmt_followers_star->find(follower_star) != stmt_followers_star->end());
    }
  }

  std::map<int, std::vector<int>> followees_star_ans = {
      {1, {}}, {2, {1}}, {3, {1, 2}}, {4, {1, 2, 3}}, {5, {}}, {6, {5}}, {7, {}}, {8, {}}, {9, {}},
      {10, {1, 2, 3, 4}}, {11, {}}, {12, {11}}, {13, {}}, {14, {}}, {15, {}}, {16, {}}
  };
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
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> children_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {}}, {12, {}}, {13, {}}, {14, {15, 16, 17, 18}}, {15, {}}, {16, {}},
      {17, {}}, {18, {}}, {19, {20, 21, 22}}, {20, {}}, {21, {}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children = stmt->get_children();
    std::vector<int> children = children_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children->size() == children.size());
    for (auto &child : children) {
      REQUIRE(stmt_children->find(child) != stmt_children->end());
    }
  }

  std::map<int, std::vector<int>> parents_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {}}, {12, {}}, {13, {}}, {14, {}}, {15, {14}}, {16, {14}}, {17, {14}},
      {18, {14}}, {19, {}}, {20, {19}}, {21, {19}}, {22, {19}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents = stmt->get_parents();
    std::vector<int> parents = parents_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents->size() == parents.size());
    for (auto &parent : parents) {
      REQUIRE(stmt_parents->find(parent) != stmt_parents->end());
    }
  }
}

TEST_CASE("PKB_ParentStarSampleProgram_Correct") {
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> children_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {}}, {12, {}}, {13, {}}, {14, {15, 16, 17, 18}}, {15, {}}, {16, {}}, {17, {}},
      {18, {}}, {19, {20, 21, 22}}, {20, {}}, {21, {}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children_star = stmt->get_children_star();
    std::vector<int> children_star = children_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children_star->size() == children_star.size());
    for (auto &child_star : children_star) {
      REQUIRE(stmt_children_star->find(child_star) != stmt_children_star->end());
    }
  }

  std::map<int, std::vector<int>> parents_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {}}, {12, {}}, {13, {}}, {14, {}}, {15, {14}}, {16, {14}}, {17, {14}},
      {18, {14}}, {19, {}}, {20, {19}}, {21, {19}}, {22, {19}}, {23, {}}
  };
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
      "      f = f + 1;"
      "    }"
      "  }"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> children_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {5, 6}}, {5, {}}, {6, {7}}, {7, {8, 9}}, {8, {}}, {9, {}},
      {10, {11, 12}}, {11, {}}, {12, {13, 16}}, {13, {14, 15}}, {14, {}}, {15, {}}, {16, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children = stmt->get_children();
    std::vector<int> children = children_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children->size() == children.size());
    for (auto &child : children) {
      REQUIRE(stmt_children->find(child) != stmt_children->end());
    }
  }

  std::map<int, std::vector<int>> parents_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {4}}, {6, {4}}, {7, {6}}, {8, {7}}, {9, {7}},
      {10, {}}, {11, {10}}, {12, {10}}, {13, {12}}, {14, {13}}, {15, {13}}, {16, {12}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents = stmt->get_parents();
    std::vector<int> parents = parents_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents->size() == parents.size());
    for (auto &parent : parents) {
      REQUIRE(stmt_parents->find(parent) != stmt_parents->end());
    }
  }
}

TEST_CASE("PKB_ParentStarNested_Correct") {
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
      "      f = f + 1;"
      "    }"
      "  }"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<int>> children_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {5, 6, 7, 8, 9}}, {5, {}}, {6, {7, 8, 9}}, {7, {8, 9}},
      {8, {}}, {9, {}}, {10, {11, 12, 13, 14, 15, 16}}, {11, {}}, {12, {13, 14, 15, 16}},
      {13, {14, 15}}, {14, {}}, {15, {}}, {16, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children_star = stmt->get_children_star();
    std::vector<int> children_star = children_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children_star->size() == children_star.size());
    for (auto &child_star : children_star) {
      REQUIRE(stmt_children_star->find(child_star) != stmt_children_star->end());
    }
  }

  std::map<int, std::vector<int>> parents_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {4}}, {6, {4}}, {7, {4, 6}}, {8, {4, 6, 7}},
      {9, {4, 6, 7}}, {10, {}}, {11, {10}}, {12, {10}}, {13, {10, 12}}, {14, {10, 12, 13}},
      {15, {10, 12, 13}}, {16, {10, 12}}
  };
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
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<std::string, std::vector<std::string>> calls_ans = {
      {"main", {"computeCentroid", "printResults"}},
      {"readPoint", {}},
      {"printResults", {}},
      {"computeCentroid", {"readPoint"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_calls = proc->get_calls();
    std::vector<std::string> calls = calls_ans[proc->get_name()];
    REQUIRE(proc_calls->size() == calls.size());
    for (auto &call : calls) {
      REQUIRE(proc_calls->find(call) != proc_calls->end());
    }
  }

  std::map<std::string, std::vector<std::string>> callers_ans = {
      {"main", {}},
      {"readPoint", {"computeCentroid"}},
      {"printResults", {"main"}},
      {"computeCentroid", {"main"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *callers_procs = proc->get_callers();
    std::vector<std::string> callers = callers_ans[proc->get_name()];
    REQUIRE(callers_procs->size() == callers.size());
    for (auto &caller : callers) {
      REQUIRE(callers_procs->find(caller) != callers_procs->end());
    }
  }
}

TEST_CASE("PKB_CallsStarSampleProgram_Correct") {
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<std::string, std::vector<std::string>> calls_star_ans = {
      {"main", {"computeCentroid", "printResults", "readPoint"}},
      {"readPoint", {}},
      {"printResults", {}},
      {"computeCentroid", {"readPoint"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *procs_calls_star = proc->get_calls_star();
    std::vector<std::string> calls_star = calls_star_ans[proc->get_name()];
    REQUIRE(procs_calls_star->size() == calls_star.size());
    for (auto &call_star : calls_star) {
      REQUIRE(procs_calls_star->find(call_star) != procs_calls_star->end());
    }
  }

  std::map<std::string, std::vector<std::string>> callers_star_ans = {
      {"main", {}},
      {"readPoint", {"computeCentroid", "main"}},
      {"printResults", {"main"}},
      {"computeCentroid", {"main"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *callers_star_procs = proc->get_callers_star();
    std::vector<std::string> callers_star = callers_star_ans[proc->get_name()];
    REQUIRE(callers_star_procs->size() == callers_star.size());
    for (auto &caller_star : callers_star) {
      REQUIRE(callers_star_procs->find(caller_star) != callers_star_procs->end());
    }
  }
}

TEST_CASE("PKB_UsesSampleProgram_Correct") {
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<std::string>> stmt_uses_ans = {
      {1, {}}, {2, {"x", "y", "count", "cenX", "cenY"}}, {3, {"flag", "cenX", "cenY", "normSq"}},
      {4, {}}, {5, {}}, {6, {"flag"}}, {7, {"cenX"}}, {8, {"cenY"}}, {9, {"normSq"}}, {10, {}},
      {11, {}}, {12, {}}, {13, {}}, {14, {"x", "y", "count", "cenX", "cenY"}}, {15, {"count"}},
      {16, {"cenX", "x"}}, {17, {"cenY", "y"}}, {18, {}}, {19, {"count", "cenX", "cenY"}},
      {20, {}}, {21, {"cenX", "count"}}, {22, {"cenY", "count"}}, {23, {"cenX", "cenY"}},
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_uses = stmt->get_uses();
    std::vector<std::string> uses = stmt_uses_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(stmt_uses->find(use) != stmt_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_uses_ans = {
      {"main", {"x", "y", "count", "cenX", "cenY", "flag", "normSq"}},
      {"readPoint", {}},
      {"printResults", {"flag", "cenX", "cenY", "normSq"}},
      {"computeCentroid", {"x", "y", "count", "cenX", "cenY"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_uses = proc->get_uses();
    std::vector<std::string> uses = proc_uses_ans[proc->get_name()];
    REQUIRE(proc_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(proc_uses->find(use) != proc_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_used_by_procs = {
      {"flag", {"printResults", "main"}},
      {"cenX", {"printResults", "computeCentroid", "main"}},
      {"cenY", {"printResults", "computeCentroid", "main"}},
      {"normSq", {"printResults", "main"}},
      {"x", {"computeCentroid", "main"}},
      {"y", {"computeCentroid", "main"}},
      {"count", {"computeCentroid", "main"}},
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_using = var->get_procs_using();
    std::vector<std::string> procs = var_used_by_procs[var->get_name()];
    REQUIRE(procs_using->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_using->find(proc) != procs_using->end());
    }
  }

  std::map<std::string, std::vector<int>> var_used_by_stmts = {
      {"flag", {3, 6}},
      {"cenX", {2, 3, 7, 14, 16, 19, 21, 23}},
      {"cenY", {2, 3, 8, 14, 17, 19, 22, 23}},
      {"normSq", {3, 9}},
      {"x", {2, 14, 16}},
      {"y", {2, 14, 17}},
      {"count", {2, 14, 15, 19, 21, 22}},
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_using = var->get_stmts_using();
    std::vector<int> stmts = var_used_by_stmts[var->get_name()];
    REQUIRE(stmts_using->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_using->find(stmt) != stmts_using->end());
    }
  }
}

TEST_CASE("PKB_ModifiesSampleProgram_Correct") {
  std::string main_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  BufferedLexer lexer(main_source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<std::string>> stmt_modifies_ans = {
      {1, {{"flag"}}}, {2, {"count", "cenX", "cenY", "x", "y", "flag", "normSq"}}, {3, {}},
      {4, {"x"}}, {5, {"y"}}, {6, {}}, {7, {}}, {8, {}}, {9, {}}, {10, {"count"}}, {11, {"cenX"}},
      {12, {"cenY"}}, {13, {"x", "y"}}, {14, {"count", "cenX", "cenY", "x", "y"}}, {15, {"count"}},
      {16, {"cenX"}}, {17, {"cenY"}}, {18, {"x", "y"}}, {19, {"flag", "cenX", "cenY"}},
      {20, {"flag"}}, {21, {"cenX"}}, {22, {"cenY"}}, {23, {"normSq"}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_modifies = stmt->get_modifies();
    std::vector<std::string> modifies = stmt_modifies_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(stmt_modifies->find(modify) != stmt_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_modifies_ans = {
      {"main", {"count", "cenX", "cenY", "x", "y", "flag", "normSq"}},
      {"readPoint", {"x", "y"}},
      {"printResults", {}},
      {"computeCentroid", {"count", "cenX", "cenY", "x", "y", "flag", "normSq"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_modifies = proc->get_modifies();
    std::vector<std::string> modifies = proc_modifies_ans[proc->get_name()];
    REQUIRE(proc_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(proc_modifies->find(modify) != proc_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_modified_by_procs = {
      {"flag", {"main", "computeCentroid"}},
      {"cenX", {"computeCentroid", "main"}},
      {"cenY", {"computeCentroid", "main"}},
      {"normSq", {"main", "computeCentroid"}},
      {"x", {"readPoint", "computeCentroid", "main"}},
      {"y", {"readPoint", "computeCentroid", "main"}},
      {"count", {"computeCentroid", "main"}},
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_modifying = var->get_procs_modifying();
    std::vector<std::string> procs = var_modified_by_procs[var->get_name()];
    REQUIRE(procs_modifying->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_modifying->find(proc) != procs_modifying->end());
    }
  }

  std::map<std::string, std::vector<int>> var_modified_by_stmts = {
      {"flag", {1, 2, 19, 20}},
      {"cenX", {2, 11, 14, 16, 19, 21}},
      {"cenY", {2, 12, 14, 17, 19, 22}},
      {"normSq", {2, 23}},
      {"x", {2, 4, 13, 14, 18}},
      {"y", {2, 5, 13, 14, 18}},
      {"count", {2, 10, 14, 15}}
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_modifying = var->get_stmts_modifying();
    std::vector<int> stmts = var_modified_by_stmts[var->get_name()];
    REQUIRE(stmts_modifying->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_modifying->find(stmt) != stmts_modifying->end());
    }
  }
}

TEST_CASE("PKB_UsesContainerStmt_Correct") {
  std::string source =
      "procedure main {"
      "  if((x==0) && (y==0) ) then {"
      "    a = b + c;"
      "  } else {"
      "    d = e + f;}"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<std::string>> stmt_uses_ans = {
      {1, {"x", "y", "b", "c", "e", "f"}},
      {2, {"b", "c"}},
      {3, {"e", "f"}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_uses = stmt->get_uses();
    std::vector<std::string> uses = stmt_uses_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(stmt_uses->find(use) != stmt_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_uses_ans = {
      {"main", {"x", "y", "b", "c", "e", "f"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_uses = proc->get_uses();
    std::vector<std::string> uses = proc_uses_ans[proc->get_name()];
    REQUIRE(proc_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(proc_uses->find(use) != proc_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_used_by_procs = {
      {"a", {}}, {"b", {"main"}}, {"c", {"main"}}, {"d", {}}, {"e", {"main"}},
      {"f", {"main"}}, {"x", {"main"}}, {"y", {"main"}}
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_using = var->get_procs_using();
    std::vector<std::string> procs = var_used_by_procs[var->get_name()];
    REQUIRE(procs_using->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_using->find(proc) != procs_using->end());
    }
  }

  std::map<std::string, std::vector<int>> var_used_by_stmts = {
      {"a", {}}, {"b", {1, 2}}, {"c", {1, 2}}, {"d", {}}, {"e", {1, 3}},
      {"f", {1, 3}}, {"x", {1}}, {"y", {1}},
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_using = var->get_stmts_using();
    std::vector<int> stmts = var_used_by_stmts[var->get_name()];
    REQUIRE(stmts_using->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_using->find(stmt) != stmts_using->end());
    }
  }
}

TEST_CASE("PKB_ModifiesContainerStmt_Correct") {
  std::string source =
      "procedure main {"
      "  if((x==0) && (y==0) ) then {"
      "    a = b + c;"
      "  } else {"
      "    d = e + f;}"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);

  std::map<int, std::vector<std::string>> stmt_modifies_ans = {
      {1, {"a", "d"}},
      {2, {"a"}},
      {3, {"d"}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_modifies = stmt->get_modifies();
    std::vector<std::string> modifies = stmt_modifies_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(stmt_modifies->find(modify) != stmt_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_modifies_ans = {
      {"main", {"a", "d"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_modifies = proc->get_modifies();
    std::vector<std::string> modifies = proc_modifies_ans[proc->get_name()];
    REQUIRE(proc_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(proc_modifies->find(modify) != proc_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_modified_by_procs = {
      {"a", {"main"}}, {"b", {}}, {"c", {}}, {"d", {"main"}}, {"e", {}},
      {"f", {}}, {"x", {}}, {"y", {}},
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_modifying = var->get_procs_modifying();
    std::vector<std::string> procs = var_modified_by_procs[var->get_name()];
    REQUIRE(procs_modifying->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_modifying->find(proc) != procs_modifying->end());
    }
  }

  std::map<std::string, std::vector<int>> var_modified_by_stmts = {
      {"a", {1, 2}}, {"b", {}}, {"c", {}}, {"d", {1, 3}}, {"e", {}},
      {"f", {}}, {"x", {}}, {"y", {}},
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_modifying = var->get_stmts_modifying();
    std::vector<int> stmts = var_modified_by_stmts[var->get_name()];
    REQUIRE(stmts_modifying->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_modifying->find(stmt) != stmts_modifying->end());
    }
  }
}

TEST_CASE("PKB_UndefinedProcCalled_ThrowsException") {
  std::string source =
      "procedure main {"
      "  call undefinedproc;"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb;
  REQUIRE_THROWS_AS(pkb = PKB(p), PKBException);
}

TEST_CASE("PKB_CyclicProcCalls_ThrowsException") {
  std::string source =
      "procedure proc1 {"
      "  call proc2;"
      "}"
      "procedure proc2 {"
      "  call proc3;"
      "}"
      "procedure proc3 {"
      "  call proc1;"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb;
  REQUIRE_THROWS_AS(pkb = PKB(p), PKBException);
}

TEST_CASE("PKB_CFGSample_Correct") {
  std::string source =
      "procedure First {"
      "  read x;"
      "  read y;"
      "  call Second; }"
      "procedure Second {"
      "  x = 0;"
      "  i = 5;"
      "  while (i != 0) {"
      "    x = x + 2 * y;"
      "    call Third;"
      "    i = i - 1; "
      "  }"
      "  if (x == 1) then {"
      "    x = x + 1; }"
      "  else {"
      "    z = 1;"
      "  }"
      "  z = z + x + i;"
      "  y = z + 2;"
      "  x = x * y + z; }"
      "procedure Third {"
      "  z = 5;"
      "  v = z;"
      "  print v; }";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> ans = {
      {1, {2}}, {2, {3}}, {4, {5}}, {5, {6}}, {6, {7, 10}}, {7, {8}}, {8, {9}}, {9, {6}},
      {10, {11, 12}}, {11, {13}}, {12, {13}}, {13, {14}}, {14, {15}}, {16, {17}}, {17, {18}},
  };
  std::map<int, std::set<int>> cfg_al = *pkb.get_cfg_al();
  REQUIRE(cfg_al.size() == ans.size());
  for (auto &[k, vals] : ans) {
    REQUIRE(cfg_al.find(k) != cfg_al.end());
    REQUIRE(cfg_al[k].size() == vals.size());
    for (auto &val : vals) {
      REQUIRE(cfg_al[k].find(val) != cfg_al[k].end());
    }
  }
}

TEST_CASE("PKB_CFGNestedIf_Correct") {
  std::string source =
      "procedure main {"
      "  zero = 1;"
      "  if(a==1) then {"
      "    if(b==1) then {"
      "      first = 1;"
      "    }else{"
      "      second = 1;"
      "    }"
      "  } else {"
      "    if(c==1) then {"
      "      third = 1;"
      "    } else {"
      "      fourth = 1;"
      "    }"
      "  }"
      "  fifth = 1;"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> ans = {
      {1, {2}}, {2, {3, 6}}, {3, {4, 5}}, {4, {9}}, {5, {9}}, {6, {7, 8}}, {7, {9}}, {8, {9}}
  };
  std::map<int, std::set<int>> cfg_al = *pkb.get_cfg_al();
  REQUIRE(cfg_al.size() == ans.size());
  for (auto &[k, vals] : ans) {
    REQUIRE(cfg_al.find(k) != cfg_al.end());
    REQUIRE(cfg_al[k].size() == vals.size());
    for (auto &val : vals) {
      REQUIRE(cfg_al[k].find(val) != cfg_al[k].end());
    }
  }
}

TEST_CASE("PKB_NextSample_Correct") {
  std::string source =
      "procedure First {"
      "  read x;"
      "  read y;"
      "  call Second; }"
      "procedure Second {"
      "  x = 0;"
      "  i = 5;"
      "  while (i != 0) {"
      "    x = x + 2 * y;"
      "    call Third;"
      "    i = i - 1; "
      "  }"
      "  if (x == 1) then {"
      "    x = x + 1; }"
      "  else {"
      "    z = 1;"
      "  }"
      "  z = z + x + i;"
      "  y = z + 2;"
      "  x = x * y + z; }"
      "procedure Third {"
      "  z = 5;"
      "  v = z;"
      "  print v; }";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> next_ans = {
      {1, {2}}, {2, {3}}, {3, {}}, {4, {5}}, {5, {6}}, {6, {7, 10}}, {7, {8}},
      {8, {9}}, {9, {6}}, {10, {11, 12}}, {11, {13}}, {12, {13}}, {13, {14}},
      {14, {15}}, {15, {}}, {16, {17}}, {17, {18}},
  };

  // Check Next(_,_)
  std::set<std::pair<int, int>> *next_wild_wild = pkb.get_next(PKB::PKB::kWild, PKB::kWild);
  for (auto &[a, nexts] : next_ans) {
    for (auto &b : nexts) {
      REQUIRE(next_wild_wild->find({a, b}) != next_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Next(a,_)
  for (auto& [a, nexts] : next_ans) {
    std::set<std::pair<int, int>>* next_a_wild = pkb.get_next(a, PKB::kWild);
    REQUIRE(next_ans[a].size() == next_a_wild->size());
    for (auto& b : nexts) {
      REQUIRE(next_a_wild->find({ a, b }) != next_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Next(a,b)
  for (auto& [a, nexts] : next_ans) {
    for (auto& b : nexts) {
      std::set<std::pair<int, int>>* next_a_b = pkb.get_next(a, b);
      REQUIRE(next_a_b->size() == 1);
      REQUIRE(next_a_b->find({ a, b }) != next_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> prev_ans = {
      {1, {}}, {2, {1}}, {3, {2}}, {4, {}}, {5, {4}}, {6, {5, 9}}, {7, {6}}, {8, {7}}, {9, {8}},
      {10, {6}}, {11, {10}}, {12, {10}}, {13, {11, 12}}, {14, {13}}, {15, {14}}, {16, {}},
      {17, {16}}, {18, {17}},
  };
  for (auto&[b, prevs] : prev_ans) {
    // Check Next(_,b)
    std::set<std::pair<int, int>> *next_wild_b = pkb.get_next(PKB::kWild, b);
    REQUIRE(prev_ans[b].size() == next_wild_b->size());
    for (auto &a : prevs) {
      REQUIRE(next_wild_b->find({a, b}) != next_wild_b->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Negative cases
  REQUIRE(pkb.get_next(1, 3)->empty()); // Not directly after
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(3, 4)->empty()); // Across procedures
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(9, 7)->empty()); // Last line of while loop goes through condition first
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(11, 12)->empty()); // Different if branches
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(PKB::kWild, 1)->empty()); // First statement of procedure
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(15, PKB::kWild)->empty()); // Last statement of procedure
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
}

TEST_CASE("PKB_NextStarSample_Correct") {
  std::string source =
      "procedure First {"
      "  read x;"
      "  read y;"
      "  call Second; }"
      "procedure Second {"
      "  x = 0;"
      "  i = 5;"
      "  while (i != 0) {"
      "    x = x + 2 * y;"
      "    call Third;"
      "    i = i - 1; "
      "  }"
      "  if (x == 1) then {"
      "    x = x + 1; }"
      "  else {"
      "    z = 1;"
      "  }"
      "  z = z + x + i;"
      "  y = z + 2;"
      "  x = x * y + z; }"
      "procedure Third {"
      "  z = 5;"
      "  v = z;"
      "  print v; }";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> next_star_ans = {
      {1, {2, 3}},
      {2, {3}},
      {3, {}},
      {4, {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
      {5, {6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
      {6, {6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
      {7, {6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
      {8, {6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
      {9, {6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
      {10, {11, 12, 13, 14, 15}},
      {11, {13, 14, 15}},
      {12, {13, 14, 15}},
      {13, {14, 15}},
      {14, {15}},
      {15, {}},
      {16, {17, 18}},
      {17, {18}},
  };

  std::set<std::pair<int, int>> *next_star_wild_wild = pkb.get_next_star(PKB::kWild, PKB::kWild);
  for (auto&[a, nexts_star] : next_star_ans) {
    // Check Next*(_,_)
    for (auto &b : nexts_star) {
      REQUIRE(next_star_wild_wild->find({a, b}) != next_star_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Next*(a,_)
  for (auto& [a, nexts_star] : next_star_ans) {
    std::set<std::pair<int, int>>* next_star_a_wild = pkb.get_next_star(a, PKB::kWild);
    REQUIRE(next_star_ans[a].size() == next_star_a_wild->size());
    for (auto& b : nexts_star) {
      REQUIRE(next_star_a_wild->find({ a, b }) != next_star_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }  

  // Check Next*(a,b)
  for (auto& [a, nexts_star] : next_star_ans) {
    for (auto& b : nexts_star) {
      std::set<std::pair<int, int>>* next_star_a_b = pkb.get_next_star(a, b);
      REQUIRE(next_star_a_b->size() == 1);
      REQUIRE(next_star_a_b->find({ a, b }) != next_star_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> prev_star_ans = {
      {1, {}},
      {2, {1}},
      {3, {1, 2}},
      {4, {}},
      {5, {4}},
      {6, {4, 5, 6, 7, 8, 9}},
      {7, {4, 5, 6, 7, 8, 9}},
      {8, {4, 5, 6, 7, 8, 9}},
      {9, {4, 5, 6, 7, 8, 9}},
      {10, {4, 5, 6, 7, 8, 9}},
      {11, {4, 5, 6, 7, 8, 9, 10}},
      {12, {4, 5, 6, 7, 8, 9, 10}},
      {13, {4, 5, 6, 7, 8, 9, 10, 11, 12}},
      {14, {4, 5, 6, 7, 8, 9, 10, 11, 12, 13}},
      {15, {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
      {16, {}},
      {17, {16}},
      {18, {16, 17}},
  };
  // Check Next*(_,b)
  for (auto&[b, prevs_star] : prev_star_ans) {
    std::set<std::pair<int, int>> *next_star_wild_b = pkb.get_next_star(PKB::kWild, b);
    REQUIRE(prev_star_ans[b].size() == next_star_wild_b->size());
    for (auto &a : prevs_star) {
      REQUIRE(next_star_wild_b->find({a, b}) != next_star_wild_b->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Negative cases
  REQUIRE(pkb.get_next_star(2, 1)->empty()); // Reverse direction
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next_star(3, 4)->empty()); // Across procedures
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next_star(11, 12)->empty()); // Different if branches
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next_star(PKB::kWild, 1)->empty()); // First statement of procedure
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next_star(15, PKB::kWild)->empty()); // Last statement of procedure
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
}

TEST_CASE("PKB_NextNestedIf_Correct") {
  std::string source =
      "procedure main {"
      "  zero = 1;"
      "  if(a==1) then {"
      "    if(b==1) then {"
      "      first = 1;"
      "    }else{"
      "      second = 1;"
      "    }"
      "  } else {"
      "    if(c==1) then {"
      "      third = 1;"
      "    } else {"
      "      fourth = 1;"
      "    }"
      "  }"
      "  fifth = 1;"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> next_ans = {
      {1, {2}}, {2, {3, 6}}, {3, {4, 5}}, {4, {9}}, {5, {9}}, {6, {7, 8}}, {7, {9}},
      {8, {9}}, {9, {}}
  };
  // Check Next(_,_)
  std::set<std::pair<int, int>>* next_wild_wild = pkb.get_next(PKB::PKB::kWild, PKB::kWild);
  for (auto& [a, nexts] : next_ans) {
    for (auto& b : nexts) {
      REQUIRE(next_wild_wild->find({ a, b }) != next_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Next(a,_)
  for (auto& [a, nexts] : next_ans) {
    std::set<std::pair<int, int>>* next_a_wild = pkb.get_next(a, PKB::kWild);
    REQUIRE(next_ans[a].size() == next_a_wild->size());
    for (auto& b : nexts) {
      REQUIRE(next_a_wild->find({ a, b }) != next_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Next(a,b)
  for (auto& [a, nexts] : next_ans) {
    for (auto& b : nexts) {
      std::set<std::pair<int, int>>* next_a_b = pkb.get_next(a, b);
      REQUIRE(next_a_b->size() == 1);
      REQUIRE(next_a_b->find({ a, b }) != next_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> prev_ans = {
      {1, {}}, {2, {1}}, {3, {2}}, {4, {3}}, {5, {3}}, {6, {2}}, {7, {6}}, {8, {6}},
      {9, {4, 5, 7, 8}}
  };
  // Check Next(_,b)
  for (auto&[b, prevs] : prev_ans) {
    std::set<std::pair<int, int>> *next_wild_b = pkb.get_next(PKB::kWild, b);
    REQUIRE(prev_ans[b].size() == next_wild_b->size());
    for (auto &a : prevs) {
      REQUIRE(next_wild_b->find({a, b}) != next_wild_b->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Negative cases
  REQUIRE(pkb.get_next(1, 9)->empty()); // Not directly after
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(1, 3)->empty()); // Not directly after
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(4, 5)->empty()); // Different if branches
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(PKB::kWild, 1)->empty()); // First statement of procedure
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next(9, PKB::kWild)->empty()); // Last statement of procedure
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
}

TEST_CASE("PKB_NextStarNestedIf_Correct") {
  std::string source =
      "procedure main {"
      "  zero = 1;"
      "  if(a==1) then {"
      "    if(b==1) then {"
      "      first = 1;"
      "    }else{"
      "      second = 1;"
      "    }"
      "  } else {"
      "    if(c==1) then {"
      "      third = 1;"
      "    } else {"
      "      fourth = 1;"
      "    }"
      "  }"
      "  fifth = 1;"
      "}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> next_star_ans = {
      {1, {2, 3, 4, 5, 6, 7, 8, 9}},
      {2, {3, 4, 5, 6, 7, 8, 9}},
      {3, {4, 5, 9}},
      {4, {9}},
      {5, {9}},
      {6, {7, 8, 9}},
      {7, {9}},
      {8, {9}},
      {9, {}}
  };  

  // Check Next*(_,_)
  std::set<std::pair<int, int>>* next_star_wild_wild = pkb.get_next_star(PKB::kWild, PKB::kWild);
  for (auto& [a, nexts_star] : next_star_ans) {
    for (auto& b : nexts_star) {
      REQUIRE(next_star_wild_wild->find({ a, b }) != next_star_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Next*(a,_)
  for (auto& [a, nexts_star] : next_star_ans) {
    std::set<std::pair<int, int>>* next_star_a_wild = pkb.get_next_star(a, PKB::kWild);
    REQUIRE(next_star_ans[a].size() == next_star_a_wild->size());
    for (auto& b : nexts_star) {
      REQUIRE(next_star_a_wild->find({ a, b }) != next_star_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Next*(a,b)
  for (auto& [a, nexts_star] : next_star_ans) {
    for (auto& b : nexts_star) {
      std::set<std::pair<int, int>>* next_star_a_b = pkb.get_next_star(a, b);
      REQUIRE(next_star_a_b->size() == 1);
      REQUIRE(next_star_a_b->find({ a, b }) != next_star_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> prev_star_ans = {
      {1, {}},
      {2, {1}},
      {3, {1, 2}},
      {4, {1, 2, 3}},
      {5, {1, 2, 3}},
      {6, {1, 2}},
      {7, {1, 2, 6}},
      {8, {1, 2, 6}},
      {9, {1, 2, 3, 4, 5, 6, 7, 8}},
  };

  // Check Next*(_,b)
  for (auto&[b, prevs_star] : prev_star_ans) {
    std::set<std::pair<int, int>> *next_star_wild_b = pkb.get_next_star(PKB::kWild, b);
    REQUIRE(prev_star_ans[b].size() == next_star_wild_b->size());
    for (auto &a : prevs_star) {
      REQUIRE(next_star_wild_b->find({a, b}) != next_star_wild_b->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Negative cases
  REQUIRE(pkb.get_next_star(2, 1)->empty()); // Reverse direction
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next_star(7, 8)->empty()); // Different if branches
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next_star(PKB::kWild, 1)->empty()); // First statement of procedure
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_next_star(9, PKB::kWild)->empty()); // Last statement of procedure
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
}

TEST_CASE("PKB_AffectsSample_Correct") {
  std::string source =
      "procedure First {"
      "  read x;"
      "  read y;"
      "  call Second; }"
      "procedure Second {"
      "  x = 0;"
      "  i = 5;"
      "  while (i != 0) {"
      "    x = x + 2 * y;"
      "    call Third;"
      "    i = i - 1; "
      "  }"
      "  if (x == 1) then {"
      "    x = x + 1; }"
      "  else {"
      "    z = 1;"
      "  }"
      "  z = z + x + i;"
      "  y = z + 2;"
      "  x = x * y + z; }"
      "procedure Third {"
      "  z = 5;"
      "  v = z;"
      "  print v; }";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> affects_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {7, 11, 13, 15}}, {5, {9, 13}}, {6, {}},
      {7, {7, 11, 13, 15}}, {8, {}}, {9, {9, 13}}, {10, {}}, {11, {13, 15}}, {12, {13}},
      {13, {14, 15}}, {14, {15}}, {15, {}}, {16, {17}}, {17, {}}, {18, {}}, {19, {}},
      {20, {}}, {21, {}}, {22, {}}, {23, {}},
  };

  // Check Affects(_,_)
  std::set<std::pair<int, int>> *affects_wild_wild = pkb.get_affects(PKB::kWild, PKB::kWild);
  for (auto&[a, affects] : affects_ans) {
    for (auto &b : affects) {
      REQUIRE(affects_wild_wild->find({a, b}) != affects_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Affects(a,_)
  for (auto& [a, affects] : affects_ans) {
    std::set<std::pair<int, int>>* affects_a_wild = pkb.get_affects(a, PKB::kWild);
    REQUIRE(affects_ans[a].size() == affects_a_wild->size());
    for (auto& b : affects) {
      REQUIRE(affects_a_wild->find({ a, b }) != affects_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Affects(a,b)
  for (auto& [a, affects] : affects_ans) {
    for (auto& b : affects) {
      std::set<std::pair<int, int>>* affects_a_b = pkb.get_affects(a, b);
      REQUIRE(affects_a_b->size() == 1);
      REQUIRE(affects_a_b->find({ a, b }) != affects_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }


  std::map<int, std::vector<int>> affected_by_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {4, 7}}, {8, {}}, {9, {5, 9}},
      {10, {}}, {11, {4, 7}}, {12, {}}, {13, {4, 5, 7, 9, 11, 12}}, {14, {13}},
      {15, {4, 7, 11, 13, 14}}, {16, {}}, {17, {16}}, {18, {}}, {19, {}}, {20, {}}, {21, {}},
      {22, {}}, {23, {}},
  };
  // Check Affects(_,b)
  for (auto&[b, affected_bys] : affected_by_ans) {
    std::set<std::pair<int, int>> *affects_wild_b = pkb.get_affects(PKB::kWild, b);
    REQUIRE(affected_by_ans[b].size() == affects_wild_b->size());
    for (auto &a : affected_bys) {
      REQUIRE(affects_wild_b->find({a, b}) != affects_wild_b->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Negative cases
  REQUIRE(pkb.get_affects(16, 7)->empty()); // Different procedures
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_affects(5, 7)->empty()); // 5 is not modifying x or y
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_affects(5, 8)->empty()); // 8 is not an assignment statement
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_affects(8, 12)->empty()); // 8 is not an assignment statement
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_affects(8, PKB::kWild)->empty()); // 8 is not an assignment statement
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
}

TEST_CASE("PKB_AffectsStarSample_Correct") {
  std::string source =
      "procedure First {"
      "  read x;"
      "  read y;"
      "  call Second; }"
      "procedure Second {"
      "  x = 0;"
      "  i = 5;"
      "  while (i != 0) {"
      "    x = x + 2 * y;"
      "    call Third;"
      "    i = i - 1; "
      "  }"
      "  if (x == 1) then {"
      "    x = x + 1; }"
      "  else {"
      "    z = 1;"
      "  }"
      "  z = z + x + i;"
      "  y = z + 2;"
      "  x = x * y + z; }"
      "procedure Third {"
      "  z = 5;"
      "  v = z;"
      "  print v; }";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> affects_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {7, 11, 13, 14, 15}}, {5, {9, 13, 14, 15}}, {6, {}},
      {7, {7, 11, 13, 14, 15}}, {8, {}}, {9, {9, 13, 14, 15}}, {10, {}}, {11, {13, 14, 15}},
      {12, {13, 14, 15}}, {13, {14, 15}}, {14, {15}}, {15, {}}, {16, {17}}, {17, {}},
      {18, {}}, {19, {}}, {20, {}}, {21, {}}, {22, {}}, {23, {}},
  };
  // Check Affects*(_,_)
  std::set<std::pair<int, int>> *affects_star_wild_wild = pkb.get_affects_star(PKB::kWild, PKB::kWild);
  for (auto&[a, affects_star] : affects_star_ans) {
    for (auto &b : affects_star) {
      REQUIRE(affects_star_wild_wild->find({a, b}) != affects_star_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Affects*(a,_)
  for (auto& [a, affects_star] : affects_star_ans) {
    std::set<std::pair<int, int>>* affects_star_a_wild = pkb.get_affects_star(a, PKB::kWild);
    REQUIRE(affects_star_ans[a].size() == affects_star_a_wild->size());
    for (auto& b : affects_star) {
      REQUIRE(affects_star_a_wild->find({ a, b }) != affects_star_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Affects*(a,b)
  for (auto& [a, affects_star] : affects_star_ans) {
    for (auto& b : affects_star) {
      std::set<std::pair<int, int>>* affects_star_a_b = pkb.get_affects_star(a, b);
      REQUIRE(affects_star_a_b->size() == 1);
      REQUIRE(affects_star_a_b->find({ a, b }) != affects_star_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> affected_by_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {4, 7}}, {8, {}}, {9, {5, 9}},
      {10, {}}, {11, {4, 7}}, {12, {}}, {13, {4, 5, 7, 9, 11, 12}}, {14, {4, 5, 7, 9, 11, 12, 13}},
      {15, {4, 5, 7, 9, 11, 12, 13, 14}}, {16, {}}, {17, {16}}, {18, {}}, {19, {}}, {20, {}},
      {21, {}}, {22, {}}, {23, {}},
  };

  // Check Affects(_,b)
  for (auto&[b, affected_bys_star] : affected_by_star_ans) {
    std::set<std::pair<int, int>> *affects_star_wild_b = pkb.get_affects_star(PKB::kWild, b);
    REQUIRE(affected_by_star_ans[b].size() == affects_star_wild_b->size());
    for (auto &a : affected_bys_star) {
      REQUIRE(affects_star_wild_b->find({a, b}) != affects_star_wild_b->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Negative cases
  REQUIRE(pkb.get_affects_star(16, 7)->empty()); // Different procedures
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_affects_star(5, 7)->empty()); // 5 is not modifying x or y
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_affects_star(5, 8)->empty()); // 8 is not an assignment statement
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_affects_star(8, 12)->empty()); // 8 is not an assignment statement
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  REQUIRE(pkb.get_affects_star(8, PKB::kWild)->empty()); // 8 is not an assignment statement
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
}

TEST_CASE("PKB_AffectsNestedWhileIf_Correct") {
  std::string source =
      "procedure main {"
      "  x = 0;"
      "  read y;"
      "  call Second;"
      "  x = x + 1; }"
      "procedure Second {"
      "  x = 0;"
      "  i = 5;"
      "  while (i != 0) {"
      "    x = x + 2 * y;"
      "    call Third;"
      "    i = i - 1;"
      "    if (z == y) then {"
      "      j = 8;"
      "    } else {"
      "      y = j;"
      "    }}"
      "  if (x == 1) then {"
      "    x = x + 1;"
      "    z = y + c + v;"
      "  } else {"
      "    z = z + i;}"
      "  z = z + x + i;"
      "  y = z + 2;"
      "  x = x * y + z; }"
      "procedure Third {"
      "  z = 5;"
      "  v = z;"
      "  print v;}";

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb(p);

  std::map<int, std::vector<int>> affects_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {8, 15, 18, 20}}, {6, {10, 17, 18}}, {7, {}},
      {8, {8, 15, 18, 20}}, {9, {}}, {10, {10, 17, 18}}, {11, {}}, {12, {13}}, {13, {8, 16}},
      {14, {}}, {15, {18, 20}}, {16, {18}}, {17, {18}}, {18, {19, 20}}, {19, {20}}, {20, {}},
      {21, {22}}, {22, {}}, {23, {}}
  };

  // Check Affects(_,_)
  std::set<std::pair<int, int>> *affects_wild_wild = pkb.get_affects(PKB::kWild, PKB::kWild);
  for (auto&[a, affects] : affects_ans) {
    for (auto &b : affects) {
      REQUIRE(affects_wild_wild->find({a, b}) != affects_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Affects(a,_)
  for (auto& [a, affects] : affects_ans) {
    std::set<std::pair<int, int>>* affects_a_wild = pkb.get_affects(a, PKB::kWild);
    REQUIRE(affects_ans[a].size() == affects_a_wild->size());
    for (auto& b : affects) {
      REQUIRE(affects_a_wild->find({ a, b }) != affects_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Affects(a,b)
  for (auto& [a, affects] : affects_ans) {
    for (auto& b : affects) {
      std::set<std::pair<int, int>>* affects_a_b = pkb.get_affects(a, b);
      REQUIRE(affects_a_b->size() == 1);
      REQUIRE(affects_a_b->find({ a, b }) != affects_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> affected_by_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {5, 8, 13}},
      {9, {}}, {10, {6, 10}}, {11, {}}, {12, {}}, {13, {12}}, {14, {}}, {15, {5, 8}},
      {16, {13}}, {17, {6, 10}}, {18, {5, 6, 8, 10, 15, 16, 17}}, {19, {18}},
      {20, {5, 8, 15, 18, 19}}, {22, {21}}
  };

  // Check Affects(_,b)
  for (auto&[b, affected_bys] : affected_by_ans) {
    std::set<std::pair<int, int>> *affects_wild_b = pkb.get_affects(PKB::kWild, b);
    REQUIRE(affected_by_ans[b].size() == affects_wild_b->size());
    for (auto &a : affected_bys) {
      REQUIRE(affects_wild_b->find({a, b}) != affects_wild_b->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }
}

TEST_CASE("PKB_NextCacheTestTime_correct") {
  std::string source =
    "procedure main {"
    "  flag = 0;"
    "  call computeCentroid;"
    "  call printResults;"
    "}"
    "procedure readPoint {"
    "  read x;"
    "  read y;"
    "}"
    "procedure printResults {"
    "  print flag;"
    "  print cenX;"
    "  print cenY;"
    "  print normSq;"
    "}"
    "procedure computeCentroid {"
    "  count = 0;"
    "  cenX = 0;"
    "  cenY = 0;"
    "  call readPoint;"
    "  while((x != 0) && (y != 0)) {"
    "    count = count+1;"
    "    cenX = cenX + x;"
    "    cenY = cenY + y;"
    "    call readPoint;"
    "  }"
    "  if (count == 0) then {"
    "    flag = 1;"
    "  } else {"
    "    cenX = cenX / count;"
    "    cenY = cenY / count;"
    "  }"
    "  normSq = cenX * cenX + cenY * cenY;"
    "}";
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode* p = ParseProgram(&lexer, &s);
  PKB pkb(p);
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next(i, j);
    }
    pkb.get_next(i, pkb.kWild);
    pkb.get_next(pkb.kWild, i);
  }
  pkb.get_next(pkb.kWild, pkb.kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  pkb.get_next(pkb.kWild, pkb.kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next(i, j);
    }
    pkb.get_next(i, pkb.kWild);
    pkb.get_next(pkb.kWild, i);
  }
  pkb.get_next(pkb.kWild, pkb.kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "next empty cache elapsed time: " << empty_cache_elapsed_seconds.count() << "s\n";
  std::cout << "next full cache elapsed time: " << full_cache_elapsed_seconds.count() << "s\n";
  REQUIRE(full_cache_elapsed_seconds.count() < empty_cache_elapsed_seconds.count());
}
TEST_CASE("PKB_NextStarCacheTestTime_correct") {
  std::string source =
    "procedure main {"
    "  flag = 0;"
    "  call computeCentroid;"
    "  call printResults;"
    "}"
    "procedure readPoint {"
    "  read x;"
    "  read y;"
    "}"
    "procedure printResults {"
    "  print flag;"
    "  print cenX;"
    "  print cenY;"
    "  print normSq;"
    "}"
    "procedure computeCentroid {"
    "  count = 0;"
    "  cenX = 0;"
    "  cenY = 0;"
    "  call readPoint;"
    "  while((x != 0) && (y != 0)) {"
    "    count = count+1;"
    "    cenX = cenX + x;"
    "    cenY = cenY + y;"
    "    call readPoint;"
    "  }"
    "  if (count == 0) then {"
    "    flag = 1;"
    "  } else {"
    "    cenX = cenX / count;"
    "    cenY = cenY / count;"
    "  }"
    "  normSq = cenX * cenX + cenY * cenY;"
    "}";
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode* p = ParseProgram(&lexer, &s);
  PKB pkb(p);
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next_star(i, j);
    }
    pkb.get_next_star(i, pkb.kWild);
    pkb.get_next_star(pkb.kWild, i);
  }
  pkb.get_next_star(pkb.kWild, pkb.kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  pkb.get_next_star(pkb.kWild, pkb.kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next_star(i, j);
    }
    pkb.get_next_star(i, pkb.kWild);
    pkb.get_next_star(pkb.kWild, i);
  }
  pkb.get_next_star(pkb.kWild, pkb.kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "next* empty cache elapsed time: " << empty_cache_elapsed_seconds.count() << "s\n";
  std::cout << "next* full cache elapsed time: " << full_cache_elapsed_seconds.count() << "s\n";
  REQUIRE(full_cache_elapsed_seconds.count() < empty_cache_elapsed_seconds.count());
}

TEST_CASE("PKB_AffectsCacheTestTime_correct") {
  std::string source =
    "procedure main {"
    "  flag = 0;"
    "  call computeCentroid;"
    "  call printResults;"
    "}"
    "procedure readPoint {"
    "  read x;"
    "  read y;"
    "}"
    "procedure printResults {"
    "  print flag;"
    "  print cenX;"
    "  print cenY;"
    "  print normSq;"
    "}"
    "procedure computeCentroid {"
    "  count = 0;"
    "  cenX = 0;"
    "  cenY = 0;"
    "  call readPoint;"
    "  while((x != 0) && (y != 0)) {"
    "    count = count+1;"
    "    cenX = cenX + x;"
    "    cenY = cenY + y;"
    "    call readPoint;"
    "  }"
    "  if (count == 0) then {"
    "    flag = 1;"
    "  } else {"
    "    cenX = cenX / count;"
    "    cenY = cenY / count;"
    "  }"
    "  normSq = cenX * cenX + cenY * cenY;"
    "}";
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode* p = ParseProgram(&lexer, &s);
  PKB pkb(p);
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (auto& a : pkb.get_statements(NodeType::Assign)) {
    for (auto& b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects(a->get_stmt_no(), pkb.kWild);
    pkb.get_affects(pkb.kWild, a->get_stmt_no());
  }
  pkb.get_affects(pkb.kWild, pkb.kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  pkb.get_affects(pkb.kWild, pkb.kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (auto& a : pkb.get_statements(NodeType::Assign)) {
    for (auto& b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects(a->get_stmt_no(), pkb.kWild);
    pkb.get_affects(pkb.kWild, a->get_stmt_no());
  }
  pkb.get_affects(pkb.kWild, pkb.kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "affects empty cache elapsed time: " << empty_cache_elapsed_seconds.count() << "s\n";
  std::cout << "affects full cache elapsed time: " << full_cache_elapsed_seconds.count() << "s\n";
  REQUIRE(full_cache_elapsed_seconds.count() < empty_cache_elapsed_seconds.count());
}

TEST_CASE("PKB_AffectsStarCacheTestTime_correct") {
  std::string source =
    "procedure main {"
    "  flag = 0;"
    "  call computeCentroid;"
    "  call printResults;"
    "}"
    "procedure readPoint {"
    "  read x;"
    "  read y;"
    "}"
    "procedure printResults {"
    "  print flag;"
    "  print cenX;"
    "  print cenY;"
    "  print normSq;"
    "}"
    "procedure computeCentroid {"
    "  count = 0;"
    "  cenX = 0;"
    "  cenY = 0;"
    "  call readPoint;"
    "  while((x != 0) && (y != 0)) {"
    "    count = count+1;"
    "    cenX = cenX + x;"
    "    cenY = cenY + y;"
    "    call readPoint;"
    "  }"
    "  if (count == 0) then {"
    "    flag = 1;"
    "  } else {"
    "    cenX = cenX / count;"
    "    cenY = cenY / count;"
    "  }"
    "  normSq = cenX * cenX + cenY * cenY;"
    "}";
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode* p = ParseProgram(&lexer, &s);
  PKB pkb(p);
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (auto& a : pkb.get_statements(NodeType::Assign)) {
    for (auto& b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects_star(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects_star(a->get_stmt_no(), pkb.kWild);
    pkb.get_affects_star(pkb.kWild, a->get_stmt_no());
  }
  pkb.get_affects_star(pkb.kWild, pkb.kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  pkb.get_affects_star(pkb.kWild, pkb.kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (auto& a : pkb.get_statements(NodeType::Assign)) {
    for (auto& b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects_star(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects_star(a->get_stmt_no(), pkb.kWild);
    pkb.get_affects_star(pkb.kWild, a->get_stmt_no());
  }
  pkb.get_affects_star(pkb.kWild, pkb.kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "affects* empty cache elapsed time: " << empty_cache_elapsed_seconds.count() << "s\n";
  std::cout << "affects* full cache elapsed time: " << full_cache_elapsed_seconds.count() << "s\n";
  REQUIRE(full_cache_elapsed_seconds.count() < empty_cache_elapsed_seconds.count());
}