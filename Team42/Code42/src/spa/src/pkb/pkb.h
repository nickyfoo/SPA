#pragma once

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <stack>
#include "tables/proc_table.h"
#include "tables/stmt_table.h"
#include "tables/var_table.h"
#include "tables/const_table.h"
#include "pattern_manager.h"
#include "pkb_exception.h"

class PKB {
 public:
  PKB();

  ~PKB();

  // Value representing wild card.
  inline static const int kWild = 0;
  // Value representing no branch.
  inline static const int kNoBranch = 0;

  // Adds a procedure to the procedures table.
  void AddProcedure(Node *node, const std::vector<Node *> &ancestor_list);
  // Adds a statement to the statements table.
  void AddStatement(Node *node, const std::vector<Node *> &ancestor_list);
  // Adds a postfix expression string to the table.
  void AddExprString(Node *node, const std::vector<Node *> &ancestor_list);
  // Adds a variable to the variables table.
  void AddVariable(Node *node, const std::vector<Node *> &ancestor_list);
  // Adds a constant to the variables table.
  void AddConstant(Node *node, const std::vector<Node *> &ancestor_list);

  // Gets the total number of procedures in the procedure table
  int get_num_procedures();
  // Gets all procedures in the program.
  virtual std::vector<Procedure *> get_all_procedures();
  // Gets a procedure by its procedure name.
  virtual Procedure *get_procedure(std::string &name);

  // Gets the total number of statements in the statement table.
  int get_num_statements();
  // Gets all statements in the statement table.
  virtual std::vector<Statement *> get_all_statements();
  // Gets all statements of the given type.
  std::vector<Statement *> get_statements(NodeType type);
  // Gets a statement by its corresponding line number.
  virtual Statement *get_statement(int line_no);

  // Gets all variables in the program.
  std::vector<Variable *> get_all_variables();

  // Gets all constants in the program.
  std::vector<Constant *> get_all_constants();

  std::map<int, std::set<int>> *get_cfg_al();
  std::map<int, std::set<int>> *get_reverse_cfg_al();
  std::map<int, std::set<std::pair<int, int>>> *get_cfg_bip_al();
  std::map<int, std::set<std::pair<int, int>>> *get_reverse_cfg_bip_al();

  // Gets Next(a,b) relation. if a==kWild or b==kWild, it is treated as a wildcard.
  // Does a check for valid stmt line input, or kWild, and returns empty set if invalid.
  std::set<std::pair<int, int>> *get_next(int a, int b);
  // Gets Next*(a,b) relation. if a==kWild or b==kWild, it is treated as a wildcard.
  // Does a check for valid stmt line input, or kWild, and returns empty set if invalid.
  std::set<std::pair<int, int>> *get_next_star(int a, int b);

  // Gets Affects(a,b) relation. if a==0 or b==0, it is treated as a wildcard.
  // Does a check for valid stmt line input, or kWild, and returns empty set if invalid.
  std::set<std::pair<int, int>> *get_affects(int a, int b);
  // Gets Affects*(a,b) relation. if a==0 or b==0, it is treated as a wildcard.
  // Does a check for valid stmt line input, or kWild, and returns empty set if invalid.
  std::set<std::pair<int, int>> *get_affects_star(int a, int b);

  // Gets NextBip(a,b) relation. if a==kWild or b==kWild, it is treated as a wildcard.
  // Does a check for valid stmt line input, or kWild, and returns nullptr if invalid.
  std::set<std::pair<int, int>> *get_next_bip(int a, int b);
  // Gets NextBip*(a,b) relation. if a==kWild or b==kWild, it is treated as a wildcard.
  // Does a check for valid stmt line input, or kWild, and returns empty set if invalid.
  std::set<std::pair<int, int>> *get_next_bip_star(int a, int b);

  // Gets AffectsBip(a,b) relation. if a==0 or b==0, it is treated as a wildcard.
  // Does a check for valid stmt line input, or kWild, and returns empty set if invalid.
  std::set<std::pair<int, int>> *get_affects_bip(int a, int b);
  // Gets AffectsBip*(a,b) relation. if a==0 or b==0, it is treated as a wildcard.
  // Does a check for valid stmt line input, or kWild, and returns empty set if invalid.
  std::set<std::pair<int, int>> *get_affects_bip_star(int a, int b);

  // Tests the RHS of assignment statement against the given pattern.
  // Returns true if pattern matches.
  bool TestAssignmentPattern(Statement *statement, const std::string &pattern, bool is_partial_match);
  // Tests for a variable in the expression of the if statement.
  // Returns true if variable matches.
  bool TestIfWhilePattern(Statement *stmt, const std::string &variable);

  // Clears cache for Next and Affects
  void ClearNextAffectsCache();
  // Checks if cache for Next and Affects is empty
  bool NextAffectsCacheIsEmpty();
  // Clears cache for Next and Affects Bip
  void ClearNextAffectsBipCache();
  // Checks if cache for Next and Affects Bip is empty
  bool NextAffectsBipCacheIsEmpty();

