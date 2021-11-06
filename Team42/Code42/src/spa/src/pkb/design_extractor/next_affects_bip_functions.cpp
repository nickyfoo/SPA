#include <pkb.h>
#include <queue>
#include <set>
#include <stack>

std::set<std::pair<int, int>> *PKB::get_next_bip(int a, int b) {
  // Return cached results if found
  if (next_bip_cache.find(a) != next_bip_cache.end()
      && next_bip_cache[a].find(b) != next_bip_cache[a].end()) {
    return &next_bip_cache[a][b];
  }

  int n = stmt_table_.get_num_statements() + 1;
  // If a or b are invalid line numbers, cache and return empty result
  if (a < 0 || a >= n || b < 0 || b >= n) {
    next_bip_cache[a][b] = std::set<std::pair<int, int>>();
    return &next_bip_cache[a][b];
  }

  if (a == kWild) {
    // Get result for NextBip(_,_)
    next_bip_cache[kWild][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      next_bip_cache[i][kWild] = std::set<std::pair<int, int>>();
      next_bip_cache[kWild][i] = std::set<std::pair<int, int>>();
      for (int j = 0; j < n; j++) {
        next_bip_cache[i][j] = std::set<std::pair<int, int>>();
      }
    }

    for (auto &[u, al_u] : cfg_bip_al_) {
      if (u < 0) continue;
      std::set<int> visited, ans;
      GetFirstPositiveStmts(u, visited, ans);
      for (auto &v:ans) {
        next_bip_cache[kWild][kWild].insert({u, v});
        next_bip_cache[kWild][v].insert({u, v});
        next_bip_cache[u][kWild].insert({u, v});
        next_bip_cache[u][v].insert({u, v});
      }
    }
  } else if (a != kWild) {
    // Get result for NextBip(a,_)
    next_bip_cache[a][kWild] = std::set<std::pair<int, int>>();

    for (int i = 0; i < n; i++) {
      next_bip_cache[a][i] = std::set<std::pair<int, int>>();
    }

    std::set<int> visited, ans;
    GetFirstPositiveStmts(a, visited, ans);
    for (auto &v:ans) {
      next_bip_cache[a][kWild].insert({a, v});
      next_bip_cache[a][v].insert({a, v});
    }
  }
  return &next_bip_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_next_bip_star(int a, int b) {
  // Return cached results if found
  if (next_bip_star_cache.find(a) != next_bip_star_cache.end()
      && next_bip_star_cache[a].find(b) != next_bip_star_cache[a].end()) {
    return &next_bip_star_cache[a][b];
  }

  int n = stmt_table_.get_num_statements() + 1;
  // If a or b are invalid line numbers, cache empty result and return
  if (a < 0 || a >= n || b < 0 || b >= n) {
    next_bip_star_cache[a][b] = std::set<std::pair<int, int>>();
    return &next_bip_star_cache[a][b];
  }

  std::string hash;
  if (a == kWild) {
    // Get result for Next(_,b)/Next(_,_)
    next_bip_star_cache[kWild][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      next_bip_star_cache[i][kWild] = std::set<std::pair<int, int>>();
      next_bip_star_cache[kWild][i] = std::set<std::pair<int, int>>();
      for (int j = 0; j < n; j++) {
        next_bip_star_cache[i][j] = std::set<std::pair<int, int>>();
      }
    }

    for (auto &stmt : stmt_table_.get_all_statements()) {
      int stmt_no = stmt->get_stmt_no();
      for (auto call_stack : *stmt->get_call_stacks()) {
        hash = CallStackToString(&call_stack);
        std::string start_hash = hash;
        if (bip_reachability_dfs_cache.find(stmt_no) == bip_reachability_dfs_cache.end()
            || bip_reachability_dfs_cache[stmt_no].find(hash) == bip_reachability_dfs_cache[stmt_no].end()) {
          std::set<std::pair<int, std::string>> visited;
          BipReachabilityDFS(visited, stmt_no, hash, stmt_no, start_hash, call_stack);
        }

        for (auto &reached_stmt_no : bip_reachability_dfs_cache[stmt_no][hash]) {
          if (reached_stmt_no < 0) continue;
          next_bip_star_cache[kWild][kWild].insert({stmt_no, reached_stmt_no});
          next_bip_star_cache[stmt_no][kWild].insert({stmt_no, reached_stmt_no});
          next_bip_star_cache[kWild][reached_stmt_no].insert({stmt_no, reached_stmt_no});
          next_bip_star_cache[stmt_no][reached_stmt_no].insert({stmt_no, reached_stmt_no});
        }
      }
    }
  } else { // a != kWild
    // Get result for Next(a,b)/Next(a,_)
    next_bip_star_cache[a][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      next_bip_star_cache[a][i] = std::set<std::pair<int, int>>();
    }

    for (auto call_stack : *stmt_table_.get_statement(a)->get_call_stacks()) {
      hash = CallStackToString(&call_stack);
      std::string start_hash = hash;
      if (bip_reachability_dfs_cache.find(a) == bip_reachability_dfs_cache.end()
          || bip_reachability_dfs_cache[a].find(hash) == bip_reachability_dfs_cache[a].end()) {
        std::set<std::pair<int, std::string>> visited;
        BipReachabilityDFS(visited, a, hash, a, start_hash, call_stack);
      }

      for (auto &reached_stmt_no : bip_reachability_dfs_cache[a][hash]) {
        if (reached_stmt_no < 0) continue;
        next_bip_star_cache[a][kWild].insert({a, reached_stmt_no});
        next_bip_star_cache[a][reached_stmt_no].insert({a, reached_stmt_no});
      }
    }
  }
  return &next_bip_star_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_affects_bip(int a, int b) {
  // Return cached results if found
  if (affects_bip_cache.find(a) != affects_bip_cache.end()
      && affects_bip_cache[a].find(b) != affects_bip_cache[a].end()) {
    return &affects_bip_cache[a][b];
  }

  int n = stmt_table_.get_num_statements() + 1;
  // If a or b are invalid line numbers, cache empty result and return
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
    return &affects_bip_cache[a][b];
  }

  std::string hash;
  if (a == kWild) {
    if (b != kWild) {
      Statement *stmt_b = stmt_table_.get_statement(b);
      // If b is an invalid statement, cache and return empty result
      if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
        affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
        return &affects_bip_cache[a][b];
      }
    }

    affects_bip_cache[kWild][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      affects_bip_cache[i][kWild] = std::set<std::pair<int, int>>();
      affects_bip_cache[kWild][i] = std::set<std::pair<int, int>>();
      for (int j = 0; j < n; j++) {
        affects_bip_cache[i][j] = std::set<std::pair<int, int>>();
      }
    }

    for (auto &stmt : stmt_table_.get_statements(NodeType::Assign)) {
      int stmt_no = stmt->get_stmt_no();
      if (stmt->get_modifies()->size() != 1) continue;

      std::string start_hash;
      std::string var_name = *(stmt->get_modifies()->begin());
      for (auto call_stack : *stmt->get_call_stacks()) {
        std::set<std::pair<int, std::string>> visited;
        start_hash = CallStackToString(&call_stack);
        hash = CallStackToString(&call_stack);
        if (affects_bip_dfs_cache.find(stmt_no) == affects_bip_dfs_cache.end()
            || affects_bip_dfs_cache[stmt_no].find(hash) == affects_bip_dfs_cache[stmt_no].end()) {
          AffectsBipDFS(stmt_no, start_hash, stmt_no, call_stack, hash, var_name, visited);
        }

        for (auto &[affected_stmt_no, affected_stmt_hash] : affects_bip_dfs_cache[stmt_no][start_hash]) {
          affects_bip_cache[kWild][kWild].insert({stmt_no, affected_stmt_no});
          affects_bip_cache[stmt_no][kWild].insert({stmt_no, affected_stmt_no});
          affects_bip_cache[kWild][affected_stmt_no].insert({stmt_no, affected_stmt_no});
          affects_bip_cache[stmt_no][affected_stmt_no].insert({stmt_no, affected_stmt_no});
        }
      }
    }
  } else { // a != kWild
    Statement *stmt_a = stmt_table_.get_statement(a);

    // If statement a are invalid statements, cache and return empty results
    if (stmt_a == nullptr || stmt_a->get_kind() != NodeType::Assign
        || stmt_a->get_modifies()->size() != 1) {
      affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_bip_cache[a][b];
    }

    if (b != kWild) {
      Statement *stmt_b = stmt_table_.get_statement(b);

      // If statement b is an invalid statement, cache and return empty results
      if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
        affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
        return &affects_bip_cache[a][b];
      }
    }

    affects_bip_cache[a][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      affects_bip_cache[a][i] = std::set<std::pair<int, int>>();
    }

    std::string start_hash;
    std::string var_name = *(stmt_a->get_modifies()->begin());
    for (auto call_stack : *stmt_a->get_call_stacks()) {
      std::set<std::pair<int, std::string>> visited;
      start_hash = CallStackToString(&call_stack);
      hash = CallStackToString(&call_stack);
      if (affects_bip_dfs_cache.find(a) == affects_bip_dfs_cache.end()
          || affects_bip_dfs_cache[a].find(hash) == affects_bip_dfs_cache[a].end()) {
        AffectsBipDFS(a, start_hash, a, call_stack, hash, var_name, visited);
      }

      for (auto &[affected_stmt_no, affected_stmt_hash] : affects_bip_dfs_cache[a][start_hash]) {
        affects_bip_cache[a][kWild].insert({a, affected_stmt_no});
        affects_bip_cache[a][affected_stmt_no].insert({a, affected_stmt_no});
      }
    }
  }

  return &affects_bip_cache[a][b];
}

