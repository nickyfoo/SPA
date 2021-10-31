#include "catch.hpp"
#include <vector>
#include <chrono>

#include "../test_utils.hpp"
#include <parse.h>
#include <pkb.h>
#include "entities/statement.h"
#include <design_extractor.h>

TEST_CASE("PkbNext_AdvancedSample_Correct") {
  PKB pkb = InitialisePKB(kAdvancedSample);

  std::map<int, std::vector<int>> next_ans = {
      {1, {2}}, {2, {3}}, {3, {}}, {4, {5}}, {5, {6}}, {6, {7, 10}}, {7, {8}},
      {8, {9}}, {9, {6}}, {10, {11, 12}}, {11, {13}}, {12, {13}}, {13, {14}},
      {14, {15}}, {15, {}}, {16, {17}}, {17, {18}},
      };

  // Check Next(_,_)
  std::set<std::pair<int, int>> *next_wild_wild = pkb.get_next(PKB::kWild, PKB::kWild);
  for (auto &[a, nexts] : next_ans) {
    for (auto &b : nexts) {
      REQUIRE(next_wild_wild->find({a, b}) != next_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Next(a,_)
  for (auto&[a, nexts] : next_ans) {
    std::set<std::pair<int, int>> *next_a_wild = pkb.get_next(a, PKB::kWild);
    REQUIRE(next_ans[a].size() == next_a_wild->size());
    for (auto &b : nexts) {
      REQUIRE(next_a_wild->find({a, b}) != next_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Next(a,b)
  for (auto&[a, nexts] : next_ans) {
    for (auto &b : nexts) {
      std::set<std::pair<int, int>> *next_a_b = pkb.get_next(a, b);
      REQUIRE(next_a_b->size() == 1);
      REQUIRE(next_a_b->find({a, b}) != next_a_b->end());
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

TEST_CASE("PkbNextStar_AdvancedSample_Correct") {
  PKB pkb = InitialisePKB(kAdvancedSample);

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
  for (auto&[a, nexts_star] : next_star_ans) {
    std::set<std::pair<int, int>> *next_star_a_wild = pkb.get_next_star(a, PKB::kWild);
    REQUIRE(next_star_ans[a].size() == next_star_a_wild->size());
    for (auto &b : nexts_star) {
      REQUIRE(next_star_a_wild->find({a, b}) != next_star_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Next*(a,b)
  for (auto&[a, nexts_star] : next_star_ans) {
    for (auto &b : nexts_star) {
      std::set<std::pair<int, int>> *next_star_a_b = pkb.get_next_star(a, b);
      REQUIRE(next_star_a_b->size() == 1);
      REQUIRE(next_star_a_b->find({a, b}) != next_star_a_b->end());
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

TEST_CASE("PkbNext_NestedIf_Correct") {
  PKB pkb = InitialisePKB(kNestedIf);

  std::map<int, std::vector<int>> next_ans = {
      {1, {2}}, {2, {3, 6}}, {3, {4, 5}}, {4, {9}}, {5, {9}}, {6, {7, 8}}, {7, {9}},
      {8, {9}}, {9, {}}
  };
  // Check Next(_,_)
  std::set<std::pair<int, int>> *next_wild_wild = pkb.get_next(PKB::kWild, PKB::kWild);
  for (auto&[a, nexts] : next_ans) {
    for (auto &b : nexts) {
      REQUIRE(next_wild_wild->find({a, b}) != next_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Next(a,_)
  for (auto&[a, nexts] : next_ans) {
    std::set<std::pair<int, int>> *next_a_wild = pkb.get_next(a, PKB::kWild);
    REQUIRE(next_ans[a].size() == next_a_wild->size());
    for (auto &b : nexts) {
      REQUIRE(next_a_wild->find({a, b}) != next_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Next(a,b)
  for (auto&[a, nexts] : next_ans) {
    for (auto &b : nexts) {
      std::set<std::pair<int, int>> *next_a_b = pkb.get_next(a, b);
      REQUIRE(next_a_b->size() == 1);
      REQUIRE(next_a_b->find({a, b}) != next_a_b->end());
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

TEST_CASE("PkbNextStar_NestedIf_Correct") {
  ProgramNode *p = BuildProgAst(kNestedIf);
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);
  design_extractor.ExtractDesigns();

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
  std::set<std::pair<int, int>> *next_star_wild_wild = pkb.get_next_star(PKB::kWild, PKB::kWild);
  for (auto&[a, nexts_star] : next_star_ans) {
    for (auto &b : nexts_star) {
      REQUIRE(next_star_wild_wild->find({a, b}) != next_star_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Next*(a,_)
  for (auto&[a, nexts_star] : next_star_ans) {
    std::set<std::pair<int, int>> *next_star_a_wild = pkb.get_next_star(a, PKB::kWild);
    REQUIRE(next_star_ans[a].size() == next_star_a_wild->size());
    for (auto &b : nexts_star) {
      REQUIRE(next_star_a_wild->find({a, b}) != next_star_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Next*(a,b)
  for (auto&[a, nexts_star] : next_star_ans) {
    for (auto &b : nexts_star) {
      std::set<std::pair<int, int>> *next_star_a_b = pkb.get_next_star(a, b);
      REQUIRE(next_star_a_b->size() == 1);
      REQUIRE(next_star_a_b->find({a, b}) != next_star_a_b->end());
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

TEST_CASE("PkbAffects_AdvancedSample_Correct") {
  PKB pkb = InitialisePKB(kAdvancedSample);

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
  for (auto&[a, affects] : affects_ans) {
    std::set<std::pair<int, int>> *affects_a_wild = pkb.get_affects(a, PKB::kWild);
    REQUIRE(affects_ans[a].size() == affects_a_wild->size());
    for (auto &b : affects) {
      REQUIRE(affects_a_wild->find({a, b}) != affects_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Affects(a,b)
  for (auto&[a, affects] : affects_ans) {
    for (auto &b : affects) {
      std::set<std::pair<int, int>> *affects_a_b = pkb.get_affects(a, b);
      REQUIRE(affects_a_b->size() == 1);
      REQUIRE(affects_a_b->find({a, b}) != affects_a_b->end());
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

TEST_CASE("PkbAffectsStar_AdvancedSample_Correct") {
  PKB pkb = InitialisePKB(kAdvancedSample);

  std::map<int, std::vector<int>> affects_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {7, 11, 13, 14, 15}}, {5, {9, 13, 14, 15}}, {6, {}},
      {7, {7, 11, 13, 14, 15}}, {8, {}}, {9, {9, 13, 14, 15}}, {10, {}}, {11, {13, 14, 15}},
      {12, {13, 14, 15}}, {13, {14, 15}}, {14, {15}}, {15, {}}, {16, {17}}, {17, {}},
      {18, {}}, {19, {}}, {20, {}}, {21, {}}, {22, {}}, {23, {}},
      };
  // Check Affects*(_,_)
  std::set<std::pair<int, int>> *affects_star_wild_wild =
      pkb.get_affects_star(PKB::kWild, PKB::kWild);
  for (auto&[a, affects_star] : affects_star_ans) {
    for (auto &b : affects_star) {
      REQUIRE(affects_star_wild_wild->find({a, b}) != affects_star_wild_wild->end());
    }
  }
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  // Check Affects*(a,_)
  for (auto&[a, affects_star] : affects_star_ans) {
    std::set<std::pair<int, int>> *affects_star_a_wild = pkb.get_affects_star(a, PKB::kWild);
    REQUIRE(affects_star_ans[a].size() == affects_star_a_wild->size());
    for (auto &b : affects_star) {
      REQUIRE(affects_star_a_wild->find({a, b}) != affects_star_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Affects*(a,b)
  for (auto&[a, affects_star] : affects_star_ans) {
    for (auto &b : affects_star) {
      std::set<std::pair<int, int>> *affects_star_a_b = pkb.get_affects_star(a, b);
      REQUIRE(affects_star_a_b->size() == 1);
      REQUIRE(affects_star_a_b->find({a, b}) != affects_star_a_b->end());
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

TEST_CASE("PkbAffects_NestedWhileIf_Correct") {
  PKB pkb = InitialisePKB(kNestedWhileIf);

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
  for (auto&[a, affects] : affects_ans) {
    std::set<std::pair<int, int>> *affects_a_wild = pkb.get_affects(a, PKB::kWild);
    REQUIRE(affects_ans[a].size() == affects_a_wild->size());
    for (auto &b : affects) {
      REQUIRE(affects_a_wild->find({a, b}) != affects_a_wild->end());
    }
    pkb.ClearNextAffectsCache();
    REQUIRE(pkb.NextAffectsCacheIsEmpty());
  }

  // Check Affects(a,b)
  for (auto&[a, affects] : affects_ans) {
    for (auto &b : affects) {
      std::set<std::pair<int, int>> *affects_a_b = pkb.get_affects(a, b);
      REQUIRE(affects_a_b->size() == 1);
      REQUIRE(affects_a_b->find({a, b}) != affects_a_b->end());
      pkb.ClearNextAffectsCache();
      REQUIRE(pkb.NextAffectsCacheIsEmpty());
    }
  }
}
