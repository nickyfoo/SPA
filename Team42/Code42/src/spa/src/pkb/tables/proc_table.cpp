#include "proc_table.h"
#include <iostream>

ProcTable::ProcTable() = default;

ProcTable::~ProcTable() = default;

int ProcTable::AddProcedure(const std::string &name) {
  auto it = name_to_index_.find(name);
  if (it != name_to_index_.end()) {
    return it->second;
  }
  int pos = table_.size();
  Procedure p(name);
  table_.push_back(p);
  all_procedures_.push_back(&table_[pos]);
  name_to_index_[name] = pos;
  return pos;
}

int ProcTable::get_num_procedures() {
  return name_to_index_.size();
}

std::vector<Procedure *> ProcTable::get_all_procedures() {
  return all_procedures_;
}

Procedure *ProcTable::get_procedure(std::string &name) {
  int index = name_to_index_[name];
  return all_procedures_[index];
}

void ProcTable::PrintProcedures() {
  std::cout << "ProcTable size: " << table_.size() << '\n';
  for (Procedure p: table_) {
    std::cout << p.get_name() << '\n';
  }
}