std::set<std::pair<int, int>> *PKB::get_affects_bip_star(int a, int b) {
  // Return cached results if found
  if (affects_bip_star_cache.find(a) != affects_bip_star_cache.end()
      && affects_bip_star_cache[a].find(b) != affects_bip_star_cache[a].end()) {
    return &affects_bip_star_cache[a][b];
  }

  int n = stmt_table_.get_num_statements() + 1;
  // If a or b are invalid line numbers, cache and return empty result
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_bip_star_cache[a][b] = std::set<std::pair<int, int>>();
    return &affects_bip_star_cache[a][b];
  }

  std::string hash;
  if (a == kWild) {
    if (b != kWild) {
      Statement *stmt_b = stmt_table_.get_statement(b);

      // If statement b is an invalid statement, cache and return empty results
      if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
        affects_bip_star_cache[a][b] = std::set<std::pair<int, int>>();
        return &affects_bip_star_cache[a][b];
      }
    }

    affects_bip_star_cache[kWild][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      affects_bip_star_cache[i][kWild] = std::set<std::pair<int, int>>();
      affects_bip_star_cache[kWild][i] = std::set<std::pair<int, int>>();
      for (int j = 0; j < n; j++) {
        affects_bip_star_cache[i][j] = std::set<std::pair<int, int>>();
      }
    }

    for (auto &stmt : stmt_table_.get_statements(NodeType::Assign)) {
      int stmt_no = stmt->get_stmt_no();
      for (auto call_stack : *stmt->get_call_stacks()) {
        hash = CallStackToString(&call_stack);
        if (affects_bip_star_dfs_cache.find(stmt_no) == affects_bip_star_dfs_cache.end()
            || affects_bip_star_dfs_cache[stmt_no].find(hash) == affects_bip_star_dfs_cache[stmt_no].end()) {
          std::set<std::pair<int, std::string>> visited;
          AffectsBipStarDFS(visited, stmt_no, hash, stmt_no, hash);
        }
        for (auto&[affected_stmt_no, affected_stmt_hash] : affects_bip_star_dfs_cache[stmt_no][hash]) {
          affects_bip_star_cache[kWild][kWild].insert({stmt_no, affected_stmt_no});
          affects_bip_star_cache[stmt_no][kWild].insert({stmt_no, affected_stmt_no});
          affects_bip_star_cache[kWild][affected_stmt_no].insert({stmt_no, affected_stmt_no});
          affects_bip_star_cache[stmt_no][affected_stmt_no].insert({stmt_no, affected_stmt_no});
        }
      }
    }
  } else { // Case: a != kWild
    Statement *stmt_a = stmt_table_.get_statement(a);

    // If statement a are invalid statements, cache and return empty results
    if (stmt_a == nullptr || stmt_a->get_kind() != NodeType::Assign
        || stmt_a->get_modifies()->size() != 1) {
      affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_bip_cache[a][b];
    }

    if (b != kWild) {
      Statement *stmt_b = stmt_table_.get_statement(b);

      // If statement b is an invalid statement, cache and return empty results
      if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
        affects_bip_star_cache[a][b] = std::set<std::pair<int, int>>();
        return &affects_bip_star_cache[a][b];
      }
    }

    affects_bip_star_cache[a][kWild] = std::set<std::pair<int, int>>();
    for (int i = 0; i < n; i++) {
      affects_bip_star_cache[a][i] = std::set<std::pair<int, int>>();
    }

    int stmt_no = stmt_a->get_stmt_no();
    for (auto call_stack : *stmt_a->get_call_stacks()) {
      hash = CallStackToString(&call_stack);
      if (affects_bip_star_dfs_cache.find(stmt_no) == affects_bip_star_dfs_cache.end()
          || affects_bip_star_dfs_cache[stmt_no].find(hash) == affects_bip_star_dfs_cache[stmt_no].end()) {
        std::set<std::pair<int, std::string>> visited;
        AffectsBipStarDFS(visited, stmt_no, hash, stmt_no, hash);
      }

      for (auto &[affected_stmt_no, affected_stmt_hash] : affects_bip_star_dfs_cache[stmt_no][hash]) {
        affects_bip_star_cache[stmt_no][kWild].insert({stmt_no, affected_stmt_no});
        affects_bip_star_cache[stmt_no][affected_stmt_no].insert({stmt_no, affected_stmt_no});
      }
    }
  }
  return &affects_bip_star_cache[a][b];
}

