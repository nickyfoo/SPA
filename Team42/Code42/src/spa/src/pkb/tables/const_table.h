#pragma once
#include <set>
#include <string>
#include <vector>
#include <map>
#include "table.hpp"
#include "entities/constant.h"

class ConstTable : public Table {
 public:
  ConstTable();

  ~ConstTable();

  // Adds a constant to the table.
  void AddConstant(int value);

  // Get all constants.
  std::vector<Constant*> get_all_constants();

  // Prints all constants in the table.
  void PrintConstants();

 private:
  // Table mapping each constant value to its corresponding constant.
  std::map<int, Constant> table_;
};