  // Prints information of statements in the statement table.
  void PrintStatements();
  // Prints information of procedures in the procedure table.
  void PrintProcedures();
  // Prints information of variables in the variable table.
  void PrintVariables();
  // Prints CFGAL.
  void PrintCFGAL();
  // Prints CFGBIPAL.
  void PrintCFGBipAL();

 private:
  friend class DesignExtractor;
  // DFS to check reachability for Next and Affects* relationship
  void ReachabilityDFS(int start, int u, std::vector<std::vector<int>> &d,
                       std::map<int, std::set<int>> &al);
  // DFS to check reachability for Affects relationship.
  void AffectsDFS(int start, int u, const std::string &var_name, std::vector<bool> &visited,
                  std::vector<std::vector<int>> &d, bool &found);
  // DFS to check reachability for Affects* relationship
  // If forward relation is true, this method propagates forward in terms of Affects*(a,b)
  void AffectsStarBFS(int start, std::vector<bool> &visited,
                      bool forward_relation);

  // DFS to check reachability for NextBip and AffectsBip* relationship
  void BipReachabilityDFS(std::set<std::pair<int,
                               std::string>> &visited,
                               int u, 
                               std::string &u_hash,
                               int start, 
                               std::string &start_hash,
                               std::vector<int> &call_stack);
  // DFS to check reachability for AffectsBip relationship.
  // If target is not kWild, supports fast termination to save on unnecessary computations.
  void AffectsBipDFS(int start,
                     std::string &start_hash,
                     int u,
                     std::vector<int> &call_stack,
                     std::string &hash,
                     std::string var_name,
                     std::set<std::pair<int, std::string>> &visited);
  // Adds a stmt to affects_bip_dfs_cache if affected, else does nothing
  void AddStmtIfAffectedBip(int start,
                            std::string &start_hash,
                            int v,
                            const std::string &v_hash,
                            std::string &var_name);
  // Returns true if v is an assign stmt that modifies var_name
  bool ModifiesVarName(int v, const std::string &var_name);
  // DFS to check reachability for AffectsBip* relationship
  void AffectsBipStarDFS(std::set<std::pair<int, std::string>> &visited,
                         int u,
                         std::string &u_hash,
                         int start,
                         std::string &start_hash);
  // Utility to hash a call_stack
  std::string CallStackToString(std::vector<int> *call_stack);

  // Root AST node of the program.
  PatternManager pattern_manager_;
  // Table of procedures in the program.
  ProcTable proc_table_;
  // Table of statements in the program.
  StmtTable stmt_table_;
  // Table of variables in the program.
  VarTable var_table_;
  // Table of constants in the program.
  ConstTable const_table_;

  // Adjacency List of CFG for Next and Affects
  std::map<int, std::set<int>> cfg_al_;
  // Reverse Adjacency List of CFG for Next and Affects.
  std::map<int, std::set<int>> reverse_cfg_al_;
  // Adjacency List of CFG for NextBip and AffectsBip, u -> {v, branch}
  // branch is positive stmt_no if branching out from u to v
  // branch is negative stmt_no if branching back from u to v
  std::map<int, std::set<std::pair<int, int>>> cfg_bip_al_;
  // Reverse Adjacency List of CFG for NextBip and AffectsBip.
  // branch is positive stmt_no if branching out from u to v
  // branch is negative stmt_no if branching back from u to v
  std::map<int, std::set<std::pair<int, int>>> reverse_cfg_bip_al_;

  // Cache for Next
  std::map<int, std::map<int, std::set<std::pair<int, int>>>> next_cache;
  // Cache for Next*
  std::map<int, std::map<int, std::set<std::pair<int, int>>>> next_star_cache;
  // Cache for Affects
  std::map<int, std::map<int, std::set<std::pair<int, int>>>> affects_cache;
  // Cache for Affects*
  std::map<int, std::map<int, std::set<std::pair<int, int>>>> affects_star_cache;

  // Cache for NextBip
  std::map<int, std::map<int, std::set<std::pair<int, int>>>> next_bip_cache;
  // Cache for BipReachabilityDfs
  std::map<int, std::map<std::string, std::set<int>>> bip_reachability_dfs_cache;
  // Cache for NextBip*
  std::map<int, std::map<int, std::set<std::pair<int, int>>>> next_bip_star_cache;
  // Cache for AffectsBip
  std::map<int, std::map<int, std::set<std::pair<int, int>>>> affects_bip_cache;
  // Cache for AffectsBipDFS
  std::map<int, std::map<std::string,
                         std::set<std::pair<int, std::string>>>> affects_bip_dfs_cache;
  // Cache for AffectsBip*
  std::map<int, std::map<int,
                         std::set<std::pair<int, int>>>> affects_bip_star_cache;
  // Cache for AffectsBipStarDFS
  std::map<int, std::map<std::string,
                         std::set<std::pair<int, std::string>>>> affects_bip_star_dfs_cache;
};
