#include "const_table.h"
#include <iostream>

ConstTable::ConstTable() = default;

ConstTable::~ConstTable() = default;

void ConstTable::AddConstant(int value) {
  auto it = table_.find(value);
  if (it == table_.end()) {
    Constant c(value);
    table_[value] = c;
    all_constants_.push_back(&table_[value]);
  }
}

std::vector<Constant *> ConstTable::get_all_constants() {
  return all_constants_;
}

void ConstTable::PrintConstants() {
  std::cout << "ConstTable size: " << table_.size() << '\n';
  for (auto&[k, x] : table_) {
    std::cout << x.get_value() << '\n';
  }
}