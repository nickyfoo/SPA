#include <pkb.h>
#include <queue>

std::set<std::pair<int, int>> *PKB::get_next(int a, int b) {
  // Return cached results if found
  if (next_cache.find(a) != next_cache.end()
      && next_cache[a].find(b) != next_cache[a].end()) {
    return &next_cache[a][b];
  }

  int n = stmt_table_.get_num_statements() + 1;
  // If a or b are invalid line numbers, cache and return empty result
  if (a < 0 || a >= n || b < 0 || b >= n) {
    next_cache[a][b] = std::set<std::pair<int, int>>();
    return &next_cache[a][b];
  }

  if (a == kWild && b == kWild) {
    // Get result for Next(_,_)
    next_cache[kWild][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      next_cache[i][kWild] = std::set<std::pair<int, int>>();
      next_cache[kWild][i] = std::set<std::pair<int, int>>();
      for (int j = 0; j < n; j++) {
        next_cache[i][j] = std::set<std::pair<int, int>>();
      }
    }
    for (auto&[u, al_u] : cfg_al_) {
      for (auto &v : al_u) {
        next_cache[kWild][kWild].insert({u, v});
        next_cache[kWild][v].insert({u, v});
        next_cache[u][kWild].insert({u, v});
        next_cache[u][v].insert({u, v});
      }
    }
  } else if (a == kWild && b != kWild) {
    // Get result for Next(_,b)
    next_cache[kWild][b] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      next_cache[i][b] = std::set<std::pair<int, int>>();
    }
    for (auto &v : reverse_cfg_al_[b]) {
      next_cache[kWild][b].insert({v, b});
      next_cache[v][b].insert({v, b});
    }
  } else if (a != kWild && b == kWild) {
    // Get result for Next(a,_)
    next_cache[a][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      next_cache[a][i] = std::set<std::pair<int, int>>();
    }
    for (auto &v : cfg_al_[a]) {
      next_cache[a][kWild].insert({a, v});
      next_cache[a][v].insert({a, v});
    }
  } else {
    // Get result for Next(a,b)
    next_cache[a][b] = std::set<std::pair<int, int>>();
    if (cfg_al_[a].find(b) != cfg_al_[a].end()) {
      // If b exists in a's next nodes, add it to the cache
      next_cache[a][b].insert({a, b});
    }
  }
  return &next_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_next_star(int a, int b) {
  // Return cached results if found
  if (next_star_cache.find(a) != next_star_cache.end()
      && next_star_cache[a].find(b) != next_star_cache[a].end()) {
    return &next_star_cache[a][b];
  }

  int n = stmt_table_.get_num_statements() + 1;
  // If a or b are invalid line numbers, cache empty result and return
  if (a < 0 || a >= n || b < 0 || b >= n) {
    next_star_cache[a][b] = std::set<std::pair<int, int>>();
    return &next_star_cache[a][b];
  }

  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    next_star_cache[kWild][kWild] = std::set<std::pair<int, int>>();

    // DFS reachability from each statement
    for (int i = 0; i < n; i++) {
      next_star_cache[kWild][i] = std::set<std::pair<int, int>>();
      next_star_cache[i][kWild] = std::set<std::pair<int, int>>();
      ReachabilityDFS(i, i, d, cfg_al_);
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        next_star_cache[i][j] = std::set<std::pair<int, int>>();
        if (d[i][j] == 0) continue;

        // Cache results if not empty
        next_star_cache[kWild][kWild].insert({i, j});
        next_star_cache[i][kWild].insert({i, j});
        next_star_cache[kWild][j].insert({i, j});
        next_star_cache[i][j].insert({i, j});
      }
    }

  } else if (a == kWild && b != kWild) {
    next_star_cache[kWild][b] = std::set<std::pair<int, int>>();
    ReachabilityDFS(b, b, d, reverse_cfg_al_);
    for (int i = 0; i < n; i++) {
      next_star_cache[i][b] = std::set<std::pair<int, int>>();
      // Be careful about the check, d[i][j] means that i can reach j!
      if (d[b][i] != 0) {
        next_star_cache[kWild][b].insert({i, b});
        next_star_cache[i][b].insert({i, b});
      }
    }
  } else if (a != kWild && b == kWild) {
    next_star_cache[a][kWild] = std::set<std::pair<int, int>>();
    ReachabilityDFS(a, a, d, cfg_al_);
    for (int j = 0; j < n; j++) {
      next_star_cache[a][j] = std::set<std::pair<int, int>>();
      if (d[a][j] != 0) {
        next_star_cache[a][kWild].insert({a, j});
        next_star_cache[a][j].insert({a, j});
      }
    }
  } else {
    next_star_cache[a][kWild] = std::set<std::pair<int, int>>();
    ReachabilityDFS(a, a, d, cfg_al_);
    for (int j = 0; j < n; j++) {
      next_star_cache[a][j] = std::set<std::pair<int, int>>();
      if (d[a][j] != 0) {
        next_star_cache[a][kWild].insert({a, j});
        next_star_cache[a][j].insert({a, j});
      }
    }
  }
  return &next_star_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_affects(int a, int b) {
  // Return cached results if found
  if (affects_cache.find(a) != affects_cache.end()
      && affects_cache[a].find(b) != affects_cache[a].end()) {
    return &affects_cache[a][b];
  }

  int n = stmt_table_.get_num_statements() + 1;
  // If a or b are invalid line numbers, cache empty result and return
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_cache[a][b] = std::set<std::pair<int, int>>();
    return &affects_cache[a][b];
  }

  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    affects_cache[kWild][kWild] = std::set<std::pair<int, int>>();
    for (auto &stmt: stmt_table_.get_all_statements()) {
      int stmt_no = stmt->get_stmt_no();

      affects_cache[kWild][stmt_no] = std::set<std::pair<int, int>>();
      affects_cache[stmt_no][kWild] = std::set<std::pair<int, int>>();

      if (stmt->get_kind() == NodeType::Assign) {
        if (stmt->get_modifies()->size() != 1) continue;
        std::vector<bool> visited(n, false);
        std::string var_name = *(stmt->get_modifies()->begin());
        bool found = false;
        AffectsDFS(stmt_no, stmt_no, var_name, visited, d, found);
      }
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        affects_cache[i][j] = std::set<std::pair<int, int>>();
        if (d[i][j] == 0) continue;

        // Cache results if not empty
        affects_cache[kWild][kWild].insert({i, j});
        affects_cache[i][kWild].insert({i, j});
        affects_cache[kWild][j].insert({i, j});
        affects_cache[i][j].insert({i, j});
      }
    }
  } else if (a == kWild && b != kWild) {
    Statement *stmt_b = stmt_table_.get_statement(b);
    // If statement b is an invalid statement, cache and return empty results
    if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
      affects_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_cache[a][b];
    }

    affects_cache[kWild][b] = std::set<std::pair<int, int>>();

    std::vector<bool> stmt_checked(n, false);
    for (auto &var_used : *(stmt_b->get_uses())) {
      for (auto &stmt_using : *(var_table_.get_variable(var_used)->get_stmts_modifying())) {
        affects_cache[stmt_using][kWild] = std::set<std::pair<int, int>>();
        affects_cache[stmt_using][b] = std::set<std::pair<int, int>>();

        // Check unchecked statement
        if (!stmt_checked[stmt_using]) {
          stmt_checked[stmt_using] = true;
          if (stmt_table_.get_statement(stmt_using)->get_kind() != NodeType::Assign) continue;
          std::vector<bool> visited(n, false);
          bool found = false;
          AffectsDFS(stmt_using, stmt_using, var_used, visited, d, found);
        }
      }
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (stmt_checked[i]) {
          affects_cache[i][j] = std::set<std::pair<int, int>>();
          if (d[i][j] != 0) {
            affects_cache[i][kWild].insert({i, j});
            affects_cache[i][j].insert({i, j});
          }
        }
      }
      // Be careful about the check, d[i][j] means that i can reach j!
      if (d[i][b] != 0) {
        affects_cache[kWild][b].insert({i, b});
        affects_cache[i][b].insert({i, b});
      }
    }
  } else if (a != kWild) {
    affects_cache[a][kWild] = std::set<std::pair<int, int>>();
    Statement *stmt_a = stmt_table_.get_statement(a);
    // If statement a is an invalid statement, cache and return empty results
    if (stmt_a == nullptr || stmt_a->get_kind() != NodeType::Assign
        || stmt_a->get_modifies()->size() != 1) {
      affects_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_cache[a][b];
    }

    std::vector<bool> visited(n, false);
    bool found = false;
    std::string var_name = *(stmt_a->get_modifies()->begin());
    AffectsDFS(stmt_a->get_stmt_no(), stmt_a->get_stmt_no(), var_name, visited, d, found);

    for (int j = 0; j < n; j++) {
      affects_cache[a][j] = std::set<std::pair<int, int>>();
      if (d[a][j] != 0) {
        affects_cache[a][kWild].insert({a, j});
        affects_cache[a][j].insert({a, j});
      }
    }
  }
  return &affects_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_affects_star(int a, int b) {
  // Return cached results if found
  if (affects_star_cache.find(a) != affects_star_cache.end()
      && affects_star_cache[a].find(b) != affects_star_cache[a].end()) {
    return &affects_star_cache[a][b];
  }

  int n = stmt_table_.get_num_statements() + 1;
  // If a or b are invalid line numbers, cache and return empty result
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_star_cache[a][b] = std::set<std::pair<int, int>>();
    return &affects_star_cache[a][b];
  }

  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    affects_star_cache[kWild][kWild] = std::set<std::pair<int, int>>();
    std::map<int, std::set<int>> affects_al;
    for (auto&[s_a, s_b] : *get_affects(kWild, kWild)) {
      affects_al[s_a].insert(s_b);
    }
    for (auto&[u, al_u] : affects_al) {
      affects_star_cache[u][kWild] = std::set<std::pair<int, int>>();
      affects_star_cache[kWild][u] = std::set<std::pair<int, int>>();
      ReachabilityDFS(u, u, d, affects_al);
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        affects_star_cache[i][j] = std::set<std::pair<int, int>>();
        if (d[i][j] == 0) continue;
        affects_star_cache[kWild][kWild].insert({i, j});
        affects_star_cache[i][kWild].insert({i, j});
        affects_star_cache[kWild][j].insert({i, j});
        affects_star_cache[i][j].insert({i, j});
      }
    }
  } else if (a == kWild && b != kWild) {
    Statement *stmt_b = stmt_table_.get_statement(b);
    // If statement b is an invalid statement, cache and return empty results
    if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
      affects_star_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_star_cache[a][b];
    }

    affects_star_cache[kWild][b] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      affects_star_cache[i][b] = std::set<std::pair<int, int>>();
    }
    std::vector<bool> visited(n, false);
    AffectsStarBFS(b, visited, false);
  } else if (a != kWild && b == kWild) {
    Statement *stmt_a = stmt_table_.get_statement(a);

    // If statement a are invalid statements, cache and return empty results
    if (stmt_a == nullptr || stmt_a->get_kind() != NodeType::Assign) {
      affects_star_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_star_cache[a][b];
    }

    affects_star_cache[a][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      affects_star_cache[a][i] = std::set<std::pair<int, int>>();
    }
    std::vector<bool> visited(n, false);
    AffectsStarBFS(a, visited, true);
  } else {
    Statement *stmt_a = stmt_table_.get_statement(a);
    Statement *stmt_b = stmt_table_.get_statement(b);

    // If statement a or b are invalid statements, cache and return empty results
    if (stmt_a == nullptr || stmt_a->get_kind() != NodeType::Assign
        || stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
      affects_star_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_star_cache[a][b];
    }
    std::vector<bool> visited(n, false);
    AffectsStarBFS(a, visited, true);
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

void PKB::AffectsDFS(int start, int u, const std::string &var_name,
                     std::vector<bool> &visited, std::vector<std::vector<int>> &d, bool &found) {
  if (found) return;
  for (auto &v : cfg_al_[u]) {
    Statement *stmt_v = stmt_table_.get_statement(v);
    if (visited[v]) continue;

    // Visit node
    visited[v] = true;
    std::set<std::string> *uses = stmt_v->get_uses();
    if (stmt_v->get_kind() == NodeType::Assign && uses->find(var_name) != uses->end()) {
      d[start][v] = 1;
    }

    std::set<std::string> *modifies = stmt_v->get_modifies();
    if (stmt_v->get_kind() == NodeType::Assign || stmt_v->get_kind() == NodeType::Read
        || stmt_v->get_kind() == NodeType::Call) {
      if (modifies->find(var_name) != modifies->end()) continue;
    }
    AffectsDFS(start, v, var_name, visited, d, found);
  }
}

void PKB::AffectsStarBFS(int start, std::vector<bool> &visited, bool forward_relation) {
  std::queue<int> q;
  visited[start] = true;
  q.push(start);

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    if (forward_relation) {
      for (auto&[a, b] : *get_affects(u, kWild)) {
        affects_star_cache[start][kWild].insert({start, b});
        affects_star_cache[start][b].insert({start, b});

        if (!visited[b]) {
          visited[b] = true;
          q.push(b);
        }
      }
    } else {
      for (auto&[a, b] : *get_affects(kWild, u)) {
        affects_star_cache[kWild][start].insert({a, start});
        affects_star_cache[a][start].insert({a, start});

        if (!visited[a]) {
          visited[a] = true;
          q.push(a);
        }
      }
    }
  }
}
