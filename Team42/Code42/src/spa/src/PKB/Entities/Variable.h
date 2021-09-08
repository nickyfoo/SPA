#pragma once
#include <string>

class Variable {
 public:
  Variable(int index, std::string name);

  ~Variable();

  // Gets the index mapping to the variable.
  int GetIndex();
  // Gets the name of the variable.
  std::string GetName();

 private:
  // Mapped index of variable.
  int index_;
  // Name of variable.
  std::string name_;
};