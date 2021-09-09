#pragma once

#include <set>
#include <string>
#include <vector>
#include "Entities/Constant.h"
#include "Table.hpp"
#include <map>

class ConstTable : public Table {
 public:
  ConstTable();

  ~ConstTable();

  // Adds a constant to the table.
  void AddConstant(int value);

  // Get all constants.
  std::vector<Constant *> GetAllConstants();

  // Prints all constants in the table.
  void PrintConstants();

 private:
  // Table mapping each constant value to its corresponding constant.
  std::map<int, Constant> table_;
  // Table of pointers to all constants.
  std::vector<Constant *> all_constants_;
};