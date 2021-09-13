#pragma once

#include <set>
#include <string>
#include <vector>
#include "entities/procedure.h"
#include "table.hpp"
#include <map>

class ProcTable : public Table {
 public:
  ProcTable();

  ~ProcTable();

  // Adds a procedure to the table.
  int AddProcedure(const std::string &name);

  // Get total number of procedures.
  int get_num_procedures();
  // Get all procedures.
  std::vector<Procedure *> get_all_procedures();
  // Gets a procedure by its procedure name.
  Procedure *get_procedure(std::string &name);

  // Prints information of all procedures.
  void PrintProcedures();

 private:
  // Table of all procedures.
  std::vector<Procedure> table_;
  // Table of pointers to all procedures.
  std::vector<Procedure *> all_procedures_;
  // Table mapping each procedure name to its corresponding index.
  std::map<std::string, int> name_to_index_;
};