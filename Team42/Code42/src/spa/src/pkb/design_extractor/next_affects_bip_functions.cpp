#include <queue>
#include <stack>
#include "pkb.h"


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

std::set<std::pair<int, int>>* PKB::get_next_bip(int a, int b) {
  if (next_bip_cache.find(a) != next_bip_cache.end() && next_bip_cache[a].find(b) != next_bip_cache[a].end()) {
    return &next_bip_cache[a][b];
  }
  int n = stmt_table_.get_num_statements() + 1;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
    return &next_bip_cache[a][b];
  }
  if (a == kWild && b == kWild) {
    for (auto& [u, al_u] : cfg_bip_al_) {
      for (auto& [v,branch] : al_u) {
        next_bip_cache[kWild][kWild].insert({ u, v });
        next_bip_cache[kWild][v].insert({ u, v });
        next_bip_cache[u][kWild].insert({ u, v });
        next_bip_cache[u][v].insert({ u, v });
      }
    }
  }
  else if (a == kWild && b != kWild) {
    for (auto& [v, branch] : reverse_cfg_bip_al_[b]) {
      next_bip_cache[kWild][b].insert({ v, b });
      next_bip_cache[v][b].insert({ v, b });
    }
  }
  else if (a != kWild && b == kWild) {
    for (auto& [v, branch] : cfg_bip_al_[a]) {
      next_bip_cache[a][kWild].insert({ a, v });
      next_bip_cache[a][v].insert({ a, v });
    }
  }
  else {
    if (cfg_bip_al_[a].lower_bound({ b,INT_MIN }) != cfg_bip_al_[a].end()) {
      next_bip_cache[a][b].insert({ a, b });
    }
  }
  return &next_bip_cache[a][b];
}

std::set<std::pair<int, int>>* PKB::get_next_bip_star(int a, int b) {
  if (next_bip_star_cache.find(a) != next_bip_star_cache.end()
    && next_bip_star_cache[a].find(b) != next_bip_star_cache[a].end()) {
    return &next_bip_star_cache[a][b];
  }
  int n = stmt_table_.get_num_statements() + 1;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) {
    next_bip_star_cache[a][b] = std::set<std::pair<int, int>>();
    return &next_bip_star_cache[a][b];
  }

  std::set<std::pair<int, std::string>> prev_stmts;
  std::string hash = "";
  if (a == kWild) {
    for (auto& stmt : stmt_table_.get_all_statements()) {
      int stmt_no = stmt->get_stmt_no();
      for (auto call_stack : *stmt->get_call_stacks()) {
        hash = CallStackToString(&call_stack);
        if (bip_reachability_dfs_cache.find(stmt_no) == bip_reachability_dfs_cache.end()
          || bip_reachability_dfs_cache[stmt_no].find(hash) == bip_reachability_dfs_cache[stmt_no].end()) {
          prev_stmts.insert({ stmt_no, hash });
          BipReachabilityDFS(prev_stmts, stmt_no, call_stack, hash);
          prev_stmts.erase({ stmt_no, hash });
        }

        for (auto& reached_stmt_no : bip_reachability_dfs_cache[stmt_no][hash]) {
          next_bip_star_cache[kWild][kWild].insert({ stmt_no, reached_stmt_no });
          next_bip_star_cache[stmt_no][kWild].insert({ stmt_no, reached_stmt_no });
          next_bip_star_cache[kWild][reached_stmt_no].insert({ stmt_no, reached_stmt_no });
          next_bip_star_cache[stmt_no][reached_stmt_no].insert({ stmt_no, reached_stmt_no });
        }
      }
    }
  }
  else if (a != kWild) {
    for (auto call_stack : *stmt_table_.get_statement(a)->get_call_stacks()) {
      hash = CallStackToString(&call_stack);
      if (bip_reachability_dfs_cache.find(a) == bip_reachability_dfs_cache.end()
        || bip_reachability_dfs_cache[a].find(hash) == bip_reachability_dfs_cache[a].end()) {
        prev_stmts.insert({ a, hash });
        BipReachabilityDFS(prev_stmts, a, call_stack, hash);
        prev_stmts.erase({ a, hash });
      }

      for (auto& reached_stmt_no : bip_reachability_dfs_cache[a][hash]) {
        next_bip_star_cache[a][kWild].insert({ a, reached_stmt_no });
        next_bip_star_cache[a][reached_stmt_no].insert({ a, reached_stmt_no });
      }
    }
  }
  return &next_bip_star_cache[a][b];
}


