#include "VarTable.h"
#include <iostream>

VarTable::VarTable() = default;

VarTable::~VarTable() = default;

int VarTable::AddVariable(const std::string& name) {
  auto it = name_to_index_.find(name);
  if (it != name_to_index_.end()) {
    return it->second;
  } else {
    int pos = table_.size();
    Variable v(pos, name);
    table_.push_back(v);
    all_variables_.push_back(&table_[pos]);
    name_to_index_[name] = pos;
    return pos;
  }
};

int VarTable::GetNumVariables() {
  return name_to_index_.size();
}

std::vector<Variable *> VarTable::GetAllVariables() {
  return all_variables_;
}

void VarTable::PrintVariables() {
  std::cout << "VarTable size: " << name_to_index_.size() << '\n';
  for (Variable v : table_) {
    std::cout << v.GetName() << "\n";
  }
}