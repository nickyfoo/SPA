#include "proc_table.h"
#include "../pkb_exception.h"
#include <iostream>

ProcTable::ProcTable() = default;

ProcTable::~ProcTable() = default;

int ProcTable::AddProcedure(const std::string &name, int stmt_no) {
  auto it = name_to_index_.find(name);
  if (it != name_to_index_.end()) {
    return it->second;
  }
  int pos = table_.size();
  Procedure p(name, stmt_no);
  table_.push_back(p);
  name_to_index_[name] = pos;
  return pos;
}

int ProcTable::get_num_procedures() {
  return name_to_index_.size();
}

std::vector<Procedure *> ProcTable::get_all_procedures() {
  std::vector<Procedure *> ans;
  for (auto &proc : table_) {
    ans.push_back(&proc);
  }
  return ans;
}

Procedure *ProcTable::get_procedure(const std::string &name) {
  if (name_to_index_.find(name) == name_to_index_.end()) {
    return nullptr;
  }
  return &table_[name_to_index_[name]];
}

void ProcTable::ProcessCalls() {
  for (Procedure &p : table_) {
    for (auto &proc_name : *p.get_calls()) {
      calls_[name_to_index_[p.get_name()]].insert(name_to_index_[proc_name]);
    }
  }
}

void ProcTable::ProcessCallsStar() {
  int n = table_.size();
  std::vector<std::vector<int>> d = GetTransitiveClosure(calls_, n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (d[i][j] == 0) continue;
      table_[i].AddCallsStar(table_[j].get_name());
      table_[j].AddCallersStar(table_[i].get_name());
    }
  }
}

void ProcTable::PrintProcedures() {
  std::cout << "ProcTable size: " << table_.size() << '\n';
  for (Procedure p : table_) {
    std::cout << p.get_name() << ' ' << p.get_stmt_no() << '\n';
  }
}

void ProcTable::PrintProcedureDetails() {
  for (auto &p : table_) {
    p.CallsInfo();
    p.UsesInfo();
    p.ModifiesInfo();
    std::cout << '\n';
  }
}

std::vector<std::vector<int>> ProcTable::SetupAL() {
  std::vector<std::vector<int>> al(table_.size());
  for (int i = 0; i < table_.size(); i++) {
    for (auto &caller_name : *(table_[i].get_callers())) {
      al[i].push_back(name_to_index_[caller_name]);
    }
  }
  return al;
}

void ProcTable::ToposortProcsDFS(int u, std::vector<std::vector<int>> &al,
                                 std::vector<int> &status, std::vector<int> &ans) {
  status[u] = kVisited;
  for (auto &v : al[u]) {
    if (status[v] == kUnvisited) {
      ToposortProcsDFS(v, al, status, ans);
    } else if (status[v] == kVisited) {
      throw PKBException("Cyclic procedure calls detected");
    }
  }
  status[u] = kExplored;
  ans.push_back(u);
}

void ProcTable::PropagateUsesModifiesIndirect(int u,
                                              std::vector<std::vector<int>> &al,
                                              std::set<std::string> uses,
                                              std::set<std::string> modifies) {
  for (auto &var_name : *(table_[u].get_uses())) {
    uses.insert(var_name);
  }
  for (auto &var_name : uses) {
    table_[u].AddUses(var_name);
  }
  for (auto &var_name : *(table_[u].get_modifies())) {
    modifies.insert(var_name);
  }
  for (auto &var_name : modifies) {
    table_[u].AddModifies(var_name);
  }

  for (auto &v : al[u]) {
    PropagateUsesModifiesIndirect(v, al, uses, modifies);
  }
}

void ProcTable::ProcessUsesModifiesIndirect() {
  std::vector<std::vector<int>> adj_list = SetupAL();
  std::vector<int> ans_int;
  std::vector<int> status(table_.size(), 0);
  for (int i = 0; i < table_.size(); i++) {
    if (status[i] == kUnvisited) {
      ToposortProcsDFS(i, adj_list, status, ans_int);
    }
  }
  reverse(ans_int.begin(), ans_int.end());
  for (auto &u : ans_int) {
    std::set<std::string> uses, modifies;
    PropagateUsesModifiesIndirect(u, adj_list, uses, modifies);
  }
}