std::set<std::pair<int, int>>* PKB::get_affects_bip(int a, int b) {
  if (affects_bip_cache.find(a) != affects_bip_cache.end()
    && affects_bip_cache[a].find(b) != affects_bip_cache[a].end()) {
    return &affects_bip_cache[a][b];
  }
  int n = stmt_table_.get_num_statements() + 1;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
    return &affects_bip_cache[a][b];
  }

  std::string hash = "";
  if (a == kWild) {
    if (b != kWild) {
      Statement* stmt_b = stmt_table_.get_statement(b);
      // Invalid, not an assign stmt
      if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
        affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
        return &affects_bip_cache[a][b];
      }
    }
    for (auto& stmt : stmt_table_.get_statements(NodeType::Assign)) {
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

        for (auto& [affected_stmt_no, affected_stmt_hash] : affects_bip_dfs_cache[stmt_no][start_hash]) {
          affects_bip_cache[kWild][kWild].insert({ stmt_no, affected_stmt_no });
          affects_bip_cache[stmt_no][kWild].insert({ stmt_no, affected_stmt_no });
          affects_bip_cache[kWild][affected_stmt_no].insert({ stmt_no, affected_stmt_no });
          affects_bip_cache[stmt_no][affected_stmt_no].insert({ stmt_no, affected_stmt_no });
        }
      }
    }
  }
  else if (a != kWild) {
    Statement* stmt = stmt_table_.get_statement(a);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign
      || stmt->get_modifies()->size() != 1) {
      affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_bip_cache[a][b];
    }
    if (b != kWild) {
      Statement* stmt_b = stmt_table_.get_statement(b);
      // Invalid, not an assign stmt
      if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
        affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
        return &affects_bip_cache[a][b];
      }
    }

    std::string start_hash;
    std::string var_name = *(stmt->get_modifies()->begin());
    for (auto call_stack : *stmt->get_call_stacks()) {
      std::set<std::pair<int, std::string>> visited;
      start_hash = CallStackToString(&call_stack);
      hash = CallStackToString(&call_stack);
      if (affects_bip_dfs_cache.find(a) == affects_bip_dfs_cache.end()
        || affects_bip_dfs_cache[a].find(hash) == affects_bip_dfs_cache[a].end()) {
        AffectsBipDFS(a, start_hash, a, call_stack, hash, var_name, visited);
      }

      for (auto& [affected_stmt_no, affected_stmt_hash] : affects_bip_dfs_cache[a][start_hash]) {
        affects_bip_cache[a][kWild].insert({ a, affected_stmt_no });
        affects_bip_cache[a][affected_stmt_no].insert({ a, affected_stmt_no });
      }
    }
  }

  return &affects_bip_cache[a][b];
}

std::set<std::pair<int, int>>* PKB::get_affects_bip_star(int a, int b) {
  if (affects_bip_star_cache.find(a) != affects_bip_star_cache.end()
    && affects_bip_star_cache[a].find(b) != affects_bip_star_cache[a].end()) {
    return &affects_bip_star_cache[a][b];
  }
  int n = stmt_table_.get_num_statements() + 1;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) {
    affects_bip_star_cache[a][b] = std::set<std::pair<int, int>>();
    return &affects_bip_star_cache[a][b];
  }

  std::set<std::pair<int, std::string>> prev_stmts;
  std::string hash = "";
  if (a == kWild) {
    if (b != kWild) {
      Statement* stmt_b = stmt_table_.get_statement(b);
      // Invalid, not an assign stmt
      if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
        affects_bip_star_cache[a][b] = std::set<std::pair<int, int>>();
        return &affects_bip_star_cache[a][b];
      }
    }

    for (auto& stmt : stmt_table_.get_statements(NodeType::Assign)) {
      int stmt_no = stmt->get_stmt_no();
      for (auto call_stack : *stmt->get_call_stacks()) {
        hash = CallStackToString(&call_stack);
        if (affects_bip_star_dfs_cache.find(stmt_no) == affects_bip_star_dfs_cache.end()
          || affects_bip_star_dfs_cache[stmt_no].find(hash) == affects_bip_star_dfs_cache[stmt_no].end()) {
          prev_stmts.insert({ stmt_no, hash });
          AffectsBipStarDFS(prev_stmts, stmt_no, hash);
          prev_stmts.erase({ stmt_no, hash });
        }

        for (auto& [affected_stmt_no, affected_stmt_hash] : affects_bip_star_dfs_cache[stmt_no][hash]) {
          affects_bip_star_cache[kWild][kWild].insert({ stmt_no, affected_stmt_no });
          affects_bip_star_cache[stmt_no][kWild].insert({ stmt_no, affected_stmt_no });
          affects_bip_star_cache[kWild][affected_stmt_no].insert({ stmt_no, affected_stmt_no });
          affects_bip_star_cache[stmt_no][affected_stmt_no].insert({ stmt_no, affected_stmt_no });
        }
      }
    }
  }
  else if (a != kWild) {
    Statement* stmt = stmt_table_.get_statement(a);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign
      || stmt->get_modifies()->size() != 1) {
      affects_bip_cache[a][b] = std::set<std::pair<int, int>>();
      return &affects_bip_cache[a][b];
    }

    if (b != kWild) {
      Statement* stmt_b = stmt_table_.get_statement(b);
      // Invalid, not an assign stmt
      if (stmt_b == nullptr || stmt_b->get_kind() != NodeType::Assign) {
        affects_bip_star_cache[a][b] = std::set<std::pair<int, int>>();
        return &affects_bip_star_cache[a][b];
      }
    }

    int stmt_no = stmt->get_stmt_no();
    for (auto call_stack : *stmt->get_call_stacks()) {
      hash = CallStackToString(&call_stack);
      if (affects_bip_star_dfs_cache.find(stmt_no) == affects_bip_star_dfs_cache.end()
        || affects_bip_star_dfs_cache[stmt_no].find(hash) == affects_bip_star_dfs_cache[stmt_no].end()) {
        prev_stmts.insert({ stmt_no, hash });
        AffectsBipStarDFS(prev_stmts, stmt_no, hash);
        prev_stmts.erase({ stmt_no, hash });
      }

      for (auto& [affected_stmt_no, affected_stmt_hash] : affects_bip_star_dfs_cache[stmt_no][hash]) {
        affects_bip_star_cache[stmt_no][kWild].insert({ stmt_no, affected_stmt_no });
        affects_bip_star_cache[stmt_no][affected_stmt_no].insert({ stmt_no, affected_stmt_no });
      }
    }
  }
  return &affects_bip_star_cache[a][b];
}

