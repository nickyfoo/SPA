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
  int AddProcedure(const std::string &name);

  // Get total number of procedures.
  int get_num_procedures();
  // Get all procedures.
  std::vector<Procedure *> get_all_procedures();
  // Gets a procedure by its procedure name.
  Procedure *get_procedure(std::string name);

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
  enum DFS_Status {
    UNVISITED,  // Node has not been visited.
    VISITED,    // Node has been visited, but has not fully explored children.
    EXPLORED,   // Node has been fully explored, and returning.
  };

  // Table of all procedures.
  std::vector<Procedure> table_;
  // Table mapping each procedure name to its corresponding index.
  std::map<std::string, int> name_to_index_;

  // for <p1, p2> in calls_, Calls(v1, v2) is true. Stored as ints for
  // transitive closure.
  std::map<int, std::vector<int>> calls_int_, calls_star_int_;
  // for <p1, p2> in calls_, Calls(v1, v2) is true. Stored as strings.
  std::set<std::pair<std::string, std::string>> calls_, calls_star_;

  // Sets up adjacency list for procedure calls graph, where al[u] has v iff u
  // is called by v
  std::vector<std::vector<int>> SetupAL();
  // Does a DFS to fill ans with a topological ordering of the procedures.
  void DFS(int u, std::vector<std::vector<int>> &al,
           std::vector<int> &status, std::vector<int> &ans);
  // Propagates the Uses and Modifies variables that have been encountered to
  // the calling procedures.
  void PropagateUsesModifiesIndirect(
      int u, std::vector<std::vector<int>> &al, std::set<std::string> &uses,
      std::set<std::string> &modifies);
};
