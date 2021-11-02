#include "catch.hpp"
#include <chrono>

#include <pkb.h>
#include "../test_utils.hpp"
#include "entities/statement.h"

TEST_CASE("PkbNext_CacheTestTime_AcceptableTiming") {
  PKB pkb = InitialisePKB(kSampleSource);

  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next(i, j);
    }
    pkb.get_next(i, PKB::kWild);
    pkb.get_next(PKB::kWild, i);
  }
  pkb.get_next(PKB::kWild, PKB::kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  pkb.get_next(PKB::kWild, PKB::kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next(i, j);
    }
    pkb.get_next(i, PKB::kWild);
    pkb.get_next(PKB::kWild, i);
  }
  pkb.get_next(PKB::kWild, PKB::kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "Next empty cache elapsed time: ";
  std::cout << empty_cache_elapsed_seconds.count() * 1000 << "ms\n";
  std::cout << "Next full cache elapsed time: ";
  std::cout << full_cache_elapsed_seconds.count() * 1000 << "ms\n";
}

TEST_CASE("Pkb_NextStarCacheTestTime_AcceptableTiming") {
  PKB pkb = InitialisePKB(kSampleSource);

  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next_star(i, j);
    }
    pkb.get_next_star(i, PKB::kWild);
    pkb.get_next_star(PKB::kWild, i);
  }
  pkb.get_next_star(PKB::kWild, PKB::kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  pkb.get_next_star(PKB::kWild, PKB::kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next_star(i, j);
    }
    pkb.get_next_star(i, PKB::kWild);
    pkb.get_next_star(PKB::kWild, i);
  }
  pkb.get_next_star(PKB::kWild, PKB::kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "Next* empty cache elapsed time: ";
  std::cout << empty_cache_elapsed_seconds.count() * 1000 << "ms\n";
  std::cout << "Next* full cache elapsed time: ";
  std::cout << full_cache_elapsed_seconds.count() * 1000 << "ms\n";
}

