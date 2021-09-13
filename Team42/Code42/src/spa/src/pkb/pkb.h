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

class PKB {
 public:
  explicit PKB(Node *programRoot);

  ~PKB();

  // Adds a procedure to the procedures table.
  void AddProcedure(Node *node);
  // Adds a statement to the statements table.
  void AddStatement(Node *node);
  // Adds a postfix expression string to the table.
  void AddExprString(Node *node);
  // Adds a variable to the variables table.
  void AddVariable(Node *node);
  // Adds a constant to the variables table.
  void AddConstant(Node *node);

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

  // Prints information of statements in the statement table.
  void PrintStatements();

 private:
  // Populates the tables with entities and relationships from AST.
  void Initialise();
  // Extracts entities from the AST root node and stores them in the pkb.
  void ExtractEntities();
  // Extracts Follows/Follows_Star relationships in the AST.
  void GetFollows();
  // Extracts Parent/Parent_Star relationships in the AST.
  void GetParent();

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
};