void PKB::BipReachabilityDFS(std::set<std::pair<int, std::string>>& prev_stmts, int u, 
  std::vector<int>& call_stack, std::string& hash) {
  if (bip_reachability_dfs_cache.find(u) != bip_reachability_dfs_cache.end() && bip_reachability_dfs_cache[u].find(hash) != bip_reachability_dfs_cache[u].end()) {
    for (auto& reached_stmt: bip_reachability_dfs_cache[u][hash]) {
      for (auto& [prev_stmt, prev_hash] : prev_stmts) {
        bip_reachability_dfs_cache[prev_stmt][prev_hash].insert(reached_stmt);
      }
    }
    return;
  }
  int branch;
  if (call_stack.size() == 0) {
    branch = kNoBranch;
  }
  else {
    branch = call_stack.back();
  }
  for (auto& [v, v_branch] : cfg_bip_al_[u]) {
    // Was called from another procedure, and this edge is a returning edge that does not return to that procedure
    if (branch != kNoBranch && v_branch < 0 && v_branch != -branch) {
      continue;
    }
    // Edge goes to a new procedure
    if (v_branch > 0) {
      call_stack.push_back(v_branch);
      std::string new_call = std::to_string(v_branch) + " ";
      hash += new_call;
      for (auto& [prev_stmt, prev_hash] : prev_stmts) {
        bip_reachability_dfs_cache[prev_stmt][prev_hash].insert(v);
      }
      bool visited_before = prev_stmts.find({ v, hash }) != prev_stmts.end();
      prev_stmts.insert({ v, hash });
      BipReachabilityDFS(prev_stmts,v,call_stack,hash);
      if (!visited_before) prev_stmts.erase({ v, hash });
      call_stack.pop_back();
      hash.erase(hash.size() - new_call.size());
    }
    else if (v_branch < 0) {
      // Call stack not empty and edge goes back
      if (branch != kNoBranch) {
        // Edge goes back
        if (v_branch == -branch) {
          hash.erase(hash.size() - 1 - std::to_string(call_stack.back()).size());
          call_stack.pop_back();
          for (auto& [prev_stmt, prev_hash] : prev_stmts) {
            bip_reachability_dfs_cache[prev_stmt][prev_hash].insert(v);
          }

          bool visited_before = prev_stmts.find({ v, hash }) != prev_stmts.end();
          prev_stmts.insert({ v, hash });
          BipReachabilityDFS(prev_stmts, v, call_stack, hash);
          if (!visited_before) prev_stmts.erase({ v, hash });
          call_stack.push_back(branch);
          hash += std::to_string(branch) + " ";
        }
        // Else checked above
      }
      // Call stack empty, go wherever you want
      else {
      }
    }
    else if (v_branch == 0) {
      for (auto& [prev_stmt, prev_hash] : prev_stmts) {
        bip_reachability_dfs_cache[prev_stmt][prev_hash].insert(v);
      }
      bool visited_before = prev_stmts.find({ v, hash }) != prev_stmts.end();
      prev_stmts.insert({ v, hash });
      BipReachabilityDFS(prev_stmts, v, call_stack, hash);
      if (!visited_before) prev_stmts.erase({ v, hash });
    }
  }
}


