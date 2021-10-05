#pragma once

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <map>
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

  // Tests the RHS of assignment statement against the given pattern.
  // Returns true if pattern matches.
  bool TestAssignmentPattern(Statement *statement, std::string pattern, bool is_partial_match);
  // Tests for a variable in the expression of the if statement.
  // Returns true if variable matches.
  bool TestIfWhilePattern(Statement *stmt, std::string variable);

  // Prints information of statements in the statement table.
  void PrintStatements();
  // Prints information of procedures in the procedure table.
  void PrintProcedures();
  // Prints information of variables in the variable table.
  void PrintVariables();

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

  // Root AST node of the program.
  Node *root_;
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
};
