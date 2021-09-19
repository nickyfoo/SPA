#include "const_table.h"
#include <iostream>
#include <sstream>

ConstTable::ConstTable() = default;

ConstTable::~ConstTable() = default;

void ConstTable::AddConstant(std::string value) {
  auto it = table_.find(value);
  if (it == table_.end()) {
    Constant c(value);
    table_[value] = c;
  }
}

std::vector<Constant *> ConstTable::get_all_constants() {
  std::vector<Constant *> ans;
  for (auto &[stmt_no, stmt] : table_) {
    ans.push_back(&stmt);
  }
  return ans;
}

void ConstTable::PrintConstants() {
  std::cout << "ConstTable size: " << table_.size() << '\n';
  for (auto&[k, x] : table_) {
    std::cout << x.get_value() << '\n';
  }
}