TEST_CASE("PkbAffects_CacheTestTime_AcceptableTiming") {
  PKB pkb = InitialisePKB(kSampleSource);

  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (auto &a : pkb.get_statements(NodeType::Assign)) {
    for (auto &b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects(a->get_stmt_no(), PKB::kWild);
    pkb.get_affects(PKB::kWild, a->get_stmt_no());
  }
  pkb.get_affects(PKB::kWild, PKB::kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  pkb.get_affects(PKB::kWild, PKB::kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (auto &a : pkb.get_statements(NodeType::Assign)) {
    for (auto &b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects(a->get_stmt_no(), PKB::kWild);
    pkb.get_affects(PKB::kWild, a->get_stmt_no());
  }
  pkb.get_affects(PKB::kWild, PKB::kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "Affects empty cache elapsed time: ";
  std::cout << empty_cache_elapsed_seconds.count() * 1000 << "ms\n";
  std::cout << "Affects full cache elapsed time: ";
  std::cout << full_cache_elapsed_seconds.count() * 1000 << "ms\n";
}

TEST_CASE("PkbAffectsStar_CacheTestTime_AcceptableTiming") {
  PKB pkb = InitialisePKB(kSampleSource);

  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (auto &a : pkb.get_statements(NodeType::Assign)) {
    for (auto &b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects_star(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects_star(a->get_stmt_no(), PKB::kWild);
    pkb.get_affects_star(PKB::kWild, a->get_stmt_no());
  }
  pkb.get_affects_star(PKB::kWild, PKB::kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());

  pkb.get_affects_star(PKB::kWild, PKB::kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (auto &a : pkb.get_statements(NodeType::Assign)) {
    for (auto &b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects_star(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects_star(a->get_stmt_no(), PKB::kWild);
    pkb.get_affects_star(PKB::kWild, a->get_stmt_no());
  }
  pkb.get_affects_star(PKB::kWild, PKB::kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsCache();
  REQUIRE(pkb.NextAffectsCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "Affects* empty cache elapsed time: ";
  std::cout << empty_cache_elapsed_seconds.count() * 1000 << "ms\n";
  std::cout << "Affects* full cache elapsed time: ";
  std::cout << full_cache_elapsed_seconds.count() * 1000 << "ms\n";
}

TEST_CASE("PkbNextBip_CacheTestTime_AcceptableTiming") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next_bip(i, j);
    }
    pkb.get_next_bip(i, PKB::kWild);
    pkb.get_next_bip(PKB::kWild, i);
  }
  pkb.get_next_bip(PKB::kWild, PKB::kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  pkb.get_next_bip(PKB::kWild, PKB::kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next_bip(i, j);
    }
    pkb.get_next_bip(i, PKB::kWild);
    pkb.get_next_bip(PKB::kWild, i);
  }
  pkb.get_next_bip(PKB::kWild, PKB::kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "NextBip empty cache elapsed time: ";
  std::cout << empty_cache_elapsed_seconds.count() * 1000 << "ms\n";
  std::cout << "NextBip full cache elapsed time: ";
  std::cout << full_cache_elapsed_seconds.count() * 1000 << "ms\n";
}

TEST_CASE("PkbNextBipStar_CacheTestTime_AcceptableTiming") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next_bip_star(i, j);
    }
    pkb.get_next_bip_star(i, PKB::kWild);
    pkb.get_next_bip_star(PKB::kWild, i);
  }
  pkb.get_next_bip_star(PKB::kWild, PKB::kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  pkb.get_next_bip_star(PKB::kWild, PKB::kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (int i = 0; i < pkb.get_num_statements(); i++) {
    for (int j = 0; j < pkb.get_num_statements(); j++) {
      pkb.get_next_bip_star(i, j);
    }
    pkb.get_next_bip_star(i, PKB::kWild);
    pkb.get_next_bip_star(PKB::kWild, i);
  }
  pkb.get_next_bip_star(PKB::kWild, PKB::kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "NextBip* empty cache elapsed time: ";
  std::cout << empty_cache_elapsed_seconds.count() * 1000 << "ms\n";
  std::cout << "NextBip* full cache elapsed time: ";
  std::cout << full_cache_elapsed_seconds.count() * 1000 << "ms\n";
}

TEST_CASE("PkbAffectsBip_CacheTestTime_AcceptableTiming") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (auto &a : pkb.get_statements(NodeType::Assign)) {
    for (auto &b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects_bip(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects_bip(a->get_stmt_no(), PKB::kWild);
    pkb.get_affects_bip(PKB::kWild, a->get_stmt_no());
  }
  pkb.get_affects_bip(PKB::kWild, PKB::kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  pkb.get_affects_bip(PKB::kWild, PKB::kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (auto &a : pkb.get_statements(NodeType::Assign)) {
    for (auto &b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects_bip(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects_bip(a->get_stmt_no(), PKB::kWild);
    pkb.get_affects_bip(PKB::kWild, a->get_stmt_no());
  }
  pkb.get_affects_bip(PKB::kWild, PKB::kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "AffectsBip empty cache elapsed time: ";
  std::cout << empty_cache_elapsed_seconds.count() * 1000 << "ms\n";
  std::cout << "AffectsBip full cache elapsed time: ";
  std::cout << full_cache_elapsed_seconds.count() * 1000 << "ms\n";
}

TEST_CASE("PkbAffectsBipStar_CacheTestTime_AcceptableTiming") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  auto empty_cache_start = std::chrono::steady_clock::now();
  for (auto &a : pkb.get_statements(NodeType::Assign)) {
    for (auto &b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects_bip_star(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects_bip_star(a->get_stmt_no(), PKB::kWild);
    pkb.get_affects_bip_star(PKB::kWild, a->get_stmt_no());
  }
  pkb.get_affects_bip_star(PKB::kWild, PKB::kWild);
  auto empty_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());

  pkb.get_affects_bip_star(PKB::kWild, PKB::kWild);
  auto full_cache_start = std::chrono::steady_clock::now();
  for (auto &a : pkb.get_statements(NodeType::Assign)) {
    for (auto &b : pkb.get_statements(NodeType::Assign)) {
      pkb.get_affects_bip_star(a->get_stmt_no(), b->get_stmt_no());
    }
    pkb.get_affects_bip_star(a->get_stmt_no(), PKB::kWild);
    pkb.get_affects_bip_star(PKB::kWild, a->get_stmt_no());
  }
  pkb.get_affects_star(PKB::kWild, PKB::kWild);
  auto full_cache_end = std::chrono::steady_clock::now();
  pkb.ClearNextAffectsBipCache();
  REQUIRE(pkb.NextAffectsBipCacheIsEmpty());
  std::chrono::duration<double> empty_cache_elapsed_seconds = empty_cache_end - empty_cache_start;
  std::chrono::duration<double> full_cache_elapsed_seconds = full_cache_end - full_cache_start;
  std::cout << "AffectsBip* empty cache elapsed time: ";
  std::cout << empty_cache_elapsed_seconds.count() * 1000 << "ms\n";
  std::cout << "AffectsBip* full cache elapsed time: ";
  std::cout << full_cache_elapsed_seconds.count() * 1000 << "ms\n";
}
