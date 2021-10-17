#include <queue>
#include "pkb.h"

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

std::set<std::pair<int, int>> *PKB::get_next(int a, int b) {
  if (next_cache.find(a) != next_cache.end() && next_cache[a].find(b) != next_cache[a].end()) {
    return &next_cache[a][b];
  }
  int n = stmt_table_.get_num_statements() + 1;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) {
    next_cache[a][b] = std::set<std::pair<int, int>>();
    return &next_cache[a][b];
  }
  if (a == kWild && b == kWild) {
    for (auto&[u, al_u] : cfg_al_) {
      for (auto &v : al_u) {
        next_cache[kWild][kWild].insert({u, v});
        next_cache[kWild][v].insert({u, v});
        next_cache[u][kWild].insert({u, v});
        next_cache[u][v].insert({u, v});
      }
    }
  } else if (a == kWild && b != kWild) {
    for (auto &v : reverse_cfg_al_[b]) {
      next_cache[kWild][b].insert({v, b});
      next_cache[v][b].insert({v, b});
    }
  } else if (a != kWild && b == kWild) {
    for (auto &v : cfg_al_[a]) {
      next_cache[a][kWild].insert({a, v});
      next_cache[a][v].insert({a, v});
    }
  } else {
    if (cfg_al_[a].find(b) != cfg_al_[a].end()) {
      next_cache[a][b].insert({a, b});
    }
  }
  return &next_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_next_star(int a, int b) {
  if (next_star_cache.find(a) != next_star_cache.end()
      && next_star_cache[a].find(b) != next_star_cache[a].end()) {
    return &next_star_cache[a][b];
  }
  int n = stmt_table_.get_num_statements() + 1;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) {
    next_star_cache[a][b] = std::set<std::pair<int, int>>();
    return &next_star_cache[a][b];
  }

  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    for (int i = 0; i < n; i++) {
      ReachabilityDFS(i, i, d, cfg_al_);
    }
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) {
          next_star_cache[kWild][kWild].insert({i, j});
          next_star_cache[i][kWild].insert({i, j});
          next_star_cache[kWild][j].insert({i, j});
          next_star_cache[i][j].insert({i, j});
        }
      }
    }
  } else if (a == kWild && b != kWild) {
    ReachabilityDFS(b, b, d, reverse_cfg_al_);
    for (int i = 0; i < n; i++) {
      // Be careful about the check, d[i][j] means that i can reach j!
      if (d[b][i] != 0) {
        next_star_cache[kWild][b].insert({i, b});
        next_star_cache[i][b].insert({i, b});
      }
    }
  } else if (a != kWild && b == kWild) {
    ReachabilityDFS(a, a, d, cfg_al_);
    for (int j = 0; j < n; j++) {
      if (d[a][j] != 0) {
        next_star_cache[a][kWild].insert({a, j});
        next_star_cache[a][j].insert({a, j});
      }
    }
  } else {
    ReachabilityDFS(a, a, d, cfg_al_);
    for (int j = 0; j < n; j++) {
      if (d[a][j] != 0) {
        next_star_cache[a][kWild].insert({a, j});
        next_star_cache[a][j].insert({a, j});
      }
    }
  }
  return &next_star_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_affects(int a, int b) {
  if (affects_cache.find(a) != affects_cache.end()
      && affects_cache[a].find(b) != affects_cache[a].end()) {
    return &affects_cache[a][b];
  }
  int n = stmt_table_.get_num_statements() + 1;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_cache[a][b] = std::set<std::pair<int, int>>();
    return &affects_cache[a][b];
  }
  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    for (auto &stmt : stmt_table_.get_statements(NodeType::Assign)) {
      if (stmt->get_modifies()->size() != 1) continue;
      std::vector<bool> visited(n, false);
      std::string var_name = *(stmt->get_modifies()->begin());
      bool found = false;
      AffectsDFS(stmt->get_stmt_no(), kWild, stmt->get_stmt_no(), var_name, visited, d, found);
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) {
          affects_cache[kWild][kWild].insert({i, j});
          affects_cache[i][kWild].insert({i, j});
          affects_cache[kWild][j].insert({i, j});
          affects_cache[i][j].insert({i, j});
        }
      }
    }
  } else if (a == kWild && b != kWild) {
    Statement *stmt = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) {
      affects_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_cache[a][b];
    }

    std::vector<bool> stmt_checked(n, false);
    for (auto &var_used : *(stmt->get_uses())) {
      for (auto &stmt_using : *(var_table_.get_variable(var_used)->get_stmts_modifying())) {
        if (!stmt_checked[stmt_using]) {
          stmt_checked[stmt_using] = true;
          if (stmt_table_.get_statement(stmt_using)->get_kind() != NodeType::Assign) continue;
          std::vector<bool> visited(n, false);
          bool found = false;
          AffectsDFS(stmt_using, b, stmt_using, var_used, visited, d, found);
        }
      }
    }

    for (int i = 0; i < n; i++) {
      // Be careful about the check, d[i][j] means that i can reach j!
      if (d[i][b] != 0) {
        affects_cache[kWild][b].insert({i, b});
        affects_cache[i][b].insert({i, b});
      }
    }
  } else if (a != kWild && b == kWild) {
    Statement *stmt = stmt_table_.get_statement(a);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign
        || stmt->get_modifies()->size() != 1) {
      affects_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_cache[a][b];
    }
    std::vector<bool> visited(n, false);
    bool found = false;
    std::string var_name = *(stmt->get_modifies()->begin());
    AffectsDFS(stmt->get_stmt_no(), kWild, stmt->get_stmt_no(), var_name, visited, d, found);

    for (int j = 0; j < n; j++) {
      if (d[a][j] != 0) {
        affects_cache[a][kWild].insert({a, j});
        affects_cache[a][j].insert({a, j});
      }
    }
  } else {
    Statement *stmt = stmt_table_.get_statement(a);
    Statement *stmt2 = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign
        || stmt->get_modifies()->size() != 1
        || stmt2 == nullptr || stmt2->get_kind() != NodeType::Assign) {
      affects_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_cache[a][b];
    }
    std::vector<bool> visited(n, false);
    bool found = false;
    std::string var_name = *(stmt->get_modifies()->begin());
    AffectsDFS(stmt->get_stmt_no(), b, stmt->get_stmt_no(), var_name, visited, d, found);

    if (d[a][b] != 0) {
      affects_cache[a][b].insert({a, b});
    }
  }
  return &affects_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_affects_star(int a, int b) {
  if (affects_star_cache.find(a) != affects_star_cache.end()
      && affects_star_cache[a].find(b) != affects_star_cache[a].end()) {
    return &affects_star_cache[a][b];
  }
  int n = stmt_table_.get_num_statements() + 1;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_star_cache[a][b] = std::set<std::pair<int, int>>();
    return &affects_star_cache[a][b];
  }

  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    std::map<int, std::set<int>> affects_al;
    for (auto&[a, b] : *get_affects(kWild, kWild)) {
      affects_al[a].insert(b);
    }
    for (auto&[u, al_u] : affects_al) {
      ReachabilityDFS(u, u, d, affects_al);
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) {
          affects_star_cache[kWild][kWild].insert({i, j});
          affects_star_cache[i][kWild].insert({i, j});
          affects_star_cache[kWild][j].insert({i, j});
          affects_star_cache[i][j].insert({i, j});
        }
      }
    }
  } else if (a == kWild && b != kWild) {
    Statement *stmt = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) {
      affects_star_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_star_cache[a][b];
    }
    std::vector<bool> visited(n, false);
    AffectsStarBFS(b, kWild, visited, false);
  } else if (a != kWild && b == kWild) {
    Statement *stmt = stmt_table_.get_statement(a);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) {
      affects_star_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_star_cache[a][b];
    }
    std::vector<bool> visited(n, false);
    AffectsStarBFS(a, 0, visited, true);
  } else {
    Statement *stmt = stmt_table_.get_statement(a);
    Statement *stmt2 = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign
        || stmt2 == nullptr || stmt2->get_kind() != NodeType::Assign) {
      affects_star_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_star_cache[a][b];
    }
    std::vector<bool> visited(n, false);
    AffectsStarBFS(a, b, visited, true);
  }
  return &affects_star_cache[a][b];
}

