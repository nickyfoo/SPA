#include "VarTable.h"
#include <iostream>

VarTable::VarTable() = default;

VarTable::~VarTable() = default;

int VarTable::AddVariable(const std::string& name) {
  auto it = variableNameToIndex.find(name);
  if (it != variableNameToIndex.end()) {
    return it->second;
  } else {
    int pos = table_.size();
    Variable v(pos, name);
    table_.push_back(v);
    all_variables_.push_back(&table_[pos]);
    variableNameToIndex[name] = pos;
    return pos;
  }
};

int VarTable::GetNumVariables() {
  return variableNameToIndex.size();
}

std::vector<Variable *> VarTable::GetAllVariables() {
  return all_variables_;
}

void VarTable::PrintVariables() {
  std::cout << "VarTable size: " << variableNameToIndex.size() << '\n';
  for (Variable v : table_) {
    std::cout << v.GetName() << "\n";
  }
}