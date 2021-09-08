#pragma once
#include <string>
#include <set>

class Procedure {
 public:
  Procedure(std::string name);

  ~Procedure();

  // Gets the name of procedure.
  std::string GetName();

 private:
  // Name of procedure.
  std::string name_;
};