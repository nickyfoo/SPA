#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Tables/ProcTable.h"
#include "Tables/StmtTable.h"
#include "Tables/VarTable.h"
#include "Tables/ConstTable.h"

class PKB {
 public:
  PKB(ast::Node *programRoot);

  ~PKB();

  // Populates the tables with entities and relationships from AST.
  void Initialise();

  // Adds a procedure to the procedures table.
  void AddProcedure(ast::Node *node);
  // Adds a statement to the statements table.
  void AddStatement(ast::Node *node);
  // Adds a postfix expression string to the table.
  void AddExprString(ast::Node *node);
  // Adds a variable to the variables table.
  void AddVariable(ast::Node *node);

  // Gets all procedures in the program.
  std::vector<Procedure *> GetAllProcedures();

  // Gets the total number of statements in the statement table.
  int GetNumStatements();
  // Gets all statements in the statement table.
  std::vector<Statement*> GetAllStatements();
  // Gets all statements of the given type.
  std::vector<Statement*> GetStatements(ast::Kind type);
  // Gets a statement by its corresponding line number.
  Statement* GetStatement(int line_no);

  // Gets all variables in the program.
  std::vector<Variable *> GetAllVariables();

  // Process and store Follows relationships for the AST procedure node.
  void followsProcessProcedureNode(ast::Node *node);
  // Process and store Follows relationships for the AST if node.
  void followsProcessIfNode(ast::Node *node);
  // Process and store Follows relationships for the AST while node.
  void followsProcessWhileNode(ast::Node *node);

  // Process and store Parent relationships for the AST if node.
  void parentProcessIfNode(ast::Node *node);
  // Process and store Parent relationships for the AST while node.
  void parentProcessWhileNode(ast::Node *node);

  // Prints information of statements in the statement table.
  void PrintStatements();

 private:
  // Extracts entities from the AST root node and stores them in the PKB.
  void ExtractEntities();
  // Extracts Follows/Follows_Star relationships in the AST.
  void GetFollows();
  // Extracts Parent/Parent_Star relationships in the AST.
  void GetParent();

  // Root AST node of the program.
  ast::Node *root_;
  // Table of procedures in the program.
  ProcTable proc_table_;
  // Table of statements in the program.
  StmtTable stmt_table_;
  // Table of variables in the program.
  VarTable var_table_;
  // Table of constants in the program.
  ConstTable const_table_;
};