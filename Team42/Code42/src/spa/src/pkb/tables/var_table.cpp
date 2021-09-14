#include "var_table.h"
#include <iostream>
#include <string>

VarTable::VarTable() = default;

VarTable::~VarTable() = default;

int VarTable::AddVariable(const std::string &name) {
  auto it = name_to_index_.find(name);
  if (it != name_to_index_.end()) {
    return it->second;
  }
  int pos = table_.size();
  Variable v(pos, name);
  table_.push_back(v);
  name_to_index_[name] = pos;
  return pos;
}

int VarTable::get_num_variables() {
  return name_to_index_.size();
}


std::vector<Variable *> VarTable::get_all_variables() {
  std::vector<Variable *> ans;
  for (auto &var : table_) {
    ans.push_back(&var);
  }
  return ans;
}

Variable *VarTable::get_variable(std::string var_name) {
  return &table_[name_to_index_[var_name]];
}

void VarTable::PrintVariables() {
  std::cout << "VarTable size: " << name_to_index_.size() << '\n';
  for (Variable v : table_) {
    std::cout << v.get_name() << "\n";
  }
}

void VarTable::PrintVariableDetails() {
  for (auto &v : table_) {
    std::cout << '\n';
  }
}
