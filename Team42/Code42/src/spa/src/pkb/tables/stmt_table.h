#pragma once
#include <map>
#include <set>
#include <vector>
#include <utility>
#include "entities/statement.h"
#include "table.hpp"

class StmtTable : public Table {
 public:
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

  // Passes over table_ to populate type_to_statement_;
  void CategorizeStatements();

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
  

  // Gets Next relationship from Statements in preparation to get
  // transitive closure.
  void ProcessNext();
  // Gets Next_Star relationship using transitive closure.
  void ProcessNextStar();

  // Gets Affects relationship from Statements in preparation to get transitive closure.
  void ProcessAffects();
  // Gets Affects_Star relationship from Statements in preparation to get transitive closure.
  void ProcessAffectsStar();


  // Prints entity information of all statements.
  void PrintStatements();
  // Prints relationship information of all statements.
  void PrintStatementDetails();

 private:
  // Number of statements in the table.
  int num_statements_ = 0;
  // Table mapping each line number to its corresponding statement.
  std::map<int, Statement> table_;
  // Table mapping each statement type to a list of its corresponding
  // statements.
  std::map<NodeType, std::vector<Statement *>> type_to_statement_;
  // for <v1, v2> in follows_, Follows(v1, v2) is true.
  std::set<std::pair<int, int>> follows_, follows_star_;
  // for <v1, v2> in parent_, Parent(v1, v2) is true.
  std::set<std::pair<int, int>> parent_, parent_star_;

  // for <v1, v2> in next_, Next(v1, v2) is true.
  std::set<std::pair<int, int>> next_, next_star_;

  // for <v1, v2> in affects_[var_name], Affects(v1, v2) is true, where var_name is the variable being modified by v1.
  std::set<std::pair<int, int>> affects_, affects_star_;

};
