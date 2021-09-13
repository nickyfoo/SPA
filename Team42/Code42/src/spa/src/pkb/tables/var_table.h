#pragma once
#include "../entities/variable.h"
#include "table.hpp"
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
  int get_num_variables();
  // Get all variables.
  std::vector<Variable *> get_all_variables();
  // Get variable by name.
  Variable *get_variable(std::string var_name);

  // Prints information of all variables.
  void PrintVariables();
  // Prints relationship information of all variables.
  void PrintVariableDetails();

 private:
  // Table of all variables.
  std::vector<Variable> table_;
  // Table mapping each variable name to its corresponding index.
  std::map<std::string, int> name_to_index_;
};