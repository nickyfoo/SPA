#include "ConstTable.h"

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

std::vector<Constant *> ConstTable::GetAllConstants() {
  return all_constants_;
}