void PKB::BipReachabilityDFS(std::set<std::pair<int, std::string>> &visited,
                             int u,
                             std::string &u_hash,
                             int start,
                             std::string &start_hash,
                             std::vector<int> &call_stack) {
  if (bip_reachability_dfs_cache.find(u) != bip_reachability_dfs_cache.end()
      && bip_reachability_dfs_cache[u].find(u_hash) != bip_reachability_dfs_cache[u].end()) {
    for (auto &reached_stmt: bip_reachability_dfs_cache[u][u_hash]) {
      bip_reachability_dfs_cache[start][start_hash].insert(reached_stmt);
    }
    return;
  }

  int branch;
  if (call_stack.empty()) {
    branch = kNoBranch;
  } else {
    branch = call_stack.back();
  }

  if (u < 0) {
    for (auto &[v, v_branch] : cfg_bip_al_[u]) {
      if (v_branch < 0) {
        // Call stack not empty and edge goes back
        if (branch != kNoBranch && v_branch == -branch) {
          u_hash.erase(u_hash.size() - 1 - std::to_string(call_stack.back()).size());
          call_stack.pop_back();

          if (visited.find({v, u_hash}) == visited.end()) {
            visited.insert({v, u_hash});
            bip_reachability_dfs_cache[start][start_hash].insert(v);
            BipReachabilityDFS(visited, v, u_hash, start, start_hash, call_stack);
          }

          call_stack.push_back(branch);
          u_hash += std::to_string(branch) + " ";
        }
      } else {
        BipReachabilityDFS(visited, v, u_hash, start, start_hash, call_stack);
        continue;
      }
    }
    return;
  }

  for (auto &[v, v_branch] : cfg_bip_al_[u]) {
    if (v < 0) {
      BipReachabilityDFS(visited, v, u_hash, start, start_hash, call_stack);
      continue;
    }

    // Was called from another procedure, and this edge is a returning edge
    // that does not return to that procedure
    if (branch != kNoBranch && v_branch < 0 && v_branch != -branch) {
      continue;
    }

    // Edge goes to a new procedure
    if (v_branch > 0) {
      call_stack.push_back(v_branch);
      std::string new_call = std::to_string(v_branch) + " ";
      u_hash += new_call;

      if (visited.find({v, u_hash}) == visited.end()) {
        visited.insert({v, u_hash});
        bip_reachability_dfs_cache[start][start_hash].insert(v);
        BipReachabilityDFS(visited, v, u_hash, start, start_hash, call_stack);
      }

      call_stack.pop_back();
      u_hash.erase(u_hash.size() - new_call.size());
    } else if (v_branch < 0) {
      // Call stack not empty and edge goes back
      if (branch != kNoBranch && v_branch == -branch) {
        u_hash.erase(u_hash.size() - 1 - std::to_string(call_stack.back()).size());
        call_stack.pop_back();

        if (visited.find({v, u_hash}) == visited.end()) {
          visited.insert({v, u_hash});
          bip_reachability_dfs_cache[start][start_hash].insert(v);
          BipReachabilityDFS(visited, v, u_hash, start, start_hash, call_stack);
        }

        call_stack.push_back(branch);
        u_hash += std::to_string(branch) + " ";
      }
    } else if (v_branch == 0) {
      if (visited.find({v, u_hash}) == visited.end()) {
        visited.insert({v, u_hash});
        bip_reachability_dfs_cache[start][start_hash].insert(v);
        BipReachabilityDFS(visited, v, u_hash, start, start_hash, call_stack);
      }

    }
  }
}

