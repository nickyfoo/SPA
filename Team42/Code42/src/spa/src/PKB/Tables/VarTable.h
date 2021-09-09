#pragma once
#include "../Entities/Variable.h"
#include "Table.hpp"
#include <map>
#include <set>
#include <vector>

class VarTable : public Table {
 public:
  VarTable();

  ~VarTable();

  // Adds a variable to the table.
  int AddVariable(const std::string &name);

  // Get total number of variables.
  int GetNumVariables();
  // Get all variables.
  std::vector<Variable *> GetAllVariables();

  // Prints information of all variables.
  void PrintVariables();

 private:
  // Table of all variables.
  std::vector<Variable> table_;
  // Table of pointers to all variables.
  std::vector<Variable *> all_variables_;
  // Table mapping each variable name to its corresponding index.
  std::map<std::string, int> name_to_index_;
};