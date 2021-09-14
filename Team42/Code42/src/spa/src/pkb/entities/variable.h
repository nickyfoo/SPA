#pragma once
#include <string>
#include <set>
#include "entity.hpp"

class Variable : public Entity {
 public:
  Variable(int index, std::string name);

  ~Variable();

  // Gets the index mapping to the variable.
  int get_index();
  // Gets the name of the variable.
  std::string get_name();

  // Adds a statement that uses this variable.
  void AddStmtUsing(int stmt_no);
  // Adds a statement that modifies this variable.
  void AddStmtModifying(int stmt_no);
  // Adds a procedure that uses this variable.
  void AddProcUsing(std::string proc_name);
  // Adds a procedure that modifies this variable.
  void AddProcModifying(std::string proc_name);

  // Gets the statement numbers using this variable.
  std::set<int> *get_stmts_using();
  // Gets the statement numbers modifying this variable.
  std::set<int> *get_stmts_modifying();
  // Gets the procedure names using this variable.
  std::set<std::string> *get_procs_using();
  // Gets the procedure names modifying this variable.
  std::set<std::string> *get_procs_modifying();

  // Prints the variables used by this statement.
  void UsesInfo();
  // Prints the variables modified by this statement.
  void ModifiesInfo();


 private:
  // Mapped index of variable.
  int index_;
  // Name of variable.
  std::string name_;

  // For s in stmts_using_, Uses(s, this) is true.
  std::set<int> stmts_using_;
  // For s in stmts_modifying_, Modifies(s, this) is true.
  std::set<int> stmts_modifying_;
  // For p in procs_using_, Uses(p, this) is true.
  std::set<std::string> procs_using_;
  // For p in procs_modifying_, Modifies(p, this) is true.
  std::set<std::string> procs_modifying_;
};
