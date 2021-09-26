#pragma once

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "tables/proc_table.h"
#include "tables/stmt_table.h"
#include "tables/var_table.h"
#include "tables/const_table.h"
#include "pattern_manager.h"
#include "pkb_exception.h"

class PKB {
 public:
   PKB();
  explicit PKB(Node *programRoot);

  ~PKB();

  // Adds a procedure to the procedures table.
  void AddProcedure(Node *node, std::vector<Node *> ancestor_list);
  // Adds a statement to the statements table.
  void AddStatement(Node *node, std::vector<Node *> ancestor_list);
  // Adds a postfix expression string to the table.
  void AddExprString(Node *node, std::vector<Node *> ancestor_list);
  // Adds a variable to the variables table.
  void AddVariable(Node *node, std::vector<Node *> ancestor_list);
  // Adds a constant to the variables table.
  void AddConstant(Node *node, std::vector<Node *> ancestor_list);

  // Gets the total number of procedures in the procedure table
  int get_num_procedures();
  // Gets all procedures in the program.
  std::vector<Procedure *> get_all_procedures();
  // Gets a procedure by its procedure name.
  Procedure *get_procedure(std::string &name);

  // Gets the total number of statements in the statement table.
  int get_num_statements();
  // Gets all statements in the statement table.
  std::vector<Statement *> get_all_statements();
  // Gets all statements of the given type.
  std::vector<Statement *> get_statements(NodeType type);
  // Gets a statement by its corresponding line number.
  Statement *get_statement(int line_no);

  // Gets all variables in the program.
  std::vector<Variable *> get_all_variables();

  // Gets all constants in the program.
  std::vector<Constant *> get_all_constants();

  std::map<int, std::set<int>> *get_cfgal();
  std::map<int, std::set<int>>* get_reverse_cfgal();

  // Gets Next(a,b) relation. if a==0 or b==0, it is treated as a wildcard.
  std::set<std::pair<int, int>> get_next(int a, int b);
  // Gets Next*(a,b) relation. if a==0 or b==0, it is treated as a wildcard.
  std::set<std::pair<int, int>> get_next_star(int a, int b);

  // Tests the RHS of assignment statement against the given pattern.
  // Returns true if pattern matches.
  static bool TestAssignmentPattern(Statement *statement, std::string pattern, bool is_partial_match);

  // Prints information of statements in the statement table.
  void PrintStatements();
  // Prints information of procedures in the procedure table.
  void PrintProcedures();
  // Prints information of variables in the variable table.
  void PrintVariables();
  // Prints CFGAL.
  void PrintCFGAL();

 private:
  // Populates the tables with entities and relationships from AST.
  void Initialise();
  // Extracts entities from the AST root node and stores them in the pkb.
  void ExtractEntities();
  // Extracts Follows/Follows_Star relationships in the AST.
  void ExtractFollows();
  // Extracts Parent/Parent_Star relationships in the AST.
  void ExtractParent();
  // Extracts direct Uses relationships in the AST.
  void ExtractUsesModifies();
  // Extracts Calls/Calls* relationships in the AST.
  void ExtractCalls();
  // Stores adjacency list into CFGAL_
  void ExtractCFG();
  // Extracts Next/Next* relationships in the CFG.
  void ExtractNext();
  // Extracts Affects/Affects* relationships in the CFG.
  void ExtractAffects();

  // Updates procs_using_ and procs_modifying_ in var_table_.
  void UpdateVarTableWithProcs();

  // Process and store Follows relationships for the AST procedure node.
  void FollowsProcessProcedureNode(Node *node);
  // Process and store Follows relationships for the AST if node.
  void FollowsProcessIfNode(Node *node);
  // Process and store Follows relationships for the AST while node.
  void FollowsProcessWhileNode(Node *node);

  // Process and store Parent relationships for the AST if node.
  void ParentProcessIfNode(Node *node);
  // Process and store Parent relationships for the AST while node.
  void ParentProcessWhileNode(Node *node);

  // Process and store Uses/Modifies relationships for the AST assign node.
  void UsesModifiesProcessAssignNode(Node *node, std::vector<Node *> &ancestorList);
  // Process and store Uses/Modifies relationships for the AST if node.
  void UsesModifiesProcessIfNode(Node *node, std::vector<Node *> &ancestorList);
  // Process and store Uses/Modifies relationships for the AST while node.
  void UsesModifiesProcessWhileNode(Node *node, std::vector<Node *> &ancestorList);
  // Process and store Uses/Modifies relationships for the AST read node.
  void UsesModifiesProcessReadNode(Node *node, std::vector<Node *> &ancestorList);
  // Process and store Uses/Modifies relationships for the AST print node.
  void UsesModifiesProcessPrintNode(Node *node, std::vector<Node *> &ancestorList);

  // Process and store Calls relationships for the AST call node.
  void CallsProcessCallNode(Node *node, std::vector<Node *> &ancestorList);

  // Recursively gets the last stmts of a statement.
  std::set<int> PKB::LastStmts(StatementNode *node);
  // Process and store the AST procedure node into the CFG.
  void CFGProcessProcedureNode(Node *node);
  // Process and store the AST if node into the CFG.
  void CFGProcessIfNode(Node *node);
  // Process and store the AST while node into the CFG.
  void CFGProcessWhileNode(Node *node);
  
  void NextDFS(int start, int u, std::vector<std::vector<int>>& d, std::map<int, std::set<int>>& al);

  // Process and store Affects relationships for the stmt_no.
  void ProcessAffectsForStatement(int stmt_no);
  void ProcessAffectsForStatementDFS(int u, int stmt_no, std::string var_name, std::vector<bool>& visited);

  // Root AST node of the program.
  Node *root_;
  // Table of procedures in the program.
  ProcTable proc_table_;
  // Table of statements in the program.
  StmtTable stmt_table_;
  // Table of variables in the program.
  VarTable var_table_;
  // Table of constants in the program.
  ConstTable const_table_;
  // Adjacency List of CFG for Next and Affects.
  std::map<int, std::set<int>> CFGAL_;
  // Reverse Adjacency List of CFG for Next and Affects.
  std::map<int, std::set<int>> ReverseCFGAL_;
  
  // Infinite value.
  inline static const int kInf = 1e9;
};
