#pragma once

#include <set>
#include <string>
#include <vector>
#include "Entities/Procedure.h"
#include <map>

class ProcTable {
 public:
  ProcTable();

  ~ProcTable();

  // Adds a procedure to the table.
  int AddProcedure(const std::string& name);

  // Get total number of procedures.
  int GetNumProcedures();
  // Get all procedures.
  std::vector<Procedure *> GetAllProcedures();

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