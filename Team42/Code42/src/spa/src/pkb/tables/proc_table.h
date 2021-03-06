#pragma once

#include <set>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "entities/procedure.h"
#include "table.hpp"

class ProcTable : public Table {
 public:
  ProcTable();

  ~ProcTable();

  // Adds a procedure to the table.
  int AddProcedure(const std::string &name, int stmt_no);

  // Get total number of procedures.
  int get_num_procedures();
  // Get all procedures.
  std::vector<Procedure *> get_all_procedures();
  // Gets a procedure by its procedure name.
  virtual Procedure *get_procedure(const std::string &name);

  // Gets Calls relationship from Procedures in preparation to get
  // transitive closure.
  void ProcessCalls();
  // Gets Calls_Star relationship using transitive closure.
  void ProcessCallsStar();
  // Gets Uses and Modifies relationship using the call graph and propagation
  void ProcessUsesModifiesIndirect();

  // Prints information of all procedures.
  void PrintProcedures();
  // Prints relationship information of all procedure.
  void PrintProcedureDetails();

 private:
  enum DfsStatus {
    kUnvisited,  // Node has not been visited.
    kVisited,    // Node has been visited, but has not fully explored children.
    kExplored,   // Node has been fully explored, and returning.
  };

  // Table of all procedures.
  std::vector<Procedure> table_;
  // Table mapping each procedure name to its corresponding index.
  std::map<std::string, int> name_to_index_;

  // for <v1, v2> in calls_, Calls(v1, v2) is true. Stored as ints for
  // transitive closure.
  std::map<int, std::set<int>> calls_;

  // Sets up adjacency list for procedure calls graph, where al[u] has v iff u
  // is called by v
  std::vector<std::vector<int>> SetupAL();
  // Does a DFS to fill ans with a topological ordering of the procedures.
  void ToposortProcsDFS(int u, std::vector<std::vector<int>> &al,
                        std::vector<int> &status, std::vector<int> &ans);
  // Propagates the Uses and Modifies variables that have been encountered to
  // the calling procedures.
  void PropagateUsesModifiesIndirect(
      int u, std::vector<std::vector<int>> &al, std::set<std::string> uses,
      std::set<std::string> modifies);
};
