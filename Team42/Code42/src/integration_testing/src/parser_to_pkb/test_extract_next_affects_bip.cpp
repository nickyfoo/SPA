#include "catch.hpp"
#include <vector>
#include <chrono>

#include "../test_utils.hpp"
#include <parse.h>
#include <pkb.h>
#include "entities/statement.h"

TEST_CASE("PkbNextBip_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  std::map<int, std::vector<int>> next_bip_ans = {
      {1, {2}}, {2, {6}}, {3, {4}}, {4, {9}}, {5, {}}, {6, {7}}, {7, {9}},
      {8, {3}}, {9, {10, 11}}, {10, {5, 8}}, {11, {5, 8}},
  };

  // Check NextBip(_,_)
  std::set<std::pair<int, int>> *next_bip_wild_wild = pkb.get_next_bip(PKB::kWild, PKB::kWild);

  for (auto&[a, nexts] : next_bip_ans) {
    for (auto &b : nexts) {
      REQUIRE(next_bip_wild_wild->find({a, b}) != next_bip_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // Check NextBip(a,_)
  for (auto&[a, nexts] : next_bip_ans) {
    std::set<std::pair<int, int>> *next_bip_a_wild = pkb.get_next_bip(a, PKB::kWild);
    REQUIRE(next_bip_ans[a].size() == next_bip_a_wild->size());
    for (auto &b : nexts) {
      REQUIRE(next_bip_a_wild->find({a, b}) != next_bip_a_wild->end());
    }
    pkb.ClearNextAffectsBipCache();
    REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  }

  // Check NextBip(a,b)
  for (auto&[a, nexts] : next_bip_ans) {
    for (auto &b : nexts) {
      std::set<std::pair<int, int>> *next_bip_a_b = pkb.get_next_bip(a, b);
      REQUIRE(next_bip_a_b->size() == 1);
      REQUIRE(next_bip_a_b->find({a, b}) != next_bip_a_b->end());
      pkb.ClearNextAffectsBipCache();
      REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> prev_bip_ans = {
      {1, {}}, {2, {1}}, {3, {8}}, {4, {3}}, {5, {10, 11}}, {6, {2}}, {7, {6}}, {8, {10, 11}},
      {9, {4, 7}}, {10, {9}}, {11, {9}},
  };
  for (auto&[b, prevs] : prev_bip_ans) {
    // Check NextBip(_,b)
    std::set<std::pair<int, int>> *next_bip_wild_b = pkb.get_next_bip(PKB::kWild, b);
    REQUIRE(prev_bip_ans[b].size() == next_bip_wild_b->size());
    for (auto &a : prevs) {
      REQUIRE(next_bip_wild_b->find({a, b}) != next_bip_wild_b->end());
    }
    pkb.ClearNextAffectsBipCache();
    REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  }
}

TEST_CASE("PkbNextBip_SampleProgram_NegativeCases") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  // Not directly after
  REQUIRE(pkb.get_next_bip(1, 3)->empty());
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // Not directly across procedures
  REQUIRE(pkb.get_next_bip(2, 7)->empty());
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // Different if branches
  REQUIRE(pkb.get_next_bip(10, 11)->empty());
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // First statement of first procedure
  REQUIRE(pkb.get_next_bip(PKB::kWild, 1)->empty());
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // Last statement of last procedure
  REQUIRE(pkb.get_next_bip(5, PKB::kWild)->empty());
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
}

TEST_CASE("PkbNextBipStar_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  std::map<int, std::vector<int>> next_bip_star_ans = {
      {1, {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},
      {2, {3, 4, 5, 6, 7, 8, 9, 10, 11}},
      {3, {4, 5, 9, 10, 11}},
      {4, {5, 9, 10, 11}},
      {5, {}},
      {6, {3, 4, 5, 7, 8, 9, 10, 11}},
      {7, {3, 4, 5, 8, 9, 10, 11}},
      {8, {3, 4, 5, 9, 10, 11}},
      {9, {3, 4, 5, 8, 9, 10, 11}},
      {10, {3, 4, 5, 8, 9, 10, 11}},
      {11, {3, 4, 5, 8, 9, 10, 11}},
  };

  // Check NextBip*(_,_)
  std::set<std::pair<int, int>> *next_bip_star_wild_wild =
      pkb.get_next_bip_star(PKB::kWild, PKB::kWild);
  for (auto&[a, nexts] : next_bip_star_ans) {
    for (auto &b : nexts) {
      REQUIRE(next_bip_star_wild_wild->find({a, b}) != next_bip_star_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // Check NextBip*(a,_)
  for (auto&[a, nexts] : next_bip_star_ans) {
    std::set<std::pair<int, int>> *next_bip_star_a_wild = pkb.get_next_bip_star(a, PKB::kWild);
    REQUIRE(next_bip_star_ans[a].size() == next_bip_star_a_wild->size());
    for (auto &b : nexts) {
      REQUIRE(next_bip_star_a_wild->find({a, b}) != next_bip_star_a_wild->end());
    }
    pkb.ClearNextAffectsBipCache();
    REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  }

  // Check NextBip*(a,b)
  for (auto&[a, nexts] : next_bip_star_ans) {
    for (auto &b : nexts) {
      std::set<std::pair<int, int>> *next_bip_star_a_b = pkb.get_next_bip_star(a, b);
      REQUIRE(next_bip_star_a_b->size() == 1);
      REQUIRE(next_bip_star_a_b->find({a, b}) != next_bip_star_a_b->end());
      pkb.ClearNextAffectsBipCache();
      REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
    }
  }
  std::map<int, std::vector<int>> prev_bip_star_ans = {
      {1, {}},
      {2, {1}},
      {3, {1, 2, 6, 7, 8, 9, 10, 11}},
      {4, {1, 2, 3, 6, 7, 8, 9, 10, 11}},
      {5, {1, 2, 3, 4, 6, 7, 8, 9, 10, 11}},
      {6, {1, 2}},
      {7, {1, 2, 6}},
      {8, {1, 2, 6, 7, 9, 10, 11}},
      {9, {1, 2, 3, 4, 6, 7, 8, 9, 10, 11}},
      {10, {1, 2, 3, 4, 6, 7, 8, 9, 10, 11}},
      {11, {1, 2, 3, 4, 6, 7, 8, 9, 10, 11}},
  };
  for (auto&[b, prevs] : prev_bip_star_ans) {
    // Check NextBip(_,b)
    std::set<std::pair<int, int>> *next_bip_star_wild_b = pkb.get_next_bip_star(PKB::kWild, b);
    REQUIRE(prev_bip_star_ans[b].size() == next_bip_star_wild_b->size());
    for (auto &a : prevs) {
      REQUIRE(next_bip_star_wild_b->find({a, b}) != next_bip_star_wild_b->end());
    }
    pkb.ClearNextAffectsBipCache();
    REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  }
}

TEST_CASE("PkbNextBipStar_SampleProgram_NegativeCases") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  // Reverse direction
  REQUIRE(pkb.get_next_bip_star(2, 1)->empty());
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // First statement of first procedure
  REQUIRE(pkb.get_next_bip_star(PKB::kWild, 1)->empty());
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // Last statement of last procedure
  REQUIRE(pkb.get_next_bip_star(5, PKB::kWild)->empty());
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
}

TEST_CASE("PkbAffectsBip_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kCfgBipSample);
  pkb.PrintProcedures();
  pkb.PrintCFGBipAL();

  std::map<int, std::vector<int>> affects_bip_ans = {
      {1, {3, 5, 6, 8, 10, 11}},
      {2, {}},
      {3, {5, 11}},
      {4, {}},
      {5, {}},
      {6, {8, 11}},
      {7, {}},
      {8, {10}},
      {9, {}},
      {10, {3, 5, 8, 10, 11}},
      {11, {5, 8}},
  };

  // Check Affects(_,_)
  std::set<std::pair<int, int>> *affects_bip_wild_wild =
      pkb.get_affects_bip(PKB::kWild, PKB::kWild);
  for (auto& [a, b] : *affects_bip_wild_wild) {
    std::cout << a << "," << b << '\n';
  }
  for (auto&[a, affects] : affects_bip_ans) {
    for (auto &b : affects) {
      std::cout << a << ' ' << b << '\n';
      REQUIRE(affects_bip_wild_wild->find({a, b}) != affects_bip_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  // Check Affects(a,_)
  for (auto&[a, affects] : affects_bip_ans) {
    std::set<std::pair<int, int>> *affects_bip_a_wild = pkb.get_affects_bip(a, PKB::kWild);
    REQUIRE(affects_bip_ans[a].size() == affects_bip_a_wild->size());
    for (auto &b : affects) {
      REQUIRE(affects_bip_a_wild->find({a, b}) != affects_bip_a_wild->end());
    }
    pkb.ClearNextAffectsBipCache();
    REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  }

  // Check Affects(a,b)
  for (auto&[a, affects] : affects_bip_ans) {
    for (auto &b : affects) {
      std::set<std::pair<int, int>> *affects_bip_a_b = pkb.get_affects_bip(a, b);
      REQUIRE(affects_bip_a_b->size() == 1);
      REQUIRE(affects_bip_a_b->find({a, b}) != affects_bip_a_b->end());
      pkb.ClearNextAffectsBipCache();
      REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> affected_by_bip_ans = {
      {1, {}},
      {2, {}},
      {3, {1, 10}},
      {4, {}},
      {5, {1, 3, 10, 11}},
      {6, {1}},
      {7, {}},
      {8, {1, 6, 10, 11}},
      {9, {}},
      {10, {1, 8, 10}},
      {11, {1, 3, 6, 10}},
  };

  // Check Affects(_,b)
  for (auto&[b, affected_bys] : affected_by_bip_ans) {
    std::set<std::pair<int, int>> *affects_bip_wild_b = pkb.get_affects_bip(PKB::kWild, b);
    REQUIRE(affected_by_bip_ans[b].size() == affects_bip_wild_b->size());
    for (auto &a : affected_bys) {
      REQUIRE(affects_bip_wild_b->find({a, b}) != affects_bip_wild_b->end());
    }
    pkb.ClearNextAffectsBipCache();
    REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  }
}

TEST_CASE("PkbAffectsBip_SampleProgram_NegativeCases") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  // Invalid statement number
  REQUIRE(pkb.get_affects_bip(0, 1)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // 1 is the first statement of cfg
  REQUIRE(pkb.get_affects_bip(PKB::kWild, 1)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // 5 is the last statement of cfg
  REQUIRE(pkb.get_affects_bip(5, PKB::kWild)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // 2 is not an assignment statement
  REQUIRE(pkb.get_affects_bip(2, PKB::kWild)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // 4 is not an assignment statement
  REQUIRE(pkb.get_affects_bip(3, 4)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
}

TEST_CASE("PkbAffectsBipStar_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  std::map<int, std::vector<int>> affects_bip_star_ans = {
      {1, {3, 5, 6, 8, 10, 11}}, {2, {}}, {3, {5, 11}}, {4, {}}, {5, {}}, {6, {5, 8, 10, 11}},
      {7, {}}, {8, {5, 10}}, {9, {}}, {10, {3, 5, 8, 10, 11}}, {11, {5, 8, 10}}
  };

  // Check AffectsBip*(_,_)
  std::set<std::pair<int, int>> *affects_bip_star_wild_wild =
      pkb.get_affects_bip_star(PKB::kWild, PKB::kWild);
  for (auto&[a, affects_star] : affects_bip_star_ans) {
    for (auto &b : affects_star) {
      REQUIRE(affects_bip_star_wild_wild->find({a, b}) != affects_bip_star_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check AffectsBip*(a,_)
  for (auto&[a, affects_star] : affects_bip_star_ans) {
    std::set<std::pair<int, int>> *affects_bip_star_a_wild =
        pkb.get_affects_bip_star(a, PKB::kWild);
    REQUIRE(affects_bip_star_ans[a].size() == affects_bip_star_a_wild->size());
    for (auto &b : affects_star) {
      REQUIRE(affects_bip_star_a_wild->find({a, b}) != affects_bip_star_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check AffectsBip*(a,b)
  for (auto&[a, affects_star] : affects_bip_star_ans) {
    for (auto &b : affects_star) {
      std::set<std::pair<int, int>> *affects_bip_star_a_b = pkb.get_affects_bip_star(a, b);
      REQUIRE(affects_bip_star_a_b->size() == 1);
      REQUIRE(affects_bip_star_a_b->find({a, b}) != affects_bip_star_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }

  std::map<int, std::vector<int>> affected_by_bip_star_ans = {
      {1, {}}, {2, {}}, {3, {1, 10}}, {4, {}}, {5, {1, 3, 6, 8, 10, 11}}, {6, {1}}, {7, {}},
      {8, {1, 6, 10, 11}}, {9, {}}, {10, {1, 6, 8, 10, 11}}, {11, {1, 3, 6, 10}}
  };

  // Check AffectsBip*(_,b)
  for (auto&[b, affected_bys_star] : affected_by_bip_star_ans) {
    std::set<std::pair<int, int>> *affects_bip_star_wild_b =
        pkb.get_affects_bip_star(PKB::kWild, b);
    REQUIRE(affected_by_bip_star_ans[b].size() == affects_bip_star_wild_b->size());
    for (auto &a : affected_bys_star) {
      REQUIRE(affects_bip_star_wild_b->find({a, b}) != affects_bip_star_wild_b->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }
}

TEST_CASE("PkbAffectsBipStar_SampleProgram_NegativeCases") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  // Invalid statement number
  REQUIRE(pkb.get_affects_bip_star(0, 1)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // 1 is the first statement of cfg
  REQUIRE(pkb.get_affects_bip_star(PKB::kWild, 1)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // 5 is the last statement of cfg
  REQUIRE(pkb.get_affects_bip_star(5, PKB::kWild)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // 2 is not an assignment statement
  REQUIRE(pkb.get_affects_bip_star(2, PKB::kWild)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // 4 is not an assignment statement
  REQUIRE(pkb.get_affects_bip_star(3, 4)->empty());
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
}


TEST_CASE("GAY_TEST") {
  std::string program =
    "procedure foo {"
    "  read x;"
    "  read y;"
    "  length = x * x + y * y;"
    "  t = 30;"
    "  call bar;"
    "  print length;"
    "}"
    ""
    "procedure bar {"
    "  if ((x >= 0) && (y >= 0)) then {"
    "    while (x != 0) {"
    "      tan = y / x;"
    "      sin = y / length;"
    "      cos = x / length;"
    "      call ellipse;"
    "      if (((sin * sin + cos * cos) == 1) || (!(tan != (sin / cos)))) then {"
    "        x = y / 2;"
    "        y = x / 2;"
    "      } else {"
    "        x = 0;"
    "      }"
    "      length = x * x + y * y;"
    "    }"
    "  } else {"
    "    x = 10;"
    "    y = 10;"
    "    length = x * x + y * y;"
    "  }"
    "}"
    ""
    "procedure ellipse {"
    "  pi = 3;"
    "  area = pi * x * y;"
    "  if (area < 10) then {"
    "    x = x + 1;"
    "    y = y + y / x + 1;"
    "  } else {"
    "    print area;"
    "  }"
    "}";

  BufferedLexer lexer(program);
  ParseState s{};
  ProgramNode* p = ParseProgram(&lexer, &s);
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);
  design_extractor.ExtractDesigns();
}

TEST_CASE("GAY_TEST2") {
  std::string program =
    "procedure a {"
    "    call b;"
    "    print x;"
    "}"
    ""
    "procedure b {"
    "    y = y + 1;"
    "    if (y > 0) then {"
    "        call c;"
    "    } else {"
    "        call d;"
    "    }"
    "}"
    ""
    "procedure c {"
    "    call e;"
    "    call d;"
    "}"
    ""
    "procedure d {"
    "    print a;"
    "    call e;"
    "}"
    ""
    "procedure e {"
    "    while (1 > 0) {"
    "        call f;"
    "    }"
    "}"
    ""
    "procedure f {"
    "    print pi;"
    "}"
    ""
    "procedure g {"
    "    call h;"
    "    call h;"
    "}"
    ""
    "procedure h {"
    "    call j;"
    "}"
    ""
    "procedure i {"
    "    call h;"
    "}"
    ""
    "procedure j {"
    "    print z;"
    "    if (z > 0) then {"
    "        call k;"
    "    } else {"
    "        print z;"
    "    }"
    "}"
    ""
    "procedure k {"
    "    print k;"
    "}";

  BufferedLexer lexer(program);
  ParseState s{};
  ProgramNode* p = ParseProgram(&lexer, &s);
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);
  design_extractor.ExtractDesigns();
}

TEST_CASE("GAY_TEST3") {
  std::string program =
    "procedure foo {"
    "  read x;"
    "  read y;"
    "  length = x * x + y * y;"
    "  t = 30;"
    "  call bar;"
    "  print length;"
    "}"
    ""
    "procedure bar {"
    "  if ((x >= 0) && (y >= 0)) then {"
    "    while (x != 0) {"
    "      tan = y / x;"
    "      sin = y / length;"
    "      cos = x / length;"
    "      call ellipse;"
    "      if (((sin * sin + cos * cos) == 1) || (!(tan != (sin / cos)))) then {"
    "        x = y / 2;"
    "        y = x / 2;"
    "      } else {"
    "        x = 0;"
    "      }"
    "      length = x * x + y * y;"
    "    }"
    "  } else {"
    "    x = 10;"
    "    y = 10;"
    "    length = x * x + y * y;"
    "  }"
    "}"
    ""
    "procedure ellipse {"
    "  pi = 3;"
    "  area = pi * x * y;"
    "  if (area < 10) then {"
    "    x = x + 1;"
    "    y = y + y / x + 1;"
    "  } else {"
    "    print area;"
    "  }"
    "}";

  BufferedLexer lexer(program);
  ParseState s{};
  ProgramNode* p = ParseProgram(&lexer, &s);
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);
  design_extractor.ExtractDesigns();
  pkb.PrintCFGBipAL();
  std::cout << "==================HELLO====================================================================================\n";
  /*
  for (auto& x : pkb.get_all_statements()) {
    std::cout << x->get_stmt_no() << ":\n";
    for (auto& cs : *x->get_call_stacks()) {
      std::cout << pkb.CallStackToString(&cs) << '\n';
    }
  }
  */
  std::set<std::pair<int, int>> ans = *pkb.get_affects_bip(15, 10);
  for (auto& [a, b] : ans) {
    std::cout << a << ',' << b << '\n';
  }
}

