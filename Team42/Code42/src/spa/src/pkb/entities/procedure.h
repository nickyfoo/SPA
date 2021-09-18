#pragma once
#include <string>
#include <set>
#include "entity.hpp"

class Procedure : public Entity {
 public:
  explicit Procedure(std::string name);

  ~Procedure();

  // Gets the name of procedure.
  std::string get_name();
  // Gets the variables used by the procedure.
  std::set<std::string> *get_uses();
  // Gets the variables modified by the procedure.
  std::set<std::string> *get_modifies();
  // Gets the procedures called by the procedure.
  std::set<std::string> *get_calls();
  // Gets the procedures called star by the procedure.
  std::set<std::string> *get_calls_star();
  // Gets the procedures called by the procedure.
  std::set<std::string> *get_callers();
  // Gets the procedures called star by the procedure.
  std::set<std::string> *get_callers_star();

  // Adds a variable that this procedure uses.
  void AddUses(std::string var_name);
  // Adds a variable that this procedure modifies.
  void AddModifies(std::string var_name);
  // Adds a procedure that this procedure calls.
  void AddCalls(std::string proc_name);
  // Adds a procedure that this procedure calls_star.
  void AddCallsStar(std::string proc_name);
  // Adds a procedure that calls this procedure.
  void AddCallers(std::string proc_name);
  // Adds a procedure that calls_star this procedure.
  void AddCallersStar(std::string proc_name);

  // Prints the calls information for this statement.
  void CallsInfo();
  // Prints the variables used by this procedure.
  void UsesInfo();
  // Prints the variables modified by this procedure.
  void ModifiesInfo();

 private:
  // Name of procedure.
  std::string name_;
  // Variables used by the procedure.
  std::set<std::string> uses_;
  // Variables modified by the procedure.
  std::set<std::string> modifies_;
  // Procedures called by this procedure.
  std::set<std::string> calls_, calls_star_;
  // Procedures that call this procedure.
  std::set<std::string> callers_, callers_star_;
};
