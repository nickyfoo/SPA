#pragma once
#include "entities/statement.h"
#include "table.hpp"
#include <map>
#include <set>
#include <vector>

class StmtTable : public Table {
 public:
  // List of valid kinds of statements
  inline static const std::vector<NodeType> kValidStmts = {
      NodeType::Assign,
      NodeType::If,
      NodeType::While,
      NodeType::Read,
      NodeType::Print,
      NodeType::Call
  };

  StmtTable();

  ~StmtTable();

  // Adds a statement to the table.
  void AddStatement(Node *node);

  // Get total number of statements.
  int get_num_statements();
  // Gets all statements.
  std::vector<Statement *> get_all_statements();
  // Gets all statements of the given type.
  std::vector<Statement *> get_statements(NodeType type);
  // Gets the statement by line number.
  Statement *get_statement(int line_no);

  // Gets Follows relationship from Statements in preparation to get
  // transitive closure.
  void ProcessFollows();
  // Gets Follows_Star relationship using transitive closure.
  void ProcessFollowsStar();
  // Gets Parents relationship from Statements in preparation to get
  // transitive closure.
  void ProcessParent();
  // Gets Parents_Star relationship using transitive closure.
  void ProcessParentStar();

  // Prints entity information of all statements.
  void PrintStatements();
  // Prints relationship information of all statements.
  void PrintStatementDetails();

 private:
  // Number of statements in the table.
  int num_statements_;
  // Table mapping each line number to its corresponding statement.
  std::map<int, Statement> table_;
  // Table of all statements.
  std::vector<Statement *> all_statements_;
  // Table mapping each statement type to a list of its corresponding
  // statements.
  std::map<NodeType, std::vector<Statement *>> type_to_statement_;
  // for <v1, v2> in follows_, Follows(v1, v2) is true.
  std::set<std::pair<int, int>> follows_, follows_star_;
  // for <v1, v2> in parent_, Parent(v1, v2) is true.
  std::set<std::pair<int, int>> parent_, parent_star_;
};