void PKB::AffectsBipDFS(int start,
                        std::string &start_hash,
                        int u,
                        std::vector<int> &call_stack,
                        std::string &hash,
                        std::string var_name,
                        std::set<std::pair<int, std::string>> &visited) {

  int branch;
  if (call_stack.empty()) {
    branch = kNoBranch;
  } else {
    branch = call_stack.back();
  }

  if (u < 0) {
    for (auto &[v, v_branch] : cfg_bip_al_[u]) {
      if (v_branch < 0) {
        // Call stack not empty and edge goes back
        if (branch != kNoBranch && v_branch == -branch) {
          hash.erase(hash.size() - 1 - std::to_string(call_stack.back()).size());
          call_stack.pop_back();

          if (visited.find({v, hash}) == visited.end()) {
            visited.insert({v, hash});
            if (v > 0) {
              AddStmtIfAffectedBip(start, start_hash, v, hash, var_name);
              if (!ModifiesVarName(v, var_name)) {
                AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
              }
            } else {
              AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
            }
          }

          call_stack.push_back(branch);
          hash += std::to_string(branch) + " ";
        }
      } else {

        AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
        continue;
      }
    }
    return;
  }

  for (auto &[v, v_branch] : cfg_bip_al_[u]) {
    if (v < 0) {
      AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
      continue;
    }

    // Was called from another procedure, and this edge is a returning edge
    // that does not return to that procedure
    if (branch != kNoBranch && v_branch < 0 && v_branch != -branch) {
      continue;
    }

    // Edge goes to a new procedure
    if (v_branch > 0) {
      call_stack.push_back(v_branch);
      std::string new_call = std::to_string(v_branch) + " ";
      hash += new_call;

      if (visited.find({v, hash}) == visited.end()) {
        visited.insert({v, hash});
        AddStmtIfAffectedBip(start, start_hash, v, hash, var_name);
        if (!ModifiesVarName(v, var_name)) {
          AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
        }
      }

      call_stack.pop_back();
      hash.erase(hash.size() - new_call.size());
    } else if (v_branch < 0) {
      // Call stack not empty and edge goes back
      if (branch != kNoBranch && v_branch == -branch) {
        hash.erase(hash.size() - 1 - std::to_string(call_stack.back()).size());
        call_stack.pop_back();

        if (visited.find({v, hash}) == visited.end()) {
          visited.insert({v, hash});
          AddStmtIfAffectedBip(start, start_hash, v, hash, var_name);
          if (!ModifiesVarName(v, var_name)) {
            AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
          }
        }

        call_stack.push_back(branch);
        hash += std::to_string(branch) + " ";
      }
    } else if (v_branch == 0) {
      if (visited.find({v, hash}) != visited.end()) continue;

      visited.insert({v, hash});
      AddStmtIfAffectedBip(start, start_hash, v, hash, var_name);
      if (!ModifiesVarName(v, var_name)) {
        AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
      }
    }
  }
}

