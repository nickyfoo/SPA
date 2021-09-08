#pragma once
#include "../Entities/Variable.h"
#include <map>
#include <set>
#include <vector>

class VarTable {
 public:
  VarTable();

  ~VarTable();

  // Adds a variable to the table.
  int AddVariable(const std::string& name);

  // Get total number of variables.
  int GetNumVariables();
  // Get all variables.
  std::vector<Variable *> GetAllVariables();

  // Prints information of all variables.
  void PrintVariables();

 private:
  std::vector<Variable> table_;
  std::vector<Variable *> all_variables_;
  std::map<std::string, int> name_to_index_;
};