void PKB::ReachabilityDFS(int start, int u, std::vector<std::vector<int>> &d,
                          std::map<int, std::set<int>> &al) {
  for (auto &v : al[u]) {
    if (d[start][v] == 0) {
      d[start][v] = 1;
      ReachabilityDFS(start, v, d, al);
    }
  }
}

void PKB::AffectsDFS(int start, int target, int u, std::string var_name,
                     std::vector<bool> &visited, std::vector<std::vector<int>> &d, bool &found) {
  if (found) return;
  Statement *stmt = stmt_table_.get_statement(start);
  for (auto &v : cfg_al_[u]) {
    Statement *stmt_v = stmt_table_.get_statement(v);
    if (visited[v]) continue;
    visited[v] = true;
    std::set<std::string> *uses = stmt_v->get_uses();
    if (stmt_v->get_kind() == NodeType::Assign && uses->find(var_name) != uses->end()) {
      d[start][v] = 1;
      if (v == target) {
        found = true;
        return;
      }
    }
    std::set<std::string> *modifies = stmt_v->get_modifies();
    if (stmt_v->get_kind() == NodeType::Assign || stmt_v->get_kind() == NodeType::Read
        || stmt_v->get_kind() == NodeType::Call) {
      if (modifies->find(var_name) != modifies->end()) continue;
    }
    AffectsDFS(start, target, v, var_name, visited, d, found);
  }
}

void PKB::AffectsStarBFS(int start, int target, std::vector<bool> &visited, bool forward_relation) {
  std::queue<int> q;
  visited[start] = true;
  q.push(start);
  while (q.size()) {
    int u = q.front();
    q.pop();
    if (forward_relation) {
      for (auto&[a, b] : *get_affects(u, kWild)) {
        if (target == kWild) {
          affects_star_cache[start][kWild].insert({start, b});
          affects_star_cache[start][b].insert({start, b});
        } else {
          if (b == target) {
            affects_star_cache[start][b].insert({start, target});
            return;
          }
        }

        if (!visited[b]) {
          visited[b] = true;
          q.push(b);
        }
      }
    } else {
      for (auto&[a, b] : *get_affects(kWild, u)) {
        if (target == kWild) {
          affects_star_cache[kWild][start].insert({a, start});
          affects_star_cache[a][start].insert({a, start});
        } else {
          if (a == target) {
            affects_star_cache[start][target].insert({start, target});
            return;
          }
        }

        if (!visited[a]) {
          visited[a] = true;
          q.push(a);
        }
      }
    }
  }
}