void PKB::AffectsBipStarDFS(std::set<std::pair<int, std::string>> &visited,
                            int u,
                            std::string &u_hash,
                            int start,
                            std::string &start_hash) {
  if (affects_bip_star_dfs_cache.find(u) != affects_bip_star_dfs_cache.end()
      && affects_bip_star_dfs_cache[u].find(u_hash) != affects_bip_star_dfs_cache[u].end()) {
    for (auto &val : affects_bip_star_dfs_cache[u][u_hash]) {
      affects_bip_star_dfs_cache[start][start_hash].insert(val);
    }
    return;
  }

  get_affects_bip(u, kWild);
  for (auto[v, v_hash] : affects_bip_dfs_cache[u][u_hash]) {
    affects_bip_star_dfs_cache[start][start_hash].insert({v, v_hash});
    if (visited.find({v, v_hash}) == visited.end()) {
      visited.insert({v, v_hash});
      AffectsBipStarDFS(visited, v, v_hash, start, start_hash);
    }
  }
}

void PKB::AddStmtIfAffectedBip(int start,
                               std::string &start_hash,
                               int v,
                               const std::string &v_hash,
                               std::string &var_name) {
  auto stmt_v = stmt_table_.get_statement(v);
  std::set<std::string> *uses = stmt_v->get_uses();

  if (stmt_v->get_kind() == NodeType::Assign && uses->find(var_name) != uses->end()) {
    affects_bip_dfs_cache[start][start_hash].insert({v, v_hash});
  }
}

bool PKB::ModifiesVarName(int v, const std::string &var_name) {
  Statement *stmt_v = stmt_table_.get_statement(v);
  std::set<std::string> *modifies = stmt_v->get_modifies();

  if (stmt_v->get_kind() == NodeType::Assign || stmt_v->get_kind() == NodeType::Read) {
    if (modifies->find(var_name) != modifies->end()) return true;
  }
  return false;
}

void PKB::GetFirstPositiveStmts(int u, std::set<int> &visited, std::set<int> &ans) {
  if (visited.find(u) != visited.end()) return;
  visited.insert(u);
  for (auto &[v, vbranch] : cfg_bip_al_[u]) {
    if (v < 0) GetFirstPositiveStmts(v, visited, ans);
    else {
      ans.insert(v);
    }
  }
}