void PKB::AffectsBipDFS(int start, std::string& start_hash, int u, std::vector<int>& call_stack, std::string& hash, std::string var_name,
  std::set<std::pair<int, std::string>>& visited) {

  Statement* stmt = stmt_table_.get_statement(start);
  int branch;
  if (call_stack.size() == 0) {
    branch = kNoBranch;
  }
  else {
    branch = call_stack.back();
  }

  for (auto& [v, v_branch] : cfg_bip_al_[u]) {
    // Was called from another procedure, and this edge is a returning edge that does not return to that procedure
    if (branch != kNoBranch && v_branch < 0 && v_branch != -branch) {
      continue;
    }

    // Edge goes to a new procedure
    if (v_branch > 0) {
      call_stack.push_back(v_branch);
      std::string new_call = std::to_string(v_branch) + " ";
      hash += new_call;
      if (visited.find({ v,hash }) == visited.end()) {
        visited.insert({ v,hash });
        AddStmtIfAffectedBip(start, start_hash, v, hash, var_name);
        if (!ModifiesVarName(v, var_name)) AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
      }
      call_stack.pop_back();
      hash.erase(hash.size() - new_call.size());
    }
    else if (v_branch < 0) {
      // Call stack not empty and edge goes back
      if (branch != kNoBranch) {
        // Edge goes back
        if (v_branch == -branch) {
          hash.erase(hash.size() - 1 - std::to_string(call_stack.back()).size());
          call_stack.pop_back();
          if (visited.find({ v,hash }) == visited.end()) {
            visited.insert({ v,hash });
            AddStmtIfAffectedBip(start, start_hash, v, hash, var_name);
            if (!ModifiesVarName(v, var_name)) AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
          }
          call_stack.push_back(branch);
          hash += std::to_string(branch) + " ";
        }
        // Else checked above
      }
      // Call stack empty, go wherever you want
      else {
      }
    }
    else if (v_branch == 0) {
      if (visited.find({ v,hash }) != visited.end()) continue;
      visited.insert({ v,hash });
      AddStmtIfAffectedBip(start, start_hash, v, hash, var_name);
      if (!ModifiesVarName(v, var_name)) AffectsBipDFS(start, start_hash, v, call_stack, hash, var_name, visited);
    }
  }
}

void PKB::AffectsBipStarDFS(std::set<std::pair<int, std::string>>&prev_stmts, int u, std::string &u_hash) {
  if (affects_bip_star_dfs_cache.find(u) != affects_bip_star_dfs_cache.end() && affects_bip_star_dfs_cache[u].find(u_hash) != affects_bip_star_dfs_cache[u].end()) {
    for (auto& val : affects_bip_star_dfs_cache[u][u_hash]) {
      for (auto& [prev_stmt, prev_hash] : prev_stmts) {
        affects_bip_star_dfs_cache[prev_stmt][prev_hash].insert(val);
      }
    }
    return;
  }
  get_affects_bip(u, kWild);
  for (auto [v, v_hash] : affects_bip_dfs_cache[u][u_hash]) {
    for (auto& [prev_stmt, prev_hash] : prev_stmts) {
      affects_bip_star_dfs_cache[prev_stmt][prev_hash].insert({ v, v_hash });
    }
    bool visited_before = prev_stmts.find({ v, v_hash }) != prev_stmts.end();
    prev_stmts.insert({ v, v_hash });
    AffectsBipStarDFS(prev_stmts, v, v_hash);
    if (!visited_before) prev_stmts.erase({ v, v_hash });
  }
}

void PKB::AddStmtIfAffectedBip(int start, std::string &start_hash, int v, std::string v_hash, std::string &var_name) {
  auto stmt_v = stmt_table_.get_statement(v);
  std::set<std::string>* uses = stmt_v->get_uses();
  if (stmt_v->get_kind() == NodeType::Assign && uses->find(var_name) != uses->end()) {
    affects_bip_dfs_cache[start][start_hash].insert({ v,v_hash });
  }
}

bool PKB::ModifiesVarName(int v, std::string var_name) {
  Statement* stmt_v = stmt_table_.get_statement(v);
  std::set<std::string>* modifies = stmt_v->get_modifies();
  if (stmt_v->get_kind() == NodeType::Assign || stmt_v->get_kind() == NodeType::Read) {
    if (modifies->find(var_name) != modifies->end()) return true;
  }
  return false;
}