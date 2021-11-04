#pragma once
#include <set>
#include <vector>
#include <string>
#include "entity.hpp"
#include "ast.h"

class Statement : public Entity {
 public:
  Statement(int line_no, NodeType type);

  Statement();

  ~Statement();

  // Gets the statement number.
  int get_stmt_no();
  // Gets the kind of statement.
  NodeType get_kind();
  // Gets the postfix expression string of an assignment statement.
  std::string get_expr_string();
  // Gets the variables in the expr_string_.
  std::vector<std::string> get_vars_from_expr_string();
  // Gets the name of the procedure being called, only works for NodeType::Call
  std::string get_called_proc_name();
  // Gets the name of the procedure that this statement is in
  std::string get_parent_proc();
  // Gets the last stmts of a container stmt
  std::set<int>* get_last_stmts();
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
  // Gets the variables which this statement uses.
  std::set<std::string> *get_uses();
  // Gets the variables which this statement modifies.
  std::set<std::string> *get_modifies();
  // Gets the call stack.
  std::vector<std::vector<int>> *get_call_stacks();

  // Sets the postfix expression string of this statement.
  void set_expr_string(std::string expr_string);
  // Sets the name of the procedure being called, only works for NodeType::Call
  void set_called_proc_name(std::string name);
  // Sets the name of the procedure that this statement is in
  void set_parent_proc(std::string proc_name);
  // Adds a last stmt of the stmt;
  void add_last_stmt(int last_stmt);
  
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
  // Adds a variable that this statement uses.
  void AddUses(const std::string &var_name);
  // Adds a variable that this statement modifies.
  void AddModifies(const std::string &var_name);
  // Adds a call stack, returns true if successfully added.
  bool AddCallStack(std::vector<int> *stack);

  // Prints the follows information for this statement.
  void FollowsInfo();
  // Prints the parents information for this statement.
  void ParentInfo();
  // Prints the variables used by this statement.
  void UsesInfo();
  // Prints the variables modified by this statement.
  void ModifiesInfo();

 private:
  // Statement number in program.
  int stmt_no_;
  // Postfix expression string. Currently, only valid in assignment statements.
  std::string expr_string_;
  // Kind of statement.
  NodeType kind_;
  // Name of procedure being called, only valid if of NodeType::Call
  std::string called_proc_name;
  // Name of procedure that this statement is in 
  std::string parent_proc;
  // Last statements for container statements
  std::set<int> last_stmts_;

  // For v in followers_, Follows(this, v) is true.
  std::set<int> followers_, followers_star_;
  // For v in followees_, Follows(v, this) is true.
  std::set<int> followees_, followees_star_;
  // For v in parents_, Parent(v, this) is true.
  std::set<int> parents_, parents_star_;
  // For v in children_, Parent(this, v) is true.
  std::set<int> children_, children_star_;

  // For var in uses_, Uses(this, var) is true.
  std::set<std::string> uses_;
  // For var in modifies_, Modifies(this, var) is true.
  std::set<std::string> modifies_;

  // Vector of possible call stacks to keep track of the possible states when encountering this node.
  std::vector<std::vector<int>> call_stacks_;
};
