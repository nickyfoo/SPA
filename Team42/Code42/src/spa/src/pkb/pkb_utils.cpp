#include <pkb.h>

void PKB::PrintStatements() {
  stmt_table_.PrintStatements();
}

void PKB::PrintProcedures() {
  proc_table_.PrintProcedures();
}

void PKB::PrintVariables() {
  var_table_.PrintVariableDetails();
}

void PKB::PrintCFGAL() {
  for (auto &[u, al] : cfg_al_) {
    std::cout << u << "->";
    for (auto &v : al) {
      std::cout << v << ' ';
    }
    std::cout << '\n';
  }
}

void PKB::PrintCFGBipAL() {
  for (auto&[u, al] : cfg_bip_al_) {
    std::cout << u << "->";
    for (auto&[v, b] : al) {
      std::cout << v << ',' << b << ' ';
    }
    std::cout << '\n';
  }
}

void PKB::ClearNextAffectsCache() {
  next_cache.clear();
  next_star_cache.clear();
  affects_cache.clear();
  affects_star_cache.clear();
}

bool PKB::NextAffectsCacheIsEmpty() {
  return next_cache.empty() && next_star_cache.empty()
  && affects_cache.empty() && affects_star_cache.empty();
}

void PKB::ClearNextAffectsBipCache() {
  next_bip_cache.clear();
  next_bip_star_cache.clear();
  bip_reachability_dfs_cache.clear();
  affects_bip_cache.clear();
  affects_bip_dfs_cache.clear();
  affects_bip_star_cache.clear();
  affects_bip_star_dfs_cache.clear();
}

bool PKB::NextAffectsBipCacheIsEmpty() {
  return next_bip_cache.empty() && next_bip_star_cache.empty()
  && bip_reachability_dfs_cache.empty()
  && affects_bip_cache.empty() && affects_bip_dfs_cache.empty()
  && affects_bip_star_cache.empty() && affects_bip_star_dfs_cache.empty();
}

std::string PKB::CallStackToString(std::vector<int> *call_stack) {
  std::string ans;
  for (auto &x : *call_stack) {
    ans += std::to_string(x);
    ans += ' ';
  }

  return ans;
}