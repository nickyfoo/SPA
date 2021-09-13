#pragma once
#include "ast.h"
#include "entity.hpp"
#include <set>

class Statement : public Entity {
 public:
  Statement(int line_no, NodeType type);

  Statement();

  ~Statement();

  // Gets the statement number.
  int get_stmt_no();
  // Gets the statement number of the given ast node.
  static int get_stmt_no(Node *node);
  // Gets the kind of statement.
  NodeType get_kind();
  // Gets the postfix expression string of an assignment statement.
  std::string get_expr_string();
  // Gets the statements which follows before this statement.
  std::set<int> *get_followers();
  // Gets the statements which follows star before this statement.
  std::set<int> *get_followers_star();
  // Gets the statements which follows after this statement.
  std::set<int> *get_followees();
  // Gets the statements which follows star after this statement.
  std::set<int> *get_followees_star();
  // Gets the statements which parents this statement.
  std::set<int> *get_parents();
  // Gets the statements which parents star this statement.
  std::set<int> *get_parents_star();
  // Gets the statements which this statement parents.
  std::set<int> *get_children();
  // Gets the statements which this statement parents star.
  std::set<int> *get_children_star();

  // Sets the postfix expression string of this statement.
  void set_expr_string(std::string expr_string);

  // Adds a statement that follows before this statement.
  void AddFollower(int line_no);
  // Adds a statement that follows_star before this statement.
  void AddFollowerStar(int line_no);
  // Adds a statement that follows after this statement.
  void AddFollowee(int line_no);
  // Adds a statement that follows_star after this statement.
  void AddFolloweeStar(int line_no);
  // Adds a statement that parents this statement.
  void AddParent(int line_no);
  // Adds a statement that parents_star this statement.
  void AddParentStar(int line_no);
  // Adds a statement that this statement parents.
  void AddChild(int line_no);
  // Adds a statement that this statement parents_star.
  void AddChildStar(int line_no);

  // Prints the follows information for this statement.
  void FollowsInfo();
  // Prints the parents information for this statement.
  void ParentInfo();

 private:
  // Statement number in program.
  int stmt_no_;
  // Postfix expression string. Currently, only valid in assignment statements.
  std::string expr_string_;
  // Kind of statement.
  NodeType kind_;

  // for v in followers_, Follows(this, v) is true.
  std::set<int> followers_, followers_star_;
  // for v in followees_, Follows(v, this) is true.
  std::set<int> followees_, followees_star_;
  // for v in parents_, Parent(v, this) is true.
  std::set<int> parents_, parents_star_;
  // for v in children_, Parent(this, v) is true.
  std::set<int> children_, children_star